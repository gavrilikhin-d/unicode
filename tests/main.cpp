#include "String.hpp"

#include <iostream>

int main()
{
	using namespace unicode;

	/// TODO: google tests

	{
		String empty;
		std::cout << "Empty string: " << empty << "\n";
		std::cout << "Size: " << empty.size() << "\n";
		std::cout << "Is ASCII: " << empty.isASCII() << "\n";
		assert(empty.isEmpty() && empty.size() == 0 && empty.isASCII());
	}

	std::cout << "\n";

	{
		String::SizeType times = 3;
		auto repeated = String::repeat('x', Times{times});
		std::cout << "Repeated string: " << repeated << "\n";
		std::cout << "Size: " << repeated.size() << "\n";
		std::cout << "Is ASCII: " << repeated.isASCII() << "\n";
		assert(
			not repeated.isEmpty() && 
			repeated.size() == times && 
			repeated.isASCII()
		);
	}


	std::cout << "\n";

	{
		String hello = 
			"🇺🇸: Hello, world!\n"
			"🇷🇺: Привет, мир!\n"
			"🇨🇳: 你好，世界！\n" 
			"🇯🇵: こんにちは世界！\n" 
			"🇰🇷: 안녕하세요 세계!\n"
			"I💜Unicode";
		std::cout << "Hello string:\n" << hello << "\n";
		std::cout << "Size: " << hello.size() << "\n";
		std::cout << "Is ASCII: " << hello.isASCII() << "\n";
		assert(
			not hello.isEmpty() && 
			hello.size() == 77 && 
			not hello.isASCII()
		);
	}

	return 0;
}