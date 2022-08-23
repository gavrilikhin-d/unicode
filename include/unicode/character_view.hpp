#pragma once

#include <string_view>

#include "unicode/comparable_interface.hpp"

namespace unicode
{

/// Single unicode character
class character_view 
	: public comparable_interface<character_view>, 
	  public std::string_view
{
public:
	explicit character_view(std::string_view bytes) : std::string_view(bytes) {}
};
	
} // namespace unicode
