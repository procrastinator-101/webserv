#include "stdlib.hpp"
#include <string>

namespace ft
{
	std::vector<std::string>	split(const std::string str, const std::string delimiter)
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

	bool isnumber(const std::string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] < '0' || str[i] > '9')
				return false;
		}
		return true;
	}

	std::string	centerString(int width, const std::string& str)
	{
		int len = str.length();
		if (width < len)
			return str;

		int diff = width - len;
		int pad1 = diff / 2;
		int pad2 = diff - pad1;
		return std::string(pad1, ' ') + str + std::string(pad2, ' ');
	}

	std::string	getDisplayFooter(size_t n)
	{
		return std::string( 40 + n, '_');
	}

	std::string	getDisplayHeader(std::string str, size_t n)
	{
		std::string	decoration(20, '_');
		return decoration + centerString(n, str) + decoration;
	}

	std::string	getDisplaySubHeader(std::string str)
	{
		return std::string(10, '-') + " " + str + ":";
	}

	std::string	getDisplaySubFooter(std::string str)
	{
		return std::string(12 + str.size(), '-');
	}
}