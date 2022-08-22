#pragma once

#include <memory>

#include <unicode/utext.h>
#include <unicode/brkiter.h>

/// Get character break iterator at the beginning of openned unicode text 
inline std::unique_ptr<icu::BreakIterator> 
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
inline auto openUText(std::string_view str) noexcept
{
	struct UTextCloser
	{
		void operator()(UText *utext) const noexcept
		{
			utext_close(utext);
		}
	};

	UErrorCode errorCode = U_ZERO_ERROR;
	std::unique_ptr<UText, UTextCloser> utext{
		utext_openUTF8(nullptr, str.data(), str.size(), &errorCode)
	};
	if (U_FAILURE(errorCode))
	{
		utext = nullptr;
	}
	return utext;
}