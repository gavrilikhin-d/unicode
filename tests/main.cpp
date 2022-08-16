#include "String.hpp"

#include <iostream>

int main()
{
	using namespace unicode;

	String empty;
	std::cout << empty << "\n";
	String repeated = String::repeat('x', Times{3});
	std::cout << repeated << "\n";
	String hello = 
		"🇺🇸: Hello, world!\n"
		"🇷🇺: Привет, мир!\n"
		"🇨🇳: 你好，世界！\n" 
		"🇯🇵: こんにちは世界！\n" 
		"🇰🇷: 안녕하세요 세계!\n"
		"I💜Unicode";
	std::cout << hello << "\n";

	return 0;
}