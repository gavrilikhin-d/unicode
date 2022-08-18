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

#define BENCHMARK_LANGUAGE(name) \
	static void name(benchmark::State& state) \
	{ \
		auto content = readFile("./data/" #name "/wiki.txt"); \
		String unicode = content; \
		for (auto _ : state) \
		{ \
			auto c = unicode[std::rand() % unicode.size()]; \
			benchmark::DoNotOptimize(c); \
		} \
	} \
	BENCHMARK(name);

/* 1-st type of texts */
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