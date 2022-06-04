#include "HeaderField.hpp"

namespace ft
{
	HeaderField::HeaderField()
	{
	}

	HeaderField::~HeaderField()
	{
	}

	HeaderField::HeaderField(const HeaderField& src) : key(src.key), value(src.value)
	{
	}

	HeaderField::HeaderField(const std::string& key, const std::string& value) : key(key), value(value)
	{
	}

	HeaderField	&HeaderField::operator=(const HeaderField& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	HeaderField::_deepCopy(const HeaderField& src)
	{
		key = src.key;
		value = src.value;
	}
}