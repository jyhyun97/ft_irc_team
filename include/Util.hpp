#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <iostream>


std::string appendStringColon(size_t startIndex, std::vector<std::string> msg);//privmsg #seoul

std::vector<std::string> split(std::string &line, std::string s);

void print_stringVector(std::vector<std::string> v);

#endif
