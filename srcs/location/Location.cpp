#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	void	Location::threat_root(std::stringstream& streamLine, std::string& token)
	{
		streamLine >> token;
		if (token != "#")
			_root = token;
		else
			throw std::runtime_error("Location: root is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location: too many arguments for root");
		}
	}

	void	Location::threat_methods(std::stringstream& streamLine, std::string& token)
	{
		int size = _methods.size();
		while (streamLine.good())
		{
			streamLine >> token;
			if (token == "#")
				break ;
			if (token == "GET" || token == "POST" || token == "DELETE")
				_methods.insert(token);
			else
				throw std::runtime_error("Location: method is not valid");
		}
		if (_methods.size() - size == 0)
			throw std::runtime_error("Location: methods is not valid");
	}

	void	Location::threat_indexes(std::stringstream& streamLine, std::string& token)
	{
		int size = _indexes.size();
		while (streamLine.good())
		{
			streamLine >> token;
			if (token == "#")
				break ;
			_indexes.insert(token);
		}
		if (_indexes.size() - size == 0)
			throw std::runtime_error("Location: indexes is not valid");
	}

	void	Location::threat_upload(std::stringstream& streamLine, std::string& token)
	{
		streamLine >> token;
		if (token != "#")
			_uploadPath = token;
		else
			throw std::runtime_error("Location: uploadPath is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location: too many arguments for uploadPath");
		}
	}

	void	Location::threat_autoindex(std::stringstream& streamLine, std::string& token)
	{
		static int i = 0;

		if (i == 1)
			throw std::runtime_error("Location: multiple autoIndex");
		streamLine >> token;
		if (token == "on")
			_autoIndex = true;
		else if (token != "off")
			throw std::runtime_error("Location: autoindex is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location: too many arguments for autoindex");
		}
		i = 1;
	}

	static	bool is_num(std::string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] < '0' || str[i] > '9')
				return false;
		}
		return true;
	}

	void	Location::threat_return(std::stringstream& streamLine, std::string& token)
	{
		std::string code_str;
		lineStream >> code_str;
		if (lineStream.good() && is_num(code_str))
		{
			int code = ::atoi(code_str.c_str());
			try
			{
				HttpStatus::resolve(code);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Location: return: invalid code");
			}
			lineStream >> token;
			_redirection = std::make_pair(code, token);
		}
		else
			throw std::runtime_error("Location: invalid code/path");
		if (lineStream.good())
		{
			lineStream >> token;
			if (token != "#")
				throw std::runtime_error("Location: too many arguments for return");
		}
	}
	



	void	Location::threat_line(std::stringstream& streamLine, std::string& token)
	{
		if (token == "root" && streamLine.good())
			threat_root(streamLine, token);
		else if (token == "methods" && streamLine.good())
			threat_methods(streamLine, token);
		else if (token == "index" && streamLine.good())
			threat_index(streamLine, token);
		else if (token == "upload" && streamLine.good())
			threat_upload(streamLine, token);
		else if (token == "autoindex" && streamLine.good())
			threat_autoindexd(streamLine, token);
		else if (token == "return" && streamLine.good())
			threat_return(streamLine, token);
	}

	Location::Location(std::ifstream& configFile, std::string& root, status autoIndex, std::set<std::string>& indexes, std::set<std::string>& methods) : _root(NULL), _autoIndex(false), _methods(), _indexes(), _redirections(), _uploadPath(NULL)
	{
		int				inside_location = 0;
		std::string		line;

		std::getline(configFile, line);
		std::stringstream	streamLine(line);
		std::string			token;
		streamLine >> token;
		if (token != "{")
			throw std::runtime_error("Location: config file is not valid");
		
		while (std::getline(configFile, line))
		{
			inside_location = 1;
			if (line.empty())
				continue ;
			streamLine.clear();
			streamLine.str(line);
			streamLine >> token;
			if (token == "#")
				continue ;
			if (token == "}")
			{
				inside_location = 0;
				break ;
			}
			threat_line(streamLine, token);
		}
		if (inside_location == 1)
			throw std::runtime_error("Location: config file is not valid");

		// take server data
		if (_root == NULL)
			_root = root;
		if (_autoIndex == false)
			_autoIndex = autoIndex;
		if (_indexes.empty())
		{
			if (indexes.empty() && _autoIndex == false) 
				throw std::runtime_error("Location: config file is not valid");
			else if (!indexes.empty())
				_indexes = indexes;
		}
		if (_methods.empty())
		{
			if (methods.empty())
				throw std::runtime_error("Location: config file is not valid");
			else
				_methods = methods;
		}
	}

	Location::Location(const Location& src) :	_root(src._root), _autoIndex(src._autoIndex), _methods(src._methods), _indexes(src._indexes),
												_redirections(src._redirections), _uploadPath(src._uploadPath)
	{
	}

	Location	&Location::operator=(const Location& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Location::_deepCopy(const Location& src)
	{
		_root = src._root;
		_autoIndex = src._autoIndex;
		_methods = src._methods;
		_indexes = src._indexes;
		_redirections = src._redirections;
		_uploadPath = src._uploadPath;
	}
}