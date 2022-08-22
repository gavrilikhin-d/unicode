#pragma once

#include <vector>

#include "unicode/layout.hpp"
#include "unicode/comparable_interface.hpp"
#include "unicode/character_view.hpp"

namespace unicode
{

/// View over unicode characters
class string_view : public comparable_interface<string_view>
{
public:
	using value_type = character_view;
	using size_type = std::string_view::size_type;
	using difference_type = std::string_view::difference_type;

	/// Iterator over unicode characters
	class iterator;
	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using pointer = iterator;
	using const_pointer = const_iterator;
	using reference = character_view;
	using const_reference = character_view;

	/// View over empty string
	string_view() = default;
	/// View over string
	string_view(const char *bytes) 
		: string_view(std::string_view(bytes)) {}
	/// View over string
	string_view(std::string_view bytes) 
		: bytes(bytes), layout(layout::of(bytes)) {};

	/// Get size of string in characters
	constexpr size_t size() const noexcept 
	{
		if (layout.offsets.empty()) { return 0; }
		return layout.offsets.back() + layout.blocks.back().size; 
	}

	/// Is string empty?
	[[nodiscard]]
	bool empty() const noexcept { return size() == 0; }

	/// Get underlying bytes
	constexpr operator std::string_view() const noexcept { return bytes; }

	/// Update layout after change in string
	void update() { layout = layout::of(bytes); }

	/// Swap 2 views
	void swap(string_view other)
	{
		std::swap(bytes, other.bytes);
		std::swap(layout, other.layout);
	}

private:
	/// Bytes of string
	std::string_view bytes;
	/// Layout of string
	layout layout;
};
	
} // namespace unicode
