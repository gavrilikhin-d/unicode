#include <benchmark/benchmark.h>

#include <string>
#include <fstream>
#include <memory>
#include <random>

#include <unicode/unistr.h>
#include <unicode/brkiter.h>

#include "String.hpp"

/// Read whole file content
static std::string readFile(const std::string &path)
{
	std::ifstream file(path);
	assert(file && "Can't open file");
	std::string content((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));
	return content;
}

using namespace unicode;

#define BENCHMARK_STRING_ITERATOR_LANGUAGE(name) \
	static void name ## StringIterator(benchmark::State& state) \
	{ \
		auto content = readFile("./data/" #name "/wiki.txt"); \
		String unicode = content; \
		for (auto _ : state) \
		{ \
			for (auto c : unicode) \
			{ \
				benchmark::DoNotOptimize(c); \
			} \
		} \
	} \
	BENCHMARK(name ## StringIterator);

#define BENCHMARK_BREAK_ITERATOR_LANGUAGE(name) \
	static void name ## BreakIterator(benchmark::State& state) \
	{ \
		auto content = readFile("./data/" #name "/wiki.txt"); \
		auto utext = detail::openUText(content); \
		auto it = detail::getCharacterBreakIterator(&utext); \
		for (auto _ : state) \
		{ \
			for ( \
				auto start = it->first(), end = it->next(); \
				end != icu::BreakIterator::DONE; \
				start = end, end = it->next() \
			) \
			{ \
				auto c = \
					std::string_view(content.data() + start, end - start); \
				benchmark::DoNotOptimize(c); \
			} \
		} \
		utext_close(&utext); \
	} \
	BENCHMARK(name ## BreakIterator);

#define BENCHMARK_LANGUAGE(name) \
	BENCHMARK_STRING_ITERATOR_LANGUAGE(name) \
	BENCHMARK_BREAK_ITERATOR_LANGUAGE(name)


/* 1-st type of texts */
static void englishStandardStringIterator(benchmark::State& state)
{
	auto content = readFile("./data/english/wiki.txt");
	String unicode = content; 
	for (auto _ : state) 
	{ 
		for (auto c : content) 
		{ 
			benchmark::DoNotOptimize(c); 
		} 
	} 
}
BENCHMARK(englishStandardStringIterator);
BENCHMARK_LANGUAGE(english)
BENCHMARK_LANGUAGE(german)

/* 2-nd type of texts */
BENCHMARK_LANGUAGE(russian)
BENCHMARK_LANGUAGE(french)

/* 3-rd type of texts */
BENCHMARK_LANGUAGE(chinese)
BENCHMARK_LANGUAGE(japanese)
BENCHMARK_LANGUAGE(korean)


BENCHMARK_MAIN();