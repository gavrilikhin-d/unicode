#include "String.hpp"

#include <iostream>

#include <gtest/gtest.h>

using namespace unicode;

TEST(ASCII, empty)
{
	String empty;
	// EXPECT_EQ(empty, "");
	EXPECT_EQ(empty.size(), 0);
	EXPECT_TRUE(empty.isEmpty());
	EXPECT_TRUE(empty.isASCII());
}

TEST(ASCII, repeated)
{
	String::SizeType times = 3;
	auto repeated = String::repeat('x', Times{times});
	// EXPECT_EQ(repeated, "xxx");
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
	// EXPECT_EQ(hello, str);
	EXPECT_EQ(hello.size(), 77);
	EXPECT_FALSE(hello.isEmpty());
	EXPECT_FALSE(hello.isASCII());

	auto utext = unicode::detail::openUText(str);
	auto it = unicode::detail::getCharacterBreakIterator(&utext);
	for (
		auto start = it->first(), end = it->next(), characterIndex = 0;
		end != icu::BreakIterator::DONE;
		start = end, end = it->next(), ++characterIndex
	)
	{
		auto c = hello[characterIndex];
		auto substr = str.substr(start, end - start);
		EXPECT_EQ(c, substr);
	}

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