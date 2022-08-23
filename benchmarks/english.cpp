#include <benchmark/benchmark.h>

#include <string>
#include <fstream>
#include <memory>
#include <random>

#include <unicode/unistr.h>
#include <unicode/brkiter.h>

#include "unicode/string_view.hpp"

#include "../sources/icu.hpp"

/// Get content of a file
static std::string getFileContent(const std::string& filename)
{
	std::ifstream file(filename);
	assert(file && "file not found");
	return std::string{
		std::istream_iterator<char>(file), 
		std::istream_iterator<char>()
	};
}

/// Get english text
static const std::string &getASCII()
{
	static const std::string content = getFileContent("data/english/wiki.txt");
	return content;
}

/// Shuffle indexes of a string with specified size
static std::vector<size_t> shuffleIndexes(size_t size)
{
	std::vector<size_t> indexes(size);
	std::iota(indexes.begin(), indexes.end(), 0);

	std::random_device rd;
	std::shuffle(indexes.begin(), indexes.end(), std::default_random_engine{});
	return indexes;
}

/// Get indexes in string
static const std::vector<size_t> &getIndexes()
{
	static const std::vector<size_t> indexes = shuffleIndexes(
		getASCII().size()
	);
	return indexes;
}

/// Random access to english string with std::string
static void englishWithSTD(benchmark::State& state) 
{
	auto &ascii = getASCII();
	auto &indexes = getIndexes();
	for (auto _ : state)
	{
		auto c = ascii[indexes[state.iterations() % indexes.size()]];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithSTD);

/// Random access to english string with icu::UnicodeString
static void englishWithICUUnicodeString(benchmark::State& state) 
{
	auto &ascii = getASCII();
	auto &indexes = getIndexes();
	icu::UnicodeString str = icu::UnicodeString::fromUTF8(ascii);

	for (auto _ : state)
	{
		auto c = str[indexes[state.iterations() % indexes.size()]];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithICUUnicodeString);

/// Random access to english string with my own implementation
static void englishWithUnicodeStringView(benchmark::State& state) 
{
	auto &ascii = getASCII();
	auto &indexes = getIndexes();

	unicode::string_view str = ascii;

	for (auto _ : state)
	{
		auto c = str[indexes[state.iterations() % indexes.size()]];
		benchmark::DoNotOptimize(c);
	}
}
BENCHMARK(englishWithUnicodeStringView);


BENCHMARK_MAIN();