#include "unicode/layout.hpp"

#include <memory>
#include <cassert>

#include <unicode/utext.h>
#include <unicode/brkiter.h>

using namespace unicode;

/// Get character break iterator at the beginning of openned unicode text 
static std::unique_ptr<icu::BreakIterator> 
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
static auto openUText(std::string_view str) noexcept
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

/// Get layout of string
layout layout::of(std::string_view bytes) noexcept
{
	if (bytes.empty()) { return {}; }

	layout layout;

	auto utext = openUText(bytes);
	assert(utext);

	auto it = getCharacterBreakIterator(utext.get());

	size_t offset = 0;
	size_t previous_character_size = 0;
	for (
		auto start = it->first(), end = it->next();
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++offset
	)
	{
		size_t character_size = end - start;

		if (character_size != previous_character_size)
		{
			layout.offsets.push_back(offset);
			layout.blocks.push_back(
				block{
					.size = 1,
					.bytes = bytes.substr(start, character_size)
				}
			);
			continue;
		}

		layout.blocks.back().size += 1;
		layout.blocks.back().bytes = 
			std::string_view{
				layout.blocks.back().bytes.data(),
				layout.blocks.back().bytes.size() + character_size
			};
	}


	return layout;
}