#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <vector>

namespace ft
{
	bool	isnumber(const std::string &str);
	
	std::vector<std::string>	split(const std::string str, const std::string delimiter);
}

#endif