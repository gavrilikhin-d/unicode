#include "unicode/utf8/compare.hpp"
#include "unicode/string_view.hpp"

#include <string>

#include <gtest/gtest.h>

#include "../sources/icu.hpp"

using namespace unicode;

TEST(UTF8, compare)
{
	{
		std::string_view text = "abcd";
		auto result = utf8::compare(text, text);
		EXPECT_EQ(result, std::strong_ordering::equal);
	}

	{
		// Denormalized unicode 'a' with acute"
		std::string_view denormalized = "a\u0301";
		// Normalized unicode 'a' with acute
		std::string_view normalized = "\u00e1";
		EXPECT_NE(denormalized, normalized);

		auto result = utf8::compare(denormalized, normalized);
		EXPECT_EQ(result, std::strong_ordering::equal);
	}

	{
		auto result = utf8::compare("1", "2");
		EXPECT_EQ(result, std::strong_ordering::less);
	}

	{
		auto result = utf8::compare("в", "б");
		EXPECT_EQ(result, std::strong_ordering::greater);
	}
}

TEST(string_view, compare)
{
	{
		unicode::string_view text = "abcd";
		EXPECT_EQ(text, text);
	}

	{
		// Denormalized unicode 'a' with acute"
		unicode::string_view denormalized = "a\u0301";
		// Normalized unicode 'a' with acute
		unicode::string_view normalized = "\u00e1";
		EXPECT_EQ(denormalized, normalized);
	}

	{
		EXPECT_LT(unicode::string_view("1"), "2");
	}

	{
		EXPECT_GT(unicode::string_view("в"), "б");
	}
}

TEST(string_view, empty)
{
	unicode::string_view view = "";

	EXPECT_EQ(view.size(), 0);
	EXPECT_TRUE(view.empty());
	EXPECT_EQ(view, "");
}

TEST(string_view, size)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	unicode::string_view view = str;

	EXPECT_EQ(view.size(), 77);
	EXPECT_FALSE(view.empty());
	EXPECT_EQ(view, str);
}

TEST(string_view, indexing)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	unicode::string_view view = str;

	auto utext = openUText(str);
	auto it = getCharacterBreakIterator(utext.get());
	for (
		auto start = it->first(), end = it->next(), index = 0;
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++index
	)
	{
		auto c = view[index];
		auto substr = str.substr(start, end - start);
		EXPECT_EQ(c, character_view(substr));
	}
}


TEST(string_view, negative_indexing)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	unicode::string_view view = str;

	auto utext = openUText(str);
	auto it = getCharacterBreakIterator(utext.get());
	for (
		auto start = it->first(), end = it->next(), index = 0;
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++index
	)
	{
		auto c = view[-(int(view.size()) - index)];
		auto substr = str.substr(start, end - start);
		EXPECT_EQ(c, character_view(substr));
	}
}

TEST(string_view, iteration)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	unicode::string_view view = str;

	size_t index = 0;
	for (auto c : view)
	{
		EXPECT_EQ(c, view[index]);
		++index;
	}
}

TEST(string_view, reverse_iteration)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	unicode::string_view view = str;

	int index = -1;
	for (auto it = view.rbegin(); it != view.rend(); ++it)
	{
		EXPECT_EQ(*it, view[index]);
		--index;
	}
}