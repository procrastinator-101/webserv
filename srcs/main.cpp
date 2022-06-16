#include "http_status/HttpStatus.hpp"
#include "nginy/Nginy.hpp"
#include "request/Request.hpp"
#include <iostream>
#include <istream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
            throw std::invalid_argument("use ./webser configuration_file");
        
        // ft::Nginy   nginy(argv[1]);
        // nginy.up();


        //for debugging only : until parsing is done
        std::cout << argv[1] << std::endl;

        ft::Nginy   nginy;
        // std::cout << nginy << std::endl;
        nginy.up();
    }
    catch (std::exception& e)
    {
        std::cerr << RED;
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << BLACK;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}