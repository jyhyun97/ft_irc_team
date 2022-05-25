#include "../include/Client.hpp"
#include <iostream>

Client::Client(int clientFd) : _clientFd(clientFd) {}

std::string Client::getMsgBuffer()
{
	return _msgBuffer;
}

bool Client::isRegist()
{
	if (_nickName == "" || _userName == "" || _hostName == "" || _realName == "" || _serverName == "")
		return (false);
	return(true);
}

void Client::clearMsgBuffer()
{
	_msgBuffer = "";
}

void Client::appendMsgBuffer(std::string msgBuffer)
{
	_msgBuffer.append(msgBuffer);
}

int Client::getClientFd()
{
	return _clientFd;
}

void Client::addChannelList(std::string channelName)
{
	_myChannelList.push_back(channelName);
}

void Client::setNickName(std::string nickName)
{
	_nickName = nickName;
}

std::string Client::getNickName()
{
	return _nickName;
}

void Client::setUser(std::string userName, std::string hostName, std::string serverName, std::string realName){
	_userName = userName;
	_hostName = hostName;
	_serverName = serverName;
	_realName = realName;
}

std::string Client::getUserName()
{
	return _userName;
}

std::string Client::getHostName()
{
	return _hostName;
}

std::string Client::getServerName()
{
	return _serverName;
}

std::string Client::getRealName()
{
	return _realName;
}

void Client::DebugPrint()
{
	std::cout << "nickName : " << _nickName << std::endl;
	std::cout << "userName : " << _userName << std::endl;
	std::cout << "hostName : " << _hostName << std::endl;
	std::cout << "serverName : " << _serverName << std::endl;
	std::cout << "realName : " << _realName << std::endl;
	std::cout << "clientFd : " << _clientFd << std::endl;
	std::cout << "myChannelList : " << std::endl;
	// print_channelList(_myChannelList);
	std::cout << "msgBuffer : " << _msgBuffer << std::endl;
}
