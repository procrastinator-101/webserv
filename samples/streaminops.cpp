#include <ios>
#include <iostream>
#include <sys/_types/_size_t.h>

int main()
{
	size_t i;
	// std::cin >> std::hex;
	std::cin >> i;

	std::cout << "i : " << i << std::endl;
	if (std::cin.fail())
		std::cout << "fail" << std::endl;
	else
		std::cout << "succes" << std::endl;
	return 0;
}