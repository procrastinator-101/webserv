#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "srcs/response/Response.hpp"
#include "srcs/stdlib/stdlib.hpp"


void	paresMimes(std::map<std::string, std::string>& _library)
{
	std::vector<std::string>	tmp;
	std::ifstream				file;
	std::string					line;

	file.open("./mimes.txt");
	if (!file.is_open())
		throw std::runtime_error("MIMES:: Could not open file");
	while (file.good())
	{
		std::getline(file, line);
		if (line.empty())
			continue ;
		tmp = ft::split(line, ",");
		if (tmp.size() < 2 || tmp[1].empty())
			continue ;
		_library[tmp[0]] = tmp[1];
	}
	file.close();
}

void	setContentType(std::map<std::string, std::string>& _library, std::string ext)
{
	std::map<std::string, std::string>::const_iterator	it = _library.find(ext);

	if (it == _library.end())
		std::cout << "Content-Disposition" << " : " << "attachement" << std::endl;
	else
		std::cout << "Content-Type" << " : " << it->second << std::endl;
}

int main()
{
	std::map<std::string, std::string>	_library;
	paresMimes(_library);
	setContentType(_library, "jpeg");
	// for (std::map<std::string, std::string>::const_iterator it = _library.begin(); it != _library.end(); ++it)
	// {
	// 	std::cout << it->first << "   |   " << it->second << std::endl;
	// }
}