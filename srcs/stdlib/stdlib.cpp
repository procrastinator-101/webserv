#include "stdlib.hpp"
#include <_ctype.h>
#include <exception>
#include <new>
#include <stdexcept>
#include <string>
#include <sys/_types/_size_t.h>
#include <sys/_types/_time_t.h>
#include <sys/_types/_timeval.h>
#include <vector>


namespace ft
{
	//return timestamp in milliseconds
	long	getTimeStamp(timeval start)
	{
		long	timestamp;
		timeval	now;

		gettimeofday(&now, 0);
		timestamp = (now.tv_sec - start.tv_sec) * 1000000;
		timestamp = timestamp + now.tv_usec - start.tv_usec;
		return (timestamp / 1000);
	}

	//left trim by delimiters
	std::string	ltrim(const std::string& str, const std::string& delimiters)
	{
		size_t	pos;

		pos = str.find_first_not_of(delimiters);
		if (pos == std::string::npos)
			return std::string();
		return str.substr(pos);
	}

	//right trim by delimiters
	std::string	rtrim(const std::string& str, const std::string& delimiters)
	{
		size_t	pos;

		pos = str.find_last_not_of(delimiters);
		if (pos == std::string::npos)
			return std::string();
		return str.substr(0, pos + 1);
	}

	//trim by delimiters
	std::string	trim(const std::string& str, const std::string& delimiters)
	{
		return ltrim(rtrim(str, delimiters), delimiters);
	}

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

	int	stoi(const std::string& str)
	{
		int	ret;
		std::stringstream	sstream(str);

		sstream >> ret;
		if (sstream.fail())
			throw std::overflow_error("stoi overflow");
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

	std::string	ztoa(size_t n)
	{
		std::stringstream	sstream;

		sstream << n;
		if (sstream.fail())
			return std::string();
		return sstream.str();
	}

	std::string	itoa(int n)
	{
		std::stringstream	sstream;

		sstream << n;
		if (sstream.fail())
			return std::string();
		return sstream.str();
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

	bool	is_path_valid(const std::string& path)
	{
		struct stat s;

		if (stat(path.c_str(), &s) == 0)
			return true;
		return false;
	}

	std::string	ft_itoa(int n)
	{
		std::stringstream	ss;

		ss << n;
		return ss.str();
	}

	std::string	getRandomFileName(const std::string& extension)
	{
		timeval	ret;

		gettimeofday(&ret, 0);
		return std::string(extension + ztoa(ret.tv_sec) + "_" + ztoa(ret.tv_usec));
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
		int				ret;
		int				ngroup;
		int				*groups;
		struct passwd	*userInfo;
		
		userInfo = getpwuid(uid);
		if (!userInfo)
			return false;
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, 0, &ngroup);
		groups = new int[ngroup];
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, groups, &ngroup);
		ret = false;
		for (int i = 0; i < ngroup; i++)
		{
			if (groups[i] == static_cast<int>(gid))
			{
				ret = true;
				break;
			}
		}
		delete [] groups;
		return ret;
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

	char	*ft_strdup(const char *s1)
	{
		size_t		len;
		char		*ret;

		len = strlen(s1);
		ret = new(std::nothrow) char[len + 1];
		if (!ret)
			return 0;
		for (size_t i = 0; i < len; i++)
			ret[i] = s1[i];
		ret[len] = 0;
		return ret;
	}

	char	**vec2arr(std::vector<std::string>& vec)
	{
		char	**ret;

		ret = new (std::nothrow) char *[vec.size() + 1];
		if (!ret)
			return ret;
		for (size_t i = 0; i < vec.size(); i++)
		{
			ret[i] = ft_strdup(vec[i].c_str());
			if (!ret[i])
			{
				destroy2arr(ret, i);
				return 0;
			}
		}
		ret[vec.size()] = 0;
		return ret;
	}

	void	destroy2arr(char **arr, size_t n)
	{
		for (size_t i = 0; i < n; i++)
			delete [] arr[i];
		delete [] arr;
	}

	char	decodePercent(std::string& src)
	{
		char	ret;

		if (src.size() != 3)
			throw std::invalid_argument("decodePercent:: invalid argument");
		try
		{
			ret = hstoz(src.substr(1, 2));
			std::cout << "fmt : " << src.substr(1, 2) << " : " << (int)ret << std::endl;
		}
		catch (std::exception& e)
		{
			throw std::invalid_argument("decodePercent:: invalid argument");
		}
		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HTTP Parsing Grammar
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace http
	{
		bool	isTchar(int c)
		{
			if (!::isprint(c))
				return false;
			return !isDelimiter(c);
		}

		bool	isDelimiter(int c)
		{
			static std::string	delimiters = "\"(),/:;<=>?@[\\]{}\"";

			return delimiters.find(c) != std::string::npos;
		}

		bool	isQdtext(int c)
		{
			if (c == '\t' || c == ' ' || c == 0x21)
				return true;
			if (c >= 0x23 && c <= 0x5b)
				return true;
			if (c >= 0x5d && c <= 0x7e)
				return true;
			return isObstext(c);
		}

		bool	isObstext(int c)
		{
			return c >= 0x80 && c <= 0xff;
		}

		bool	isCtext(int c)
		{
			if (c == '\t' || c == ' ')
				return true;
			if (c >= 0x21 && c <= 0x27)
				return true;
			if (c >= 0x2a && c <= 0x5b)
				return true;
			if (c >= 0x5d && c <= 0x7e)
				return true;
			return isObstext(c);
		}

		bool	isUnreserved(int c)
		{
			if (isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '_' || c =='~')
				return true;
			return false;
		}

		bool	isPctEncoded(int c)//
		{
			return c == 0;
		}

		bool	isSubDelim(int c)//
		{
			return c == 0;
		}

		bool	isPchar(int c)
		{
			if (isUnreserved(c) || isPctEncoded(c) || isSubDelim(c))
				return true;
			return c == ':' || c == '@';
		}

		bool	isQuery(const std::string& query)
		{
			for (size_t i = 0; i < query.length(); i++)
			{
				if (!isPchar(query[i]) && query[i] != '/' && query[i] != '?')
					return false;
			}
			return true;
		}

		bool	isFragment(const std::string& fragment)
		{
			for (size_t i = 0; i < fragment.length(); i++)
			{
				if (!isPchar(fragment[i]) && fragment[i] != '/' && fragment[i] != '?')
					return false;
			}
			return true;
		}

		bool	isPort(const std::string& port)
		{
			size_t	tmp;

			try
			{
				tmp = stoz(port);
				if (tmp > 65535)
					return false;
			}
			catch (std::exception& e)
			{
				return false;
			}
			return true;
		}

		std::string	headerToEnv(const std::string& header)
		{
			std::string	ret(header);

			for (size_t i = 0; i < ret.length(); i++)
			{
				if (ret[i] == '-')
					ret[i] = '_';
				else if (islower(ret[i]))
					ret[i] = toupper(ret[i]);
			}
			return "HTTP_" + ret;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HTTP Parsing Grammar End
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
}