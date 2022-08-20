#pragma once

#include <string_view>

namespace unicode::utf8
{

/// Compare two UTF-8 strings with default locale comparison rules
std::strong_ordering compare(
	std::string_view lhs, 
	std::string_view rhs
) noexcept;

} // namespace unicode::utf8
