#include <benchmark/benchmark.h>

#include <string>
#include <fstream>
#include <memory>

#include <unicode/unistr.h>
#include <unicode/brkiter.h>

#include "String.hpp"

/// Random access to english string with std::string
static void englishWithSTD(benchmark::State& state) 
{
	std::ifstream file("data/english/wiki.txt");

	std::string ascii{
		std::istream_iterator<char>(file), 
		std::istream_iterator<char>()
	};
	for (auto _ : state)
	{
		auto c = ascii[std::rand() % ascii.size()];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithSTD);

/// Random access to english string with icu::UnicodeString
static void englishWithICUUnicodeString(benchmark::State& state) 
{
	std::ifstream file("data/english/wiki.txt");

	std::string ascii{
		std::istream_iterator<char>(file), 
		std::istream_iterator<char>()
	};

	icu::UnicodeString str = icu::UnicodeString::fromUTF8(ascii);

	for (auto _ : state)
	{
		auto c = str[std::rand() % str.length()];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithICUUnicodeString);

/// Random access to english string with icu::BreakIterator
static void englishWithICUBreakIterator(benchmark::State& state) 
{
	std::ifstream file("data/english/wiki.txt");

	std::string ascii{
		std::istream_iterator<char>(file), 
		std::istream_iterator<char>()
	};

	auto utext = unicode::detail::openUText(ascii);
	auto it = unicode::detail::getCharacterBreakIterator(&utext);

	for (auto _ : state)
	{
		it->first();
		size_t index = std::rand() % ascii.size();
		for (size_t i = 0; i < index; ++i)
		{
			it->next();
		}
		auto c = ascii[it->current()];
		benchmark::DoNotOptimize(c);
	}

	utext_close(&utext);
}
BENCHMARK(englishWithICUBreakIterator);

/// Random access to english string with my own implementation
static void englishWithMyString(benchmark::State& state) 
{
	std::ifstream file("data/english/wiki.txt");

	std::string ascii{
		std::istream_iterator<char>(file), 
		std::istream_iterator<char>()
	};

	unicode::String str = unicode::String::fromASCII(ascii);

	for (auto _ : state)
	{
		auto c = str[std::rand() % str.size()];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithMyString);


BENCHMARK_MAIN();