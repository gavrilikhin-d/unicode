#pragma once

#include <vector>
#include <string_view>

#include "unicode/utility/sorted_vector.hpp"

namespace unicode
{

/// Contiguous block of characters with same number of bytes per character
struct block
{
	/// Size of block in characters
	size_t size = 0;
	/// Bytes of characters
	std::string_view bytes;
};

/// Unicode string layout
struct layout
{
	/// Character offsets of blocks
	utility::sorted_vector<size_t> offsets;
	/// Blocks of consecutive characters with same size
	std::vector<block> blocks;

	/// Get layout of string
	static layout of(std::string_view bytes) noexcept;
};
	
} // namespace unicode
