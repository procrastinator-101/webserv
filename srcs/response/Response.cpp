#include "Response.hpp"
#include <fstream>
#include <ostream>
#include <string>

namespace ft
{
	Response::Response()
	{
	}

	Response::~Response()
	{
	}

	Response::Response(const Response& src)
	{
	}

	Response	&Response::operator=(const Response& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Response::_deepCopy(const Response& src)
	{
	}

	std::ostream	&operator<<(std::ostream& ostr, const Response& response)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::fstream	bodyFile(response._bodyFileName);

		ostr << std::left;
		ostr << getDisplayHeader("Response", RESPONSE_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "version : " << response._version << std::endl;
		ostr << std::setw(fieldSize) << "status : " << response._status.code << " : " << response._status.msg << std::endl;

		ostr << getDisplaySubHeader("headers") << std::endl;
		for (size_t i = 0; i < response._headers.size(); i++)
			ostr << std::setw(fieldSize) << response._headers[i].key << " : " << response._headers[i].value << std::endl;
		ostr << getDisplaySubFooter("headers") << std::endl;

		ostr << getDisplaySubHeader("body") << std::endl;
		ostr << "name : " << response._bodyFileName << std::endl;
		ostr << "=============================================================================" << std::endl;
		while (bodyFile.good())
		{
			std::getline(bodyFile, line);
			ostr << line << std::endl;
		}
		ostr << "=============================================================================" << std::endl;
		ostr << getDisplaySubFooter("body") << std::endl;
		
		ostr << getDisplayFooter(RESPONSE_HSIZE) << std::endl;
		return ostr;
	}
}