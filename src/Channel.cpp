/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swang <swang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:26:48 by swang             #+#    #+#             */
/*   Updated: 2022/06/02 16:34:25 by swang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel(std::string channelName, int fd) : _operator(fd), _channelName(channelName){};

Channel::~Channel(){};

int Channel::getMyOperator()
{
	return _operator;
}

void Channel::setMyOperator(int fd)
{
	_operator = fd;
	return ;
}

std::string Channel::getChannelName() { return (_channelName); };
std::vector<int> Channel::getMyClientFdList()
{
    return _myClientFdList;
}
void Channel::addMyClientList(int fd)
{
	_myClientFdList.push_back(fd);
}

void Channel::removeClientList(int fd)
{
	std::vector<int>::iterator it = findClient(fd);
	if (it != _myClientFdList.end())
		_myClientFdList.erase(it);
}

std::vector<int>::iterator Channel::findClient(int fd)
{
	std::vector<int>::iterator it = _myClientFdList.begin();
	while (it != _myClientFdList.end())
	{
		if ((*it) == fd)
			return (it);
		it++;
	}
	return (_myClientFdList.end());
}

bool Channel::checkClientInChannel(int fd)
{
	std::vector<int>::iterator it = findClient(fd);
	if (it != _myClientFdList.end())
		return true;
	return false;
}
