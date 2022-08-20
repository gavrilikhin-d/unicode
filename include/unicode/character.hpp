#pragma once

#include <string_view>

#include "unicode/comparable_interface.hpp"

namespace unicode
{

/// Single unicode character
class character : public comparable_interface<character>
{
public:
	bool operator==(const character& other) const;
	bool operator<=>(const character& other) const;
private:
	std::string_view bytes;
};
	
} // namespace unicode
