#include "Header.hpp"

namespace ft
{
	Header::Header()
	{
	}

	Header::~Header()
	{
	}

	Header::Header(const Header& src) : key(src.key), value(src.value)
	{
	}

	Header::Header(const std::string& key, const std::string& value) : key(key), value(value)
	{
	}

	Header	&Header::operator=(const Header& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Header::_deepCopy(const Header& src)
	{
		key = src.key;
		value = src.value;
	}
}