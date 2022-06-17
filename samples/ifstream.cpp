// print the content of a text file.
#include <fstream>  // std::ifstream
#include <iostream> // std::cout

int main()
{
    std::ifstream ifs;

    ifs.open("client.cpp", std::ifstream::in);

    char c = ifs.get();

    while (ifs.good())
    {
        std::cout << c;
        c = ifs.get();
    }

    ifs.close();

	ifs.open("main.cpp", std::ifstream::in);

	c = ifs.get();

    while (ifs.good())
    {
        std::cout << c;
        c = ifs.get();
    }

    ifs.close();
    return 0;
}