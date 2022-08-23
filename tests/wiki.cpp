#include "unicode/string_view.hpp"

#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

#include "../sources/icu.hpp"

using namespace unicode;

/// Read whole file content
static std::string readFile(const std::string &path)
{
	std::ifstream file(path);
	assert(file && "Can't open file");
	std::string content((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));
	return content;
}

#define TEST_LANGUAGE(language) \
	TEST(wiki, language) \
	{ \
		auto content = readFile("../../data/" #language "/wiki.txt"); \
		string_view view = content; \
		EXPECT_FALSE(view.empty()); \
		EXPECT_EQ(view, content); \
		auto utext = openUText(content); \
		auto it = getCharacterBreakIterator(utext.get()); \
		for ( \
			auto start = it->first(), end = it->next(), index = 0; \
			end != icu::BreakIterator::DONE; \
			start = end, end = it->next(), ++index \
		) \
		{ \
			auto c = view[index]; \
			auto substr = content.substr(start, end - start); \
			EXPECT_EQ(c, character_view(substr)); \
		} \
	}

TEST_LANGUAGE(english);
TEST_LANGUAGE(russian);
TEST_LANGUAGE(chinese);
TEST_LANGUAGE(french);
TEST_LANGUAGE(german);
TEST_LANGUAGE(japanese);
TEST_LANGUAGE(korean);