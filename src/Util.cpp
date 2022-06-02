/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swang <swang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:26:25 by swang             #+#    #+#             */
/*   Updated: 2022/06/02 16:26:26 by swang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Util.hpp"

# define C_NRML "\033[0m"
# define C_GREN "\033[32m" 


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
}

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
	return tab;
}

void print_stringVector(std::vector<std::string> v)
{
	std::vector<std::string>::iterator it = v.begin();
	std::cout << C_GREN << "----------in print_stringVector\n";
	while (it != v.end())
	{
		std::cout << "[" << (*it) << "]" << std::endl;
		it++;
	}
	std::cout << "ㄴ------------------\n\n" << C_NRML;
}
