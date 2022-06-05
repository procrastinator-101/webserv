#include "Nginy.hpp"

int main(int argc, char** argv)
{
    std::map<int, ft::Server> servers;

    try
    {
		if (argc != 2)
			throw std::invalid_argument("use ./webser configuration_file");
		// initiateServers(argv[1], servers);

    }
    catch (std::exception& e)
    {
		std::cerr << RED;
        std::cerr << e.what() << std::endl;
        std::cerr << BLACK;
        exit(EXIT_FAILURE);
    }
	return EXIT_SUCCESS;
}