#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <istream>
#include <iostream>

#include "nginy/Nginy.hpp"
#include "mime_type/mimeType.hpp"

int debug = 1;

int main(int argc, char** argv)
{
	ft::Nginy nginy;
	try
	{
		if (argc != 2)
			throw std::invalid_argument("use ./webser configuration_file");
		
		// std::cout << NGINY_CONF_PATH << std::endl;
		if (debug)
		{
			nginy.fetchConfiguration(argv[1]);
			ft::MimeType::setMimesTypes(std::string(NGINY_ROOT_DIR_INC) + "/srcs/mime_type/mimes.txt");
		}
		else
		{
			std::cout << argv[1] << std::endl;
		}
		// std::cout << nginy << std::endl;
		nginy.up();
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