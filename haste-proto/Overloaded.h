#pragma once

namespace r0 {

template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
Overloaded(Ts...)->Overloaded<Ts...>;

} // namespace r0