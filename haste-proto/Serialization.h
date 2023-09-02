#pragma once

#include "json.hpp"

#include "ExpandedVariant.h"

namespace r0 {

class WriterVisitor {
public:
	class StateGuard {
	public:
		StateGuard(WriterVisitor& writer);
		~StateGuard();
	private:
		WriterVisitor& writer_;
		nlohmann::json* current_ = nullptr;
	};

	WriterVisitor();

	template<typename T>
	nlohmann::json Write(const T& value) {
		VisitValue(value);
		return value_;
	}

	template<typename T>
	void Visit(const T& value, const char* name) {
		VisitField(value, name);
	}

private:
	nlohmann::json& Current();

	nlohmann::json value_;

	nlohmann::json* current_ = &value_;

	template<typename T>
	void VisitField(const T& value, const char* name) {
		StateGuard guard{ *this };
		current_ = &Current()[name];
		VisitValue(value);
	}

	void VisitValue(int value);
	void VisitValue(const std::string& value);

	// array
	template<typename T>
	void VisitValue(const std::vector<T>& value) {
		Current() = nlohmann::json::value_t::array;
		for (const T& item : value) {
			StateGuard guard{ *this };
			current_ = &Current().emplace_back();
			VisitValue(item);
		}
	}

	// object
	template<typename T>
	void VisitValue(const T& value) {
		Current() = nlohmann::json::value_t::object;
		T::ApplyVisitor(value, *this);
	}

	// variant
	template<typename... Ts>
	void VisitValue(const ExpandedVariant<Ts...>& value) {
		Current() = nlohmann::json::value_t::object;
		Current()["_which_"] = value.which;

		StateGuard guard{ *this };
		current_ = &Current()["_fields_"];

		value.Visit<void>(
			[this](auto subValue) {
				VisitValue(subValue);
			}
		);
	}
};

template<typename T>
nlohmann::json Write(const T& data) {
	return WriterVisitor{}.Write(data);
}

} // namespace r0