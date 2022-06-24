#include "stdlib.hpp"
#include <stdexcept>
#include <sys/_types/_gid_t.h>
#include <sys/_types/_uid_t.h>
#include <unistd.h>

namespace ft
{
	//split by a string delimiter, no multiple traversal
	std::vector<std::string>	split(const std::string& str, const std::string& delimiter)
	{
		size_t	len;
		size_t	end;
		size_t	start;
		std::vector<std::string>	ret;

		start = 0;
		len = delimiter.length();
		while (start < str.length())
		{
			end = str.find(delimiter, start);
			ret.push_back(str.substr(start, end - start));
			if (end == std::string::npos)
				break ;
			start = end + len;
		}
		return ret;
	}

	//split by a string of delimiters, multiple traversal
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

	//split by a string delimiter, no multiple traversal
	std::string	strdtok(std::string& str, const std::string& delimiter)
	{
		size_t	pos;
		std::string	ret;

		pos = str.find(delimiter);
		if (pos != std::string::npos)
		{
			ret = str.substr(0, pos);
			str.erase(0, pos + delimiter.length());
		}
		else
			ret.swap(str);
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
				for (i = i + 1; i < str.length() && delimiters.find(str[i]) != std::string::npos; i++);
				str.erase(0, i);
				return ret;
			}
			else
				i++;
		}
		str.swap(ret);
		return ret;
	}

	size_t	stoz(const std::string& str)
	{
		size_t	ret;
		std::stringstream	sstream(str);

		sstream >> ret;
		if (sstream.fail())
			throw std::overflow_error("stoz overflow");
		return ret;
	}

	size_t	hstoz(const std::string& str)
	{
		size_t	ret;
		std::stringstream	sstream(str);

		sstream >> std::hex;
		sstream >> ret;
		if (sstream.fail())
			throw std::overflow_error("hstoz overflow");
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
	size_t	getFileSize(const std::string& name)
	{
		struct stat	fileStat;

		lstat(name.c_str(), &fileStat);
		return fileStat.st_size;
	}

	bool	isUserInGroup(gid_t gid, uid_t uid)
	{
		int				ngroup;
		int				*groups;
		struct passwd	*userInfo;
		
		userInfo = getpwuid(uid);
		if (!userInfo)
			return false;
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, 0, &ngroup);
		groups = new int[ngroup];
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, groups, &ngroup);
		for (int i = 0; i < ngroup; i++)
		{
			if (groups[i] == gid)
				return true;
		}
		delete [] groups;
		return false;
	}

	bool	isFileWritable(std::string& name)
	{
		int		ret;
		uid_t	uid;
		struct stat	fileStat;

		uid = getuid();
		ret = lstat(name.c_str(), &fileStat);
		if (ret < 0)
			return false;
		//write other
		if (fileStat.st_mode & S_IWOTH)
			return true;
		//write group
		if (fileStat.st_mode & S_IWGRP)
		{
			if (isUserInGroup(fileStat.st_gid, uid))
				return true;
		}
		//write user
		if (fileStat.st_mode & S_IWUSR)
		{
			if (uid == fileStat.st_uid)
				return true;
		}
		return false;
	}
}