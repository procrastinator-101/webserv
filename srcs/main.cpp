#include "http_status/HttpStatus.hpp"
#include "nginy/Nginy.hpp"
#include "request/Request.hpp"
#include <fcntl.h>
#include <iostream>
#include <istream>
#include <string>
#include <unistd.h>

int debug = 1;

int main(int argc, char** argv)
{
	ft::Nginy nginy;
	try
	{
		if (argc != 2)
			throw std::invalid_argument("use ./webser configuration_file");
		
		if (debug)
		{
			nginy.fetchConfiguration(argv[1]);
		}
		else
		{
			std::cout << argv[1] << std::endl;
		}
		std::cout << nginy << std::endl;
		// nginy.up();
	}
	catch (std::exception& e)
	{
		std::cout << nginy << std::endl;
		std::cerr << RED;
		std::cerr << "Error" << std::endl;
		std::cerr << e.what() << std::endl;
		std::cerr << BLACK;
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}