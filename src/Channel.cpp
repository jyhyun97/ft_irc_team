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

