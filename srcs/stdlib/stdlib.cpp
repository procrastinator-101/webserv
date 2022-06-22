#include "stdlib.hpp"
#include <_ctype.h>
#include <string>
#include <sys/_types/_size_t.h>

namespace ft
{
	std::vector<std::string>	split(const std::string& str, const std::string& delimiter)
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

	std::vector<std::string>	splitWhiteSpaces(const std::string& str)
	{
		std::string	token;
		std::stringstream			tmp;
		std::vector<std::string>	ret;

		tmp << str;
		while (tmp.good())
		{
			tmp >> token;
			ret.push_back(token);
		}
		return ret;
	}

	std::string	strtok(std::string& str, int delimiter)
	{
		std::string	token;
		std::stringstream	tmp;

		tmp << str;
		if (tmp.good())
		{
			if (delimiter == -1)
				tmp >> token;
			else
				std::getline(tmp, token, static_cast<char>(delimiter));
			str = tmp.str();
		}
		return token;
	}

	std::vector<std::string>	split(const std::string& str, char delimiter)
	{
		std::string	token;
		std::stringstream			tmp;
		std::vector<std::string>	ret;

		tmp << str;
		while (tmp.good())
		{
			std::getline(tmp, token, delimiter);
			ret.push_back(token);
		}
		return ret;
	}

	std::string	removeTrailingWhiteSpaces(const std::string& str)
	{
		size_t	i;

		for (i = 0; i < str.length(); i++)
		{
			if (!::isspace(str[i]))
				break ;
		}
		if (i == 0)
			return str;
		return str.substr(i);
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

	std::string	centerString(int width, const std::string& str, char c)
	{
		int len = str.length();
		if (width < len)
			return str;

		int diff = width - len;
		int pad1 = diff / 2;
		int pad2 = diff - pad1;
		return std::string(pad1, c) + str + std::string(pad2, c);
	}

	std::string	getDisplayFooter(size_t n)
	{
		return std::string( 40 + n, '-');
	}

	std::string	getDisplayHeader(std::string str, size_t n)
	{
		std::string	decoration(20, '_');
		return decoration + centerString(n, str, '_') + decoration;
	}

	std::string	getDisplaySubHeader(std::string str)
	{
		return std::string(10, '=') + " " + str + ":";
	}

	std::string	getDisplaySubFooter(std::string str)
	{
		return std::string(12 + str.size(), '=');
	}

	std::string	ft_itoa(int n)
	{
		std::stringstream	ss;

		ss << n;
		return ss.str();
	}

	std::string	getRandomFileName()
	{
		return std::string("file" + ft_itoa(time(0)));
	}

	//returns the file size in bytes
	size_t	getFileSize(const std::string& fileName)
	{
		struct stat	fileStat;

		lstat(fileName.c_str(), &fileStat);
		return fileStat.st_size;
	}
}