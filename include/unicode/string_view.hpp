#pragma once

#include <string>
#include <string_view>
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
	class iterator
	{
	private:
		/// View over string
		const string_view *view = nullptr; 
	public:
		using value_type = character_view;
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using reference = character_view;
		using iterator_category = std::random_access_iterator_tag;

		/// Index of character
		size_t index = 0;

		/// Create iterator over unicode characters for string view
		iterator(const string_view &view, size_t index = 0) noexcept
			: view(&view), index(index)
		{}

		/// Random access iterator methods
		iterator &operator+=(difference_type offset) noexcept
		{
			index += offset;
			return *this;
		}
		iterator &operator-=(difference_type offset) noexcept
		{
			index -= offset;
			return *this;
		}
		iterator operator+(difference_type offset) const noexcept
		{
			return iterator(*view, index + offset);
		}
		iterator operator-(difference_type offset) const noexcept
		{
			return iterator(*view, index - offset);
		}
		difference_type operator-(const iterator &other) const noexcept
		{
			assert(
				view == other.view && 
				"comparing iterators from different views"
			);
			return index - other.index;
		}
		iterator &operator++() noexcept
		{
			++index;
			return *this;
		}
		iterator operator++(int) noexcept
		{
			return iterator(*view, index++);
		}
		iterator &operator--() noexcept
		{
			--index;
			return *this;
		}
		iterator operator--(int) noexcept
		{
			return iterator(*view, index--);
		}
		value_type operator*() const noexcept
		{
			return view->operator[](index);
		}
		value_type operator[](difference_type offset) const noexcept
		{
			return view->operator[](index + offset);
		}
		bool operator==(const iterator &other) const noexcept
		{
			return index == other.index;
		}
		auto operator<=>(const iterator &other) const noexcept
		{
			return index <=> other.index;
		}
	};

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
		: bytes(bytes), layout(layout::of(bytes)) {}
	/// View over string
	string_view(const std::string &bytes)
		: string_view(std::string_view(bytes)) {}

	/// Get iterator for first character
	iterator begin() const noexcept
	{
		return iterator(*this);
	}
	/// Get iterator for one past last character
	iterator end() const noexcept
	{
		return iterator(*this, size());
	}
	/// Get iterator for first character
	const_iterator cbegin() const noexcept
	{
		return begin();
	}
	/// Get iterator for one past last character
	const_iterator cend() const noexcept
	{
		return end();
	}
	/// Get reverse iterator for last character
	reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(end());
	}
	/// Get reverse iterator for one before first character
	reverse_iterator rend() const noexcept
	{
		return reverse_iterator(begin());
	}
	/// Get reverse iterator for last character
	const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(cend());
	}
	/// Get reverse iterator for one before first character
	const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(cbegin());
	}

	/// Get first character
	character_view front() const noexcept
	{
		return operator[](0);
	}

	/// Get last character
	character_view back() const noexcept
	{
		return operator[](size() - 1);
	}

	/// Get size of string in characters
	constexpr size_t size() const noexcept 
	{
		if (layout.offsets.empty()) { return 0; }

		auto &last_block = layout.blocks.back();
		return 
			layout.offsets.back() + 
				(bytes.size() - last_block.byte_offset) / 
				last_block.character_size; 
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

	/// Get character by absolute index
	character_view operator[](size_type index) const noexcept
	{
		assert(index < size() && "out of range");

		auto block_index = layout.block_index_for_character(index);

		auto offset = layout.offsets[block_index];
		auto &block = layout.blocks[block_index];

		return character_view(
			bytes.substr(
				block.byte_offset + (index - offset) * block.character_size, block.character_size
			)
		);
	}

	/// Get character by index. Negative indexes are relative to end of string
	template<std::signed_integral index_t>
	character_view operator[](index_t index) const noexcept
	{
		if (index < 0) { index += size(); }
		assert(0 <= index && size_type(index) < size() && "out of range");

		return operator[](static_cast<size_type>(index));
	}

private:
	/// Bytes of string
	std::string_view bytes;
	/// Layout of string
	layout layout;
};
	
} // namespace unicode
