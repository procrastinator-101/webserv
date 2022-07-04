#include "mimeType.hpp"

namespace ft
{
	std::map<std::string, std::string>	MimeType::_library;

	MimeType::MimeType()
	{
	}

	MimeType::~MimeType()
	{
	}

	MimeType::MimeType(const std::string& fileName)
	{
		setMimesTypes(fileName);
	}

	MimeType::MimeType(const MimeType& src)
	{
		(void)src;
	}

	MimeType	&MimeType::operator=(const MimeType& rhs)
	{
		if (this == &rhs)
			return *this;
		_deepCopy(rhs);
		return *this;
	}

	std::string	MimeType::resolve(const std::string& extension)
	{
		std::map<std::string, std::string>::const_iterator	it;

		if (_library.empty())
			setMimesTypes(std::string(NGINY_ROOT_DIR_INC) + "/srcs/mime_type/mimes.txt");
		it = _library.find(extension);
		if (it != _library.end())
			return it->second;
		return std::string();
	}

	void	MimeType::_deepCopy(const MimeType& src)
	{
		(void)src;
	}

	void	MimeType::setMimesTypes(const std::string& fileName)
	{
		std::string		line;
		std::ifstream	file;
		std::vector<std::string> tmp;

		file.open(fileName);
		if (!file.is_open())
			throw std::runtime_error("MIMES:: Could not open file");
		while (file.good())
		{
			std::getline(file, line);
			if (line.empty())
				continue;
			tmp = ft::split(line, ",");
			if (tmp.size() < 2 || tmp[1].empty())
				continue;
			_library[tmp[0]] = tmp[1];
		}
		file.close();
	}
}