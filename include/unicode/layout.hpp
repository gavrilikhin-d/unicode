#pragma once

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
	/// Bytes of characters
	std::string_view bytes;

	/// Size of block in characters
	constexpr size_t size() const noexcept
	{
		assert(character_size > 0 && bytes.size() % character_size == 0);
		return bytes.size() / character_size;
	}
	
	/// Extend block by one character
	constexpr void extend() noexcept
	{
		bytes = std::string_view{
			bytes.data(),
			bytes.size() + character_size
		};
	}

	/// Get character with index, relative to start of block
	constexpr std::string_view operator[](size_t index) const noexcept
	{
		assert(index < size() && "out of range");
		return std::string_view{
			bytes.data() + index * character_size,
			character_size
		};
	}
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
