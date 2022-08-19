#pragma once

#include <string>
#include <string_view>
#include <iosfwd>
#include <memory>
#include <cmath> // round
#include <cassert>
#include <optional>

#include "SortedVector.hpp"

#include <unicode/utext.h>
#include <unicode/brkiter.h>

namespace unicode
{

/// Helpers that deal with unicode
namespace detail
{

/// Get character break iterator at the beginning of openned unicode text 
std::unique_ptr<icu::BreakIterator> 
getCharacterBreakIterator(UText *utext) noexcept
{
	UErrorCode errorCode = U_ZERO_ERROR;
	std::unique_ptr<icu::BreakIterator> it {
		icu::BreakIterator::createCharacterInstance(
			icu::Locale::getDefault(), 
			errorCode
		)
	};
	if (U_FAILURE(errorCode))
	{
		return nullptr;
	}

	errorCode = U_ZERO_ERROR;
	it->setText(utext, errorCode);
	if (U_FAILURE(errorCode))
	{
		return nullptr;
	}
	return it;
}

/// Open utf-8 string as unicode text
UText openUText(std::string_view str) noexcept
{
	UErrorCode errorCode = U_ZERO_ERROR;
	UText utext = UTEXT_INITIALIZER;
	utext_openUTF8(&utext, str.data(), str.size(), &errorCode);
	if (U_FAILURE(errorCode))
	{
		return UTEXT_INITIALIZER;
	}
	return utext;
}

/// Calculate size of utf-8 string in characters
uint32_t calculateSizeInCharacters(std::string_view text) noexcept
{
	if (text.empty()) { return 0; }
	if (text.size() == 1) { return 1; }

	auto utext = openUText(text);

	auto it = getCharacterBreakIterator(&utext);

	uint32_t size = 0;
	for (
		int32_t end = it->next(); 
		end != icu::BreakIterator::DONE; 
		end = it->next()
	)
	{
		++size;
	}

	utext_close(&utext);

	return size;
}

} // namespace detail

/// Structure to differentiate between char and count in repeat function
struct Times { uint32_t count; };

/**
 *	 Unicode string - collection of user-percieved characters,
 * 	 which may be represented with multiple Unicode code points.
 * 
 *   TODO: requirements for the container and readonly version
 * 
 *   TODO: determine underlying UTF-8, UTF-16 or UTF-32 encoding on the fly
 */
template<typename Storage = std::string>
class BasicString
{
public:
	/// Type of string size in characters.
	///
	/// @details 
	///  Is less than 2^64 because of: 
	///	 	* @c BreakIterator limitations
	///     * @c Block memory efficiency
	///     * Why would you need contignuous strings that big?
	using SizeType = uint32_t;

	/// Type for index of Character
	using CharacterIndex = uint32_t;
	/// Relative character index
	using RelativeCharacterIndex = int32_t;

	/// Size in bytes
	using ByteSize = uint8_t;

	/// Type for index of a byte in character
	using ByteIndex = typename Storage::size_type;

	/// Type for difference between byte indexes
	using ByteDifference = typename Storage::difference_type;

	/// Character inside of string
	using Character = std::string_view;

	using value_type = Character;
	using size_type = uint32_t;
	using difference_type = int32_t;
	using reference = value_type;
	using const_reference = const value_type;
	using pointer = value_type *;
	using const_pointer = const value_type *;

	/// Random access iterator over characters
	class Iterator
	{
	public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = Character;
		using difference_type = RelativeCharacterIndex;
		using pointer = Character *;
		using reference = Character;

		Iterator(BasicString &str, CharacterIndex index = 0) noexcept
			: stringPointer(&str), index(index)
		{}

		reference operator*() const noexcept
		{
			return (*stringPointer)[index];
		}

		pointer operator->() const noexcept
		{
			return &(stringPointer)[index];
		}

		Iterator &operator++() noexcept
		{
			++index;
			return *this;
		}

		Iterator operator++(int) noexcept
		{
			auto copy = *this;
			++index;
			return copy;
		}

		Iterator &operator--() noexcept
		{
			--index;
			return *this;
		}

		Iterator operator--(int) noexcept
		{
			auto copy = *this;
			--index;
			return copy;
		}

		Iterator &operator+=(difference_type offset) noexcept
		{
			index += offset;
			return *this;
		}

		Iterator operator+(difference_type offset) const noexcept
		{
			return Iterator(*this) += offset;
		}

		Iterator &operator-=(difference_type offset) noexcept
		{
			index -= offset;
			return *this;
		}

		Iterator operator-(difference_type offset) const noexcept
		{
			return Iterator(*this) -= offset;
		}

		difference_type operator-(const Iterator &other) const noexcept
		{
			assert(
				stringPointer == other.stringPointer && 
				"Comparing iterators from different strings"
			);
			return index - other.index;
		}

		reference operator[](difference_type offset) const noexcept
		{
			return (*stringPointer)[index + offset];
		}

		bool operator==(const Iterator &other) const noexcept
		{
			assert(
				stringPointer == other.stringPointer && 
				"Comparing iterators from different strings"
			);
			return index == other.index;
		}

		auto operator<=>(const Iterator &other) const noexcept
		{
			assert(
				stringPointer == other.stringPointer && 
				"Comparing iterators from different strings"
			);
			return index <=> other.index;
		}
		
	private:
		/// TODO: use BreakIterator if possible

		/// String, this iterator belongs to
		BasicString *stringPointer = nullptr;
		/// Index of character, this iterator points to
		CharacterIndex index = 0;
	};

	using iterator = Iterator;
	using const_iterator = Iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


	/// Get maximum number of characters in string
	static constexpr SizeType maxSize() noexcept 
	{
		return SizeType(std::numeric_limits<int32_t>::max());
	}

	/// Create unicode string from string with only ASCII characters.
	/// @warning No validation is performed.
	static BasicString fromASCII(Storage ascii) noexcept
	{
		BasicString str;
		str._layout.size = ascii.size();
		str._layout.averageCharacterSize = 1;
		str.bytes = std::move(ascii);
		return str;
	}

	/// Get ASCII character
	static BasicString repeat(char c, Times times) noexcept
	{
		return BasicString::fromASCII(Storage(times.count, c));
	}

	/// Create empty string
	BasicString() noexcept : _layout(Layout{.averageCharacterSize = 1, .size = 0}) {}
	/// Create string from ASCII character
	BasicString(char c) noexcept 
		: BasicString(BasicString::fromASCII(Storage(1, c))) {}
	/// Create string from ASCII characters
	BasicString(std::initializer_list<char> chars) noexcept 
		: BasicString(BasicString::fromASCII(Storage(chars))) {}
	/// Create string from UTF-8 encoded characters
	BasicString(const char *str) : bytes(str) {}
	/// Create string from UTF-8 encoded characters
	BasicString(std::string_view view) : bytes(view) {}
	/// Create unicode string from utf-8 encoded string
	BasicString(Storage str) noexcept : bytes(std::move(str)) {}

	/// Don't create string from nullptr
	BasicString(std::nullptr_t) = delete;


	iterator begin() noexcept { return iterator(*this); }
	iterator end() noexcept { return iterator(*this, size()); }
	const_iterator begin() const noexcept 
	{ 
		return const_iterator(*this); 
	}
	const_iterator end() const noexcept 
	{ 
		return const_iterator(*this, size()); 
	}

	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }

	reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	const_reverse_iterator rbegin() const noexcept 
	{ 
		return const_reverse_iterator(end()); 
	}
	const_reverse_iterator rend() const noexcept 
	{ 
		return const_reverse_iterator(begin()); 
	}

	/// Does string contain only ASCII characters?
	[[nodiscard("Possibly expensive O(n) operation")]]
	bool isASCII() const noexcept 
	{ 
		return layout().isASCII();
	}

	/// Is string empty?
	bool isEmpty() const noexcept { return bytes.empty(); }

	/// Is string empty?
	[[nodiscard, deprecated("use isEmpty() instead")]]
	bool empty() const noexcept { return isEmpty(); }

	/// Clear string
	void clear() noexcept
	{ 
		_layout.clear();
		bytes.clear();
	}

	/// Get size of string in characters
	[[nodiscard("Possibly expensive O(n) operation")]]
	SizeType size() const noexcept 
	{ 
		/// TODO: SizeRequest helper, that will lazily calculate size
		return layout().size;
	}
	
	/// Get length of string in characters
	[[nodiscard("Possibly expensive O(n) operation")]]
	SizeType length() const noexcept { return size(); }


	operator const Storage &() const noexcept { return bytes; }
	operator std::string_view() const noexcept { return bytes; }

	/// Get character by index.
	template<std::unsigned_integral PositiveIndex>
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character operator[](PositiveIndex index) const noexcept
	{
		assert(index < size() && "out of bounds");
		auto [byteIndex, size] = layout().getCharacterByteIndexAndSize(index);
		return Character{bytes.data() + byteIndex, size};
	}

	// Get character by index. Negative index is counted from the end.
	template<std::signed_integral NegativeIndex>
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character operator[](NegativeIndex index) const noexcept
	{
		CharacterIndex characterIndex = index;
		if (index < 0)
		{
			characterIndex = absoluteIndex(index);
		}
		return (*this)[characterIndex];
	}

	/// Get character by index.
	template<std::unsigned_integral PositiveIndex>
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character at(PositiveIndex index) const noexcept
	{
		if (index >= size())
		{
			throw std::out_of_range(
				"Index " + std::to_string(index) + " is out of range"
			);
		}
		return (*this)[index];
	}

	// Get character by index. Negative index is counted from the end.
	template<std::signed_integral NegativeIndex>
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character at(NegativeIndex index) const noexcept
	{
		CharacterIndex characterIndex = index;
		if (index < 0)
		{
			if (index < -size())
			{
				throw std::out_of_range(
					"Index " + std::to_string(index) + " is out of range"
				);
			}
			characterIndex = absoluteIndex(index);
		}
		return this->at(characterIndex);
	}

	/// Convert relative index from end to absolute
	[[nodiscard("Possibly expensive O(n) operation")]]
	CharacterIndex absoluteIndex(RelativeCharacterIndex relative) const noexcept
	{
		return absoluteIndex(relative, size());
	}

	/// Convert relative to some position index to absolute
	CharacterIndex absoluteIndex(
		RelativeCharacterIndex relative, 
		CharacterIndex start
	) const noexcept
	{
		assert(
			relative >= 0 || start >= CharacterIndex(-relative) && 
			"index below zero"
		);
		assert(relative <= 0 || start + relative <= size() && "past end index");
		return start + relative;
	}

	/// Get first character
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character front() const noexcept { return (*this)[0]; }

	/// Get last character
	[[nodiscard("Possibly expensive O(n) operation")]]
	Character back() const noexcept { return (*this)[-1]; }

	/// Get C string
	const char *c_str() const noexcept { return bytes.c_str(); }

	/// Get data string
	char *data() noexcept 
	{ 
		// Clear layout, as it may become invalid
		_layout.clear();
		return bytes.data(); 
	}

	/// Get data string
	const char *data() const noexcept { return bytes.data(); }


	/// Get capacity of string in bytes
	ByteSize capacity() const noexcept { return bytes.capacity(); }

	/// Get size of string in bytes
	ByteSize size_in_bytes() const noexcept { return bytes.size(); }

	/// Shrink string to fit its size
	void shrink_to_fit() { bytes.shrink_to_fit(); }

	/// Reserve space for string
	void reserve(ByteSize size) { bytes.reserve(size); }

	/// Resize string
	void resize(ByteSize size) 
	{
		if (size < size_in_bytes())
		{
			// Clear layout, as it may become invalid
			_layout.clear();
		}
		bytes.resize(size); 
	}


	/// Append utf-8 string to the end of this string
	BasicString &operator+=(std::string_view str)
	{
		if (str.empty()) { return *this; }

		// Appended string is bigger
		if (bytes.size() <= str.size())
		{
			// just clear layout information to reevaluate later
			_layout.clear();
		}
		bytes += str;
		
		if (not isEmpty() && layoutIsEvaluated())
		{
			auto lastCharacter = back();
			_layout.dropLayoutStartingFrom(_layout.size - 1);
			str = std::string_view{
				lastCharacter.begin(), 
				lastCharacter.size() + str.size()
			};
			_layout.updateAfterAppend(str);
		}
		
		return *this;
	}

	/// Copy and concatenate two strings
	BasicString operator+(std::string_view str) const noexcept
	{
		BasicString result = *this;
		result += str;
		return result;
	} 

	friend std::ostream &operator<<(std::ostream &os, const BasicString &str)
	{
		return os << str.bytes;
	}

	friend std::istream &operator>>(std::istream &is, BasicString &str)
	{
		return is >> str.bytes;
	}
private:
	/// UTF-8 encoded content of string
	Storage bytes;

	/// Metadata about string layout
	struct Layout
	{
		enum { NOT_EVALUATED = 0 };
		/// Average character size in bytes
		ByteSize averageCharacterSize : 8 = NOT_EVALUATED;

		/// Number of characters in string
		SizeType size = -1;

		/// Sequence of characters with size that differs from average.
		///
		/// @note 
		///	 This must be as much memory efficient as possible,
		///	 as it will be created every ~8 characters (average word size)
		///  in languages like Russian
		struct [[gnu::packed]] Block
		{
			/// Index of the first character in block
			CharacterIndex firstCharacter = 0;
			/// Difference from first byte estimation
			ByteDifference firstByteDifference : 32 = 0;
			/// Count of characters in block. 
			/// 0 means 1 character, as block may not be empty
			SizeType charactersCountMinusOne : 4 = 0;
			/// Size of character in bytes.
			/// 0 means 1 byte, as character occupies at least 1 byte
			ByteSize characterSizeMinusOne : 4 = 0;

			/// Get size of character in bytes
			constexpr ByteSize characterSize() const noexcept
			{
				return characterSizeMinusOne + 1;
			}

			/// Set character size in bytes
			constexpr void setCharacterSize(ByteSize size) noexcept
			{
				assert(size > 0 && size <= 16 && "invalid character size");
				characterSizeMinusOne = size - 1;
			}

			/// Get count of characters in block
			constexpr SizeType charactersCount() const noexcept
			{
				return charactersCountMinusOne + 1;
			}

			/// Set character count in block
			constexpr void setCharactersCount(SizeType count) noexcept
			{
				assert(count > 0 && count <= 16 && "invalid character count");
				charactersCountMinusOne = count - 1;
			}

			/// Increment characters count
			constexpr void incrementCharactersCount() noexcept
			{
				assert(not isFull() && "block is full");
				++charactersCountMinusOne;
			}

			/// Is block full?
			constexpr bool isFull() const noexcept
			{
				return charactersCount() == 16;
			}

			/// Does this block contain character with given index?
			constexpr bool containsCharacterIndex(
				CharacterIndex index
			) const noexcept
			{
				return 
					index >= firstCharacter && 
					index < firstCharacter + CharacterIndex(charactersCount());
			}

			/// Sort blocks by first character index
			constexpr auto operator<=>(const Block &other) const noexcept
			{
				return firstCharacter <=> other.firstCharacter;
			}

			/// Sort blocks by first character index
			constexpr auto operator<=>(CharacterIndex index) const noexcept
			{
				return firstCharacter <=> index;
			}
		};
		static_assert(sizeof(Block) == 9, "Block is not packed");

		/// Set of character sequences where size differs from average.
		/// Sorted by first character index
		SortedVector<Block, std::less<>> blocks;

		/// Is layout evaluated?
		constexpr bool isEvaluated() const noexcept
		{
			return averageCharacterSize != NOT_EVALUATED;
		}

		/// Is this layout for ASCII string?
		constexpr bool isASCII() const noexcept
		{
			assert(isEvaluated() && "Layout is not evaluated");
			return averageCharacterSize == 1 && blocks.isEmpty();
		}
		
		/// Evaluate layout for utf-8 string
		static Layout getFor(std::string_view str) noexcept
		{
			assert(str.size() <= BasicString::maxSize() && "String is too big");

			auto size = detail::calculateSizeInCharacters(str);

			// Don't need to do anything else for ASCII.
			if (size == SizeType(str.size())) 
			{ 
				return Layout{
					.averageCharacterSize = 1, 
					.size = size
				}; 
			}

			Layout layout{
				.averageCharacterSize = 
					ByteSize(std::round(double(str.size()) / size)),
				.size = 0
			};

			layout.updateAfterAppend(str);

			return layout;
		}


		/// Get iterator to block that contains character or just before it
		auto getNearestLeftBlock(CharacterIndex index) const noexcept
		{
			auto block = blocks.upper_bound(index);
			return std::make_reverse_iterator(block);
		}

		/// Get iterator to block that contains character or just before it
		auto getNearestLeftBlock(CharacterIndex index) noexcept
		{
			auto block = blocks.upper_bound(index);
			return std::make_reverse_iterator(block);
		}

		/// Estimate first byte index for character
		ByteIndex estimateFirstByteIndex(CharacterIndex index) const noexcept
		{
			assert(isEvaluated() && "layout is not evaluated");
			return index * averageCharacterSize;
		}

		/// Differance from average character size in block
		ByteDifference characterSizeDifference(
			const Block &block
		) const noexcept
		{
			return 
				ByteDifference(block.characterSize()) - 
				ByteDifference(averageCharacterSize);
		}

		/// Difference in bytes that this whole block adds
		ByteDifference byteDifferenceFromBlock(
			const Block &block
		) const noexcept
		{
			return block.charactersCount() * characterSizeDifference(block);
		}

		/// Difference in bytes after block
		ByteDifference byteDifferenceAfterBlock(
			const Block &block
		) const noexcept
		{
			return block.firstByteDifference + byteDifferenceFromBlock(block);
		}

		/// Information about character
		struct ByteIndexAndSize
		{
			/// Index of first byte
			ByteIndex index;
			/// Size in bytes
			ByteSize size;
		};

		/// Get byte index and size for character
		ByteIndexAndSize getCharacterByteIndexAndSize(
			CharacterIndex characterIndex
		) const noexcept
		{
			assert(isEvaluated() && "layout is not evaluated");

			ByteIndexAndSize res{
				.index = estimateFirstByteIndex(characterIndex),
			 	.size = averageCharacterSize
			};

			if (blocks.isEmpty()) { return res; }

			auto end = getNearestLeftBlock(characterIndex);
			if (
				end != blocks.rend()
			)
			{
				res.index += end->firstByteDifference;
				if (end->containsCharacterIndex(characterIndex))
				{
					res.size = end->characterSize();
					res.index += 
						(characterIndex - end->firstCharacter) * characterSizeDifference(*end);
				}
			}

			return res;
		}

		/// Clear layout information
		void clear() noexcept
		{
			averageCharacterSize = NOT_EVALUATED;
			size = 0;
			blocks.clear();
		}

		/// Drop layout information starting from character and to the end
		void dropLayoutStartingFrom(CharacterIndex index)
		{
			assert(isEvaluated() && "layout is not evaluated");

			if (index == 0) { *this = Layout::getFor(""); return; }
			if (index == size) { return; }

			size = index;

			if (blocks.isEmpty()) { return; }

			auto end = getNearestLeftBlock(index);
			if (end != blocks.rend() && end->containsCharacterIndex(index))
			{
				if (end->firstCharacter != index)
				{
					end->setCharactersCount(index - end->firstCharacter);
				}
				else
				{
					++end;
				}
			}
			blocks.erase(end.base(), blocks.end());
		}

		/// Update layout after appending string.
		/// @warning include last symbol from string to be appended as well,
		/// inorder to correctly merge it
		void updateAfterAppend(std::string_view str) noexcept
		{
			assert(isEvaluated() && "layout is not evaluated");

			auto utext = detail::openUText(str);
			auto it = detail::getCharacterBreakIterator(&utext);

			ByteSize previousCharacterSize = averageCharacterSize;
			std::optional<Block> currentBlock;
			CharacterIndex characterIndex = size;
			ByteDifference byteDifference = 0;
			if (not blocks.isEmpty())
			{
				byteDifference = byteDifferenceAfterBlock(blocks.back());
			}
			for (
				auto start = it->first(), end = it->next();
				end != icu::BreakIterator::DONE; 
				start = end, end = it->next(), ++characterIndex, ++size)
			{
				ByteSize characterByteSize = end - start;

				// This character may be part of block
				if (characterByteSize == previousCharacterSize)
				{
					// Character size is the same as average
					if (previousCharacterSize == averageCharacterSize)
					{
						continue;
					}

					assert(
						currentBlock.has_value() && 
						"current block is not set"
					);

					// Add character to block, if it's not full
					if (not currentBlock->isFull())
					{
						currentBlock->incrementCharactersCount();
						continue;
					}

					// block is full -> falthrough to creating new block
				}

				// Add previous block to set
				if (currentBlock)
				{
					byteDifference += byteDifferenceFromBlock(*currentBlock);
					blocks.push_back(std::move(*currentBlock));
				}

				// New block needed
				currentBlock = Block{
					.firstCharacter = characterIndex,
					.firstByteDifference = byteDifference
				};
				currentBlock->setCharacterSize(characterByteSize);

				previousCharacterSize = characterByteSize;
			}

			// Add last block to set
			if (currentBlock) 
			{ 
				blocks.push_back(std::move(*currentBlock)); 
			}

			utext_close(&utext);
		}
	};

	/// Metadata about string layout. 
	/// @warning don't use it directly, as it may not be evaluated. 
	/// Use getter layout() instead
	mutable Layout _layout;

	/// Check that layout is evaluated, without evaluatiation
	bool layoutIsEvaluated() const noexcept
	{
		return _layout.isEvaluated();
	}

	/// Get layout of string
	[[nodiscard("Possibly expensive O(n) operation")]]
	Layout & layout() const noexcept
	{
		if (not _layout.isEvaluated())
		{
			_layout = Layout::getFor(bytes);
		}
		return _layout;
	}
};

/// Alias for default string
using String = BasicString<>;

} // namespace unicode
