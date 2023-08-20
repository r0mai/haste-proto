#pragma once

#include <variant>

namespace r0 {

template<typename... Ts>
struct ExpandedVariant {
	using TupleType = std::tuple<Ts...>;
	using VariantType = std::variant<Ts...>;

	int which = 0;
	TupleType values;

	ExpandedVariant() = default;

	template<typename T>
	ExpandedVariant(const T& value) {
		constexpr size_t construtedIdx = VariantType(T{}).index();
		which = construtedIdx;
		std::get<construtedIdx>(values) = value;
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

	VariantType ToVariant() const {
		return Visit<VariantType>([](const auto& value) {
			return VariantType(value);
		});
	}
};

} // namespace r0