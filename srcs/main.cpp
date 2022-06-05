#include "http_status/HttpStatus.hpp"
#include "nginy/Nginy.hpp"

int main(int argc, char** argv)
{
    std::map<int, ft::Server> servers;

    try
    {
        if (argc != 2)
            throw std::invalid_argument("use ./webser configuration_file");
		std::cout << argv[1] << std::endl;
        // initiateServers(argv[1], servers);
		std::cout << ft::HttpStatus::resolve(200) << std::endl;
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