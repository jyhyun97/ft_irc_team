#include "../include/Channel.hpp"

Channel::Channel(std::string channelName) : _channelName(channelName){};
Channel::~Channel(){};
std::string Channel::getChannelName() { return (_channelName); };
std::vector<int> Channel::getMyClientFdList()
{
    return _myClientFdList;
}
void Channel::addMyClientList(int fd)
{
	_myClientFdList.push_back(fd);
};

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