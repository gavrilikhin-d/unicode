#include "unicode/utf8/compare.hpp"
#include "unicode/string_view.hpp"

#include <string>

#include <gtest/gtest.h>

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
		EXPECT_LT(unicode::string_view("1"), unicode::string_view("2"));
	}

	{
		EXPECT_GT(unicode::string_view("в"), unicode::string_view("б"));
	}
}