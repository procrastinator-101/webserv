//trim by delimiters
	std::string	trim(const std::string& str, const std::string& delimiters)
	{
		size_t	end;
		size_t	start;

		if (str.empty())
			return str;
		start = 0;
		while (start < str.length() && delimiters.find(str[start]) != std::string::npos)
			start++;
		end = str.length() - 1;
		while (end > start && delimiters.find(str[end]) != std::string::npos)
			end--;
		return str.substr(start, end + 1 - start);
	}