#include "String.hpp"

#include <iostream>

#include <gtest/gtest.h>

using namespace unicode;

/// Check that 2 texts are equal symbol by symbol
static bool areEqual(const String &str, const std::string &utf8)
{
	auto utext = unicode::detail::openUText(str);
	auto it = unicode::detail::getCharacterBreakIterator(&utext);
	for (
		auto start = it->first(), end = it->next(), characterIndex = 0;
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++characterIndex
	)
	{
		auto c = str[characterIndex];
		auto substr = utf8.substr(start, end - start);
		if (c != substr)
		{
			utext_close(&utext);
			return false;
		}
	}
	utext_close(&utext);
	return true;
}

TEST(ASCII, empty)
{
	String empty;
	EXPECT_TRUE(areEqual(empty, ""));
	EXPECT_EQ(empty.size(), 0);
	EXPECT_TRUE(empty.isEmpty());
	EXPECT_TRUE(empty.isASCII());
}

TEST(ASCII, repeated)
{
	String::SizeType times = 3;
	auto repeated = String::repeat('x', Times{times});
	EXPECT_TRUE(areEqual(repeated, std::string(3, 'x')));
	EXPECT_EQ(repeated.size(), times);
	EXPECT_FALSE(repeated.isEmpty());
	EXPECT_TRUE(repeated.isASCII());
}

TEST(UTF8, hello)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	String hello = str;
	EXPECT_EQ(hello.size(), 77);
	EXPECT_FALSE(hello.isEmpty());
	EXPECT_FALSE(hello.isASCII());
	EXPECT_TRUE(areEqual(hello, str));
}

TEST(UTF8, reverseIteration)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	String hello = str;

	auto utext = unicode::detail::openUText(str);
	auto it = unicode::detail::getCharacterBreakIterator(&utext);
	for (
		auto start = it->first(), end = it->next(), characterIndex = 0;
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++characterIndex
	)
	{
		auto c = hello[
			-String::RelativeCharacterIndex(hello.size()) + characterIndex
		];
		auto substr = str.substr(start, end - start);
		EXPECT_EQ(c, substr);
	}
	utext_close(&utext);
}