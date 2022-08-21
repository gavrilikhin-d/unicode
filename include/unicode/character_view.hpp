#pragma once

#include <string_view>

#include "unicode/comparable_interface.hpp"

namespace unicode
{

/// Single unicode character
class character_view : public comparable_interface<character>
{
public:
	explicit character(std::string_view bytes) : bytes(bytes) {}

	constexpr operator std::string_view() const noexcept { return bytes; }
private:
	/// Bytes of character
	std::string_view bytes;
};
	
} // namespace unicode
