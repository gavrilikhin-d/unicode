#include "unicode/layout.hpp"

#include <cassert>

#include "icu.hpp"

using namespace unicode;

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