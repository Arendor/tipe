

#include "utils.h"
#include <random>
#include <iostream>
//#include <chrono>

std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd());

std::unordered_map<int, std::string> color_to_str = {
	{0, "a"},
	{1, "b"},
	{2, "c"},
	{3, "d"},
	{4, "e"},
	{5, "f"}
};

std::unordered_map<char, int> c_to_color = {
	{'a',0},
	{'b',1},
	{'c',2},
	{'d',3},
	{'e',4},
	{'f', 5}
};

std::string utils::numberCardToStr(int card)
{
	//std::cout << utils::getColor(card);

	return std::to_string(utils::getValue(card) + 1) + color_to_str[utils::getColor(card)];

}

int utils::getColorFromChar(char c)
{
	return c_to_color[c];
}

bool utils::isColor(char c)
{
	return c_to_color.contains(c);
}
