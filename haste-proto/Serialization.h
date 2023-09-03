#pragma once

#include "tinyformat.h"

#include "json.hpp"

#include "ExpandedVariant.h"

namespace r0 {

class WriterVisitor {
public:
	WriterVisitor();

	template<typename T>
	nlohmann::json Write(const T& value) && {
		root_ = nlohmann::json::value_t::null;
		VisitValue(value);
		return std::move(root_);
	}

	template<typename T>
	void Visit(const T& value, const char* name) {
		VisitField(value, name);
	}

private:
	class StateGuard {
	public:
		StateGuard(WriterVisitor& writer);
		~StateGuard();
	private:
		WriterVisitor& writer_;
		nlohmann::json* current_ = nullptr;
	};

	nlohmann::json& Current();
	nlohmann::json root_;
	nlohmann::json* current_ = &root_;

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

		value.template Visit<void>(
			[this](const auto& subValue) {
				VisitValue(subValue);
			}
		);
	}
};

class ReaderVisitor {
public:
	ReaderVisitor(const nlohmann::json& root);

	template<typename T>
	bool Read(T& value) {
		VisitValue(value);
		return hasError_;
	}

	template<typename T>
	void Visit(T& value, const char* name) {
		VisitField(value, name);
	}

private:
	class StateGuard {
	public:
		StateGuard(ReaderVisitor& reader);
		~StateGuard();
	private:
		ReaderVisitor& reader_;
		const nlohmann::json* current_ = nullptr;
	};

	const nlohmann::json& Current();
	const nlohmann::json& root_;
	const nlohmann::json* current_ = &root_;
	bool hasError_ = false;

	template<typename T>
	void VisitField(T& value, const char* name) {
		StateGuard guard{ *this };
		current_ = &Current()[name];
		VisitValue(value);
	}

	void ReportError(const char* error);

	void VisitValue(int& value);
	void VisitValue(std::string& value);

	// array
	template<typename T>
	void VisitValue(std::vector<T>& value) {
		if (!Current().is_array()) {
			ReportError("Reader: not an array");
			return;
		}

		for (const auto& arrayItem : Current()) {
			StateGuard guard{ *this };
			current_ = &arrayItem;
			VisitValue(value.emplace_back());
		}
	}

	// object
	template<typename T>
	void VisitValue(T& value) {
		if (!Current().is_object()) {
			ReportError("Reader: not an object");
			return;
		}

		T::ApplyVisitor(value, *this);
	}

	// variant
	template<typename... Ts>
	void VisitValue(ExpandedVariant<Ts...>& value) {
		if (!Current().is_object()) {
			ReportError("Reader: variant not an object");
			return;
		}

		if (!Current().contains("_which_")) {
			ReportError("Reader: variant has no _which_ field");
			return;
		}

		if (!Current().contains("_fields_")) {
			ReportError("Reader: variant has no _fields_ field");
			return;
		}

		auto whichField = Current()["_which_"];
		auto fieldsField = Current()["_fields_"];

		if (!whichField.is_number_integer()) {
			ReportError("Reader: variant _which_ is not an integer");
			return;
		}

		auto which = whichField.get<int>();

		if (which < 0 || which >= sizeof...(Ts)) {
			ReportError("Reader: variant _which_ fields is out of bounds");
			return;
		}

		value.which = which;

		StateGuard guard{ *this };
		current_ = &fieldsField;

		value.template Visit<void>(
			[this](auto& subValue) {
				VisitValue(subValue);
			}
		);
	}
};

template<typename T>
nlohmann::json Write(const T& value) {
	return WriterVisitor{}.Write(value);
}

template<typename T>
bool Read(const nlohmann::json& json, T& value) {
	return ReaderVisitor{json}.Read(value);
}

} // namespace r0