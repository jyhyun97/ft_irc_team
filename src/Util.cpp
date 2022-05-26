#include "../include/Util.hpp"





std::string appendStringColon(size_t startIndex, std::vector<std::string> msg)
{
	std::string result = "";
	for (size_t i = startIndex; i < msg.size() - 1; i++)
	{
		result.append(msg[i]);
		result.append(" ");
	}
	result.append(msg[msg.size() - 1]);
	return result;
}//privmsg #seoul

std::vector<std::string> split(std::string &line, std::string s)
{
	std::vector<std::string> tab;
	std::string cmd_buf;
	size_t start = 0;
	size_t pos;

	while ((pos = line.find(s)) != std::string::npos){
		tab.push_back(line.substr(start, pos));
		line = line.substr(pos + s.length());
	}
	if (line != "")
		tab.push_back(line);
	// for (size_t i = 0; i < tab.size(); i++)
	// {
	// 	std::cout << "[[" <<  tab[i] << "]]" << std::endl;
	// }
	return tab;
}

void print_stringVector(std::vector<std::string> v)
{
	std::vector<std::string>::iterator it = v.begin();
	while (it != v.end())
	{
		std::cout << "[" << (*it) << "]" << std::endl;
		it++;
	}
}
