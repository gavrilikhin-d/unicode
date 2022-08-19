#include "String.hpp"

#include <iostream>
#include <fstream>

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

TEST(Functionality, negativeIndexes)
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

TEST(Functionality, iteration)
{
	std::string str =
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";

	String hello = str;

	for (auto c : hello)
	{
		EXPECT_EQ(c, str.substr(0, c.size()));
		str.erase(0, c.size());
	}
}

/// Read whole file content
static std::string readFile(const std::string &path)
{
	std::ifstream file(path);
	assert(file && "Can't open file");
	std::string content((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));
	return content;
}

TEST(English, wiki)
{
	auto content = readFile("../../data/english/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(Russian, wiki)
{
	auto content = readFile("../../data/russian/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(Chinese, wiki)
{
	auto content = readFile("../../data/chinese/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(French, wiki)
{
	auto content = readFile("../../data/french/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(German, wiki)
{
	auto content = readFile("../../data/german/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(Japanese, wiki)
{
	auto content = readFile("../../data/japanese/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}

TEST(Korean, wiki)
{
	auto content = readFile("../../data/korean/wiki.txt");
	String unicode = content;
	EXPECT_FALSE(unicode.isEmpty());
	EXPECT_TRUE(areEqual(unicode, content));
}