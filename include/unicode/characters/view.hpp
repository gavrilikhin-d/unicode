#pragma once

#include <ranges>

#include "unicode/characters/iterator.hpp"
#include "unicode/layout.hpp"


namespace unicode::characters
{

/// View over unicode characters
class view : public std::ranges::view_interface<view>
{
public:
	iterator begin() const;
private:
	std::shared_ptr<layout> layout_;
};
	
} // namespace unicode::characters
