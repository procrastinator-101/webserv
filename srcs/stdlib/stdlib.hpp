#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <vector>

namespace ft
{
	bool	isnumber(std::string &str);
	
	std::vector<std::string>	split(std::string str, std::string delimiter);
}

#endif