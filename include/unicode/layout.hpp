#pragma once

#include <cassert>
#include <vector>
#include <string_view>

#include "unicode/utility/sorted_vector.hpp"

namespace unicode
{

/// Contiguous block of characters with same number of bytes per character
struct block
{
	/// Size of characters inside of block, in bytes
	size_t character_size = 0;
	/// Offset of the first byte of block
	size_t byte_offset = 0;
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

	/// Get index of block for specified character
	size_t block_index_for_character(size_t character_index) const noexcept
	{
		auto next = offsets.upper_bound(character_index);
		assert(next != offsets.begin() && "block not found");
		return std::distance(offsets.begin(), next) - 1;
	}
};
	
} // namespace unicode
