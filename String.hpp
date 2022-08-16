#pragma once

#include <string>
#include <string_view>

namespace unicode
{

/**
 *	 Unicode string - collection of user-percieved characters,
 * 	 which may be represented with multiple Unicode code points.
 * 
 *   TODO: add allocators support
 * 
 *   TODO: determine underlying UTF-8, UTF-16 or UTF-32 encoding on the fly
 */
class String
{
public:
	/// Type of string size in characters.
	///
	/// @details 
	///  Is less than @c size_t because of: 
	///	 	* @c BreakIterator limitations
	///     * @c Block memory efficiency
	///     * Why would you need more than 2 GiB of contignuous text data?
	using SizeType = int32_t;

	/// Get maximum number of characters in string
	static constexpr SizeType maxSize() noexcept 
	{
		return std::numeric_limits<SizeType>::max();
	}

	/// Structure to differentiate between char and count in repeat function
	struct Times { SizeType count; };

	/// Get ASCII character
	static String repeat(char c, Times times) noexcept
	{
		String str(std::string(times.count, c));
		/// TODO: optimization, as we know this is ASCII
		return str;
	}

	/// Create empty string
	String() noexcept = default;
	/// Create string from ASCII character
	String(char c) noexcept : bytes({c}) 
	{
		/// TODO: optimization, as we know this is ASCII
	}
	/// Create string from ASCII characters
	String(std::initializer_list<char> chars) noexcept 
		: bytes(std::move(chars)) 
	{
		/// TODO: optimization, as we know this is ASCII
	}
	/// Create unicode string from utf-8 encoded string
	String(std::string str) noexcept : bytes(std::move(str)) {}

	/// Assign other utf-8 encoded string
	String & operator=(std::string str) noexcept
	{
		bytes = std::move(str);
		return *this;
	}

	/// Is string empty?
	bool isEmpty() const noexcept { return bytes.empty(); }


	/// Get size of string in characters
	SizeType size() const noexcept 
	{ 
		/// TODO: implement real size
		/// TODO: SizeRequest helper, that will lazily calculate size
		return bytes.size();
	}

	operator const std::string &() const noexcept { return bytes; }
	operator std::string_view() const noexcept { return bytes; }
private:
	/// UTF-8 encoded content of string
	std::string bytes;
};

} // namespace unicode
