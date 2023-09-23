#pragma once

#include <variant>

namespace r0 {

// https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type
template <class T, class Tuple>
struct TupleIndexOf;

template <class T, class... Types>
struct TupleIndexOf<T, std::tuple<T, Types...>> {
	static const int value = 0;
};

template <class T, class U, class... Types>
struct TupleIndexOf<T, std::tuple<U, Types...>> {
	static const int value = 1 + TupleIndexOf<T, std::tuple<Types...>>::value;
};

template<typename... Ts>
struct ExpandedVariant {
private:
	template<typename T>
	static consteval size_t CalcTypeIndex() {
		return VariantType(T{}).index();
	}

public:

	using TupleType = std::tuple<Ts...>;
	using VariantType = std::variant<Ts...>;

	int which = 0;
	TupleType values;

	ExpandedVariant() = default;

	template<typename T>
	ExpandedVariant(const T& value) {
		constexpr size_t construtedIdx = CalcTypeIndex<T>();
		which = construtedIdx;
		std::get<construtedIdx>(values) = value;
	}

	template<typename T>
	constexpr int IndexOf() const {
		return TupleIndexOf<T, TupleType>::value;
	}

	template<typename R, typename F>
	auto Visit(F func) {
		using InvokerType = R(*)(F, TupleType&);
		
		static const InvokerType invokers[] = {
			[](F func, TupleType& t) {
				return func(std::get<Ts>(t));
			}...
		};
		return invokers[which](std::move(func), values);
	}

	template<typename R, typename F>
	auto Visit(F func) const {
		using InvokerType = R(*)(F, const TupleType&);
		
		static const InvokerType invokers[] = {
			[](F func, const TupleType& t) {
				return func(std::get<Ts>(t));
			}...
		};
		return invokers[which](std::move(func), values);
	}

	template<typename F>
	void VisitAll(F func) {
		using InvokerType = void(*)(F&, int, TupleType&);
		static const InvokerType invokers[] = {
			[](F& func, int which, TupleType& t) {
				return func(which, std::get<Ts>(t));
			}...
		};
		for (int i = 0; i < sizeof...(Ts); ++i) {
			invokers[i](func, i, values);
		}
	}

	template<typename F>
	void VisitAll(F func) const {
		using InvokerType = void(*)(F&, int, const TupleType&);
		static const InvokerType invokers[] = {
			[](F& func, int which, const TupleType& t) {
				return func(which, std::get<Ts>(t));
			}...
		};
		for (int i = 0; i < sizeof...(Ts); ++i) {
			invokers[i](func, i, values);
		}
	}

	VariantType ToVariant() const {
		return Visit<VariantType>([](const auto& value) {
			return VariantType(value);
		});
	}
};

} // namespace r0