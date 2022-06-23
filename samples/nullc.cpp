#include <fstream>  // std::ifstream
#include <iostream> // std::cout
#include <string>

int main()
{
    std::string	str = "0123456789";

	str[2] = 0;
	if (str.find("5") != std::string::npos)
		std::cout << "found" << std::endl;

	std::cout << str << std::endl;
    return 0;
}