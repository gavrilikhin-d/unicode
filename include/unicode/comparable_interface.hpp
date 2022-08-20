#pragma once

#include "unicode/utf8/compare.hpp"

namespace unicode
{

/// CRTP base class for unicode comparable types
template<typename CRTP>
struct comparable_interface
{
	bool operator==(const CRTP& other) const noexcept
	{
		return (*this <=> other) == 0;
	}

	auto operator<=>(const CRTP& other) const noexcept
	{
		return utf8::compare(
			static_cast<const CRTP &>(*this), other
		);
	}
};

} // namespace unicode
