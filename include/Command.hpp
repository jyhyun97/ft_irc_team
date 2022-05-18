#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>

class Server;

void print_channel(std::vector<Channel*> v)
{
	std::vector<Channel *>::iterator it = v.begin();
	std::cout << "channel begin: " << (*(v.begin()))->getChannelName() << std::endl;
	while(it != v.end())
	{
		std::cout << "print channel: " << (*it)->getChannelName() << std::endl;
		it++;
	}
}

std::vector<std::string> cmd_split(std::string &line, char c)
{
	std::vector<std::string> tab;
	std::string word_buf;
	bool space = false;

	for (int i = 0; i < (int)line.size(); i++)
	{
		if (line[i] == c && space == false)
		{
			space = true;
			if (word_buf.size() > 0)
				tab.push_back(word_buf);
			word_buf.clear();
		}
		else
		{
			word_buf += line[i];
			if (line[i] != c)
				space = false;
		}
	}
	if (word_buf.size() > 0)
		tab.push_back(word_buf);
	return tab;
}
//템플릿 함수로 만들 필요성이 있을까?
std::vector<Channel *>::iterator find(std::vector<Channel *> search, std::string item){
	std::vector<Channel *>::iterator it = search.begin();
	while (it != search.end()){
		if ((*it)->getChannelName() == item)
			return (it);
		it++;
	}
	return (it);
}

class Command
{
    public :
    void nick(std::vector<std::string> s){std::cout << "i am nick\n";};//NICK <parameter>
    void user(std::vector<std::string> s){std::cout << "i am user\n";};//USER <username> <hostname> <servername> <realname>
    void join(std::vector<std::string> s, Client *client, std::vector<Channel *> &channelList){//JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
		//JOIN #foo,#bar
		std::vector<std::string> joinChannel = cmd_split(s[1], ',');
		std::vector<std::string>::iterator it = joinChannel.begin();
		while (it != joinChannel.end())
		{
			//if (중복아니면)
			//	새 채널 만들기; 서버.채널리스트에 갱신
			if (find(channelList, *it) == channelList.end())
			{
				std::cout << "channel name " << *it << std::endl;
				channelList.push_back(new Channel(*it));
				std::cout << "add channel name" << channelList.back()->getChannelName() << std::endl;
			}
			//클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신//join	
			// 들어간채널|| 찾은 채널 .클라이언트갱신()
			(*find(channelList, *it))->addMyClientList(client);
			// client.갱신채널리스트()
			client->addChannelList((*find(channelList, *it)));
			it++;
		}
		print_channel(channelList);

		
		//파싱해서 채널 이름 따오고
		//서버.채널리스트에 없으면 채널 객체 만들어서 채널 리스트에 추가
		//클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신

		std::cout << "i am join\n";
	};
	void kick(std::vector<std::string> s){std::cout << "i am kick\n";};//KICK <channel> <user> [<comment>]//KICK <channel> <user> [<comment>]
	void privmsg(std::vector<std::string> s, Client *client){
		std::stringstream ss;
		ss << client->getClientFd();
		std::string str = ss.str();
		
		client->setMsgBuffer("001 babo :42424242 [" + str + "]\r\n");
		send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		client->getMsgBuffer().clear();

		std::cout << "i am privmsg\n";};//PRIVMSG <msgtarget> <text to be sent>
	void pass(std::vector<std::string> s){std::cout << "i am pass\n";};//PASS <password>
	void part(std::vector<std::string> s){std::cout << "i am part\n";};//PART <channel> *( "," <channel> ) [ <Part Message> ]
	void quit(std::vector<std::string> s){std::cout << "i am quitt\n";};//QUIT [<Quit message>]
	void whois(std::vector<std::string> s, Client *client){
		client->setMsgBuffer("311 jeonhyun jeonhyun 127.0.0.1 * :현정연\r\n");
		send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		client->getMsgBuffer().clear();
		std::cout << "i am whois\n";
	};
	void welcome(std::vector<std::string> s, Client *client){
		client->setMsgBuffer("001 babo :Welcome to the Internet Relay Network babo\r\n");
		send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		std::cout << "wellcome\n";
		client->getMsgBuffer().clear();
	}

};

#endif