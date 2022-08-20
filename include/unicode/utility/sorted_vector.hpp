#pragma once

#include <vector>

namespace unicode::utility
{

template<
	typename Value, 
	typename Comparator = std::less<Value>, 
	typename Allocator = std::allocator<Value>
>
class sorted_vector : public std::vector<Value, Allocator>
{

};
	
} // namespace unicode::utility
