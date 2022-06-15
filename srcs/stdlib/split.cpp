#include "stdlib.hpp"

namespace ft
{
	std::vector<std::string>	split(std::string str, std::string delimiter)
	{
		size_t	len;
		size_t	end;
		size_t	start;
		std::string	token;
		std::vector<std::string>	ret;

		start = 0;
		len = delimiter.length();
		while (start < str.length())
		{
			end = str.find(delimiter, start);
			token = str.substr(start, end - start);
			ret.push_back(token);
			if (end == std::string::npos)
				break ;
			start = end + len;
		}
		return ret;
	}
}