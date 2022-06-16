#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <vector>

#define SOCKT_HSIZE			15
#define LOCATION_HSIZE		15
#define REQUEST_HSIZE		15
#define RESPONSE_HSIZE		15

#define CLIENT_HSIZE		30

#define SERVER_HSIZE		40
#define SELECTER_HSIZE		40

#define NGINY_HSIZE			50


namespace ft
{
	bool	isnumber(const std::string &str);
	
	std::vector<std::string>	split(const std::string str, const std::string delimiter);

	std::string	getDisplayFooter(size_t n);
	std::string	getDisplayHeader(std::string str, size_t n);

	std::string	getDisplaySubHeader(std::string str);
	std::string	getDisplaySubFooter(std::string str);
}

#endif