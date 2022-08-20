#include "unicode/utf8/compare.hpp"

#include <cassert>
#include <memory> // std::unique_ptr

#include <unicode/coll.h>

/// Compare two UTF-8 strings with default locale comparison rules
std::strong_ordering unicode::utf8::compare(
	std::string_view lhs, 
	std::string_view rhs
) noexcept
{
	UErrorCode errorCode = U_ZERO_ERROR;
	std::unique_ptr<icu::Collator> coll{
		icu::Collator::createInstance(
			icu::Locale::getDefault(), 
			errorCode
		)
	};
	if (U_FAILURE(errorCode)) 
	{
		assert(false && "coudn't create collator"); 
		/// Fallback to byte comparison
		return lhs.compare(rhs) <=> 0; 
	}

	errorCode = U_ZERO_ERROR;
	auto res = coll->compareUTF8(lhs, rhs, errorCode);
	if (U_FAILURE(errorCode))
	{
		assert(false && "collator error");
		/// Fallback to byte comparison
		return lhs.compare(rhs) <=> 0; 
	}

	return res <=> 0;
}
