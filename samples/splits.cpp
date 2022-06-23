#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

std::vector<std::string>	mtsplit(const std::string& str, const std::string& delimiters)
{
	size_t	i;
	size_t	pos;
	size_t	start;
	std::vector<std::string>	ret;

	i = 0;
	start = 0;
	while (i < str.length())
	{
		pos = delimiters.find(str[i]);
		if (pos != std::string::npos)
		{
			ret.push_back(str.substr(start, i - start));
			for (; i < str.length() && delimiters.find(str[i]) != std::string::npos; i++);
			start = i;
		}
		else
			i++;
	}
	if (start < str.length())
		ret.push_back(str.substr(start));
	return ret;
}

//split by a string of delimiters, multiple traversal
std::string	strtok(std::string& str, const std::string& delimiters)
{
	size_t	i;
	size_t	pos;
	std::string	ret;

	i = 0;
	while (i < str.length())
	{
		pos = delimiters.find(str[i]);
		if (pos != std::string::npos)
		{
			ret = str.substr(0, i);
			while (i < str.length() && delimiters.find(str[i]) != std::string::npos)
				i++;
			str.erase(0, i);
			return ret;
		}
		else
			i++;
	}
	str.swap(ret);
	return ret;
}

int	main()
{
	std::string	str = "n m  l\t\v   k\t\n\v\f\r o";
	std::string delimiters = "\t\n\v\f\r ";

	std::vector<std::string>	v;
	v = mtsplit(str, delimiters);
	for (size_t i = 0; i < v.size(); i++)
		std::cout << "{" << v[i] << "}" << std::endl;

	std::cout << "--------------------------------------------------------" << std::endl;
	while (!str.empty())
	{
		std::cout << "{" << strtok(str, delimiters) << "}" << std::endl;
	}
	return 0;
}