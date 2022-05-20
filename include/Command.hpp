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

//TODO : 함수 이름 바꾸기, msg target split 하는 함수
std::vector<std::string> split_command(std::string &line, char c)
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

void print_channel(std::vector<Channel *> v)
{
	std::vector<Channel *>::iterator it = v.begin();
	std::cout << "channel begin: " << (*(v.begin()))->getChannelName() << std::endl;
	while (it != v.end())
	{
		std::cout << "print channel: " << (*it)->getChannelName() << std::endl;
		it++;
	}
}
void print_client(std::vector<Client *> v)
{
	std::vector<Client *>::iterator it = v.begin();
	while (it != v.end())
	{
		std::cout << "print Client: " << (*it)->getNickName() << " client fd : " << (*it)->getClientFd() << std::endl;
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
// std::vector<Channel *>::iterator find(std::vector<Channel *> search, std::string item)
// {
// 	std::vector<Channel *>::iterator it = search.begin();
// 	while (it != search.end())
// 	{
// 		if ((*it)->getChannelName() == item)
// 			return (it);
// 		it++;
// 	}
// 	return (it);
// }

std::vector<Channel *>::iterator findChannel(std::vector<Channel *>::iterator begin, std::vector<Channel *>::iterator end, std::string item)
{
	std::vector<Channel *>::iterator it = begin;
	while (it != end)
	{
		if ((*it)->getChannelName() == item)
			return (it);
		it++;
	}
	return (end);
}

std::vector<Client *>::iterator findClient(std::vector<Client *>::iterator begin, std::vector<Client *>::iterator end, std::string item)
{
	std::vector<Client *>::iterator it = begin;
	while (it != end)
	{
		if ((*it)->getNickName() == item)
			return (it);
		it++;
	}
	return (end);
}
// ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
// ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

class Command
{
private:
	bool isLetter(char c)
	{
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
			return true;
		else
			return false;
	};
	bool isNumber(char c)
	{
		if ('0' <= c && c <= '9')
			return true;
		else
			return false;
	};
	bool isSpecial(char c)
	{
		if (c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}')
			return true;
		else
			return false;
	};
	bool isDuplication(std::string s, std::vector<Client *> &clientList)
	{
		// clientList 돌면서 s랑 똑같은 게 있는 지 찾아서 t/f 반환
		for (int i = 0; i < clientList.size(); i++)
		{
			if (clientList[i]->getNickName() == s)
				return true;
		}
		return false;
	}
	/// connect 127.0.0.1 6667 0000
	bool nickValidate(std::string s)
	{
		if (0 >= s.length() || s.length() > 9)
			return false;
		if (isLetter(s[0]) == false)
			return false;
		for (int i = 1; i < s.length(); i++)
		{
			if (!isLetter(s[i]) && !isSpecial(s[i]) && !isNumber(s[i]))
				return false;
		}
		return true;
	}

public:
	void nick(std::vector<std::string> s, std::vector<Client *> &clientList, Client *client)
	{

		if (isDuplication(s[1], clientList))
		{
			std::cout << "nick dup\n";
			// 변경시 중복이면 중복프로토콜메시지
			// 중복이면 마지막에 '_' 추가하기()->웰컴단계에서
			return;
		}
		if (!nickValidate(s[1]))
		{
			std::cout << "닉네임규칙안맞음\n";
			return;
		}
		
		client->setMsgBuffer(":" + client->getNickName() + " NICK " + s[1] + "\r\n");
		client->setNickName(s[1]);
		//중복 체크 ->응답
		//닉네임 규칙 체크 ->응답

		//맞으면 닉네임 바꾸고 응답 보내기
		
		std::cout << "i am nick\n";
		print_client(clientList);

	};																	   // NICK <parameter>
	void user(std::vector<std::string> s) { std::cout << "i am user\n"; }; // USER <username> <hostname> <servername> <realname>
	void join(std::vector<std::string> s, Client *client, std::vector<Channel *> &channelList)
	{
		// JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
		// JOIN #foo,#bar
		std::vector<std::string> joinChannel = cmd_split(s[1], ',');
		std::vector<std::string>::iterator it = joinChannel.begin();
		while (it != joinChannel.end())
		{
			//클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신//join

			std::vector<Channel *>::iterator channelBegin = channelList.begin();
			std::vector<Channel *>::iterator channelEnd = channelList.end();
			std::vector<Channel *>::iterator findIter = findChannel(channelBegin, channelEnd, *it);

			if (findIter != channelList.end())
			{
				// 채널 객체가 이미 존재하는 경우
				(*findIter)->addMyClientList(client);
				client->addChannelList(*findIter);
				client->setMsgBuffer(":" + client->getNickName() + " JOIN #seoul" + "\r\n");
				//TODO : #seoul 말고 getchannelName헀을 때 segfault 나중에 해결하기
				//:이름 JOIN 채널명
			}
			else
			{
				// 새 채널 만들기; 서버.채널리스트에(( 추가))
				channelList.push_back(new Channel(*it));
				// 위에서 생성된 채널 클래스 객체에 유저 추가
				channelList.back()->addMyClientList(client);
				// 클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신
				client->addChannelList(channelList.back());
				client->setMsgBuffer(":" + client->getNickName() + " JOIN #seoul" + "\r\n");
			}
			it++;
		}
		//채널의 클라이언트 리스트 돌면서 다 메세지 보내기
		

		print_channel(channelList);

		// //connect 127.0.0.1 6667 0000
		//파싱해서 채널 이름 따오고
		//서버.채널리스트에 없으면 채널 객체 만들어서 채널 리스트에 추가
		//클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신

		std::cout << "i am join\n";
	};
	void kick(std::vector<std::string> s) { std::cout << "i am kick\n"; }; // KICK <channel> <user> [<comment>]//KICK <channel> <user> [<comment>]
	void privmsg(std::vector<std::string> s, Client *client, std::vector<Client *> clientList, std::vector<Channel *> channelList)
	{
		// privmsg <target> :text
		std::vector<std::string> target = split_command(s[1], ',');
		//TODO : 스플릿 변경
		std::vector<std::string>::iterator targetIt = target.begin();
		while (targetIt != target.end())
		{
			if((*targetIt)[0] == '#')
			{
				std::vector<Channel *>::iterator channelIt = findChannel(channelList.begin(), channelList.end(), *targetIt);
				//TODO :find 실패시 예외처리
				channelMessage(s[2], client, *channelIt);
			}
			else
			{
				std::vector<Client *>::iterator clientIt = findClient(clientList.begin(), clientList.end(), *targetIt);
				//TODO :find 실패시 예외처리
				personalMessage(s[2], client->getNickName(), *clientIt);
			}
			targetIt++;
		}
		// std::stringstream ss;
		// ss << client->getClientFd();
		// std::string str = ss.str();

		// client->setMsgBuffer("001 babo :42424242 [" + str + "]\r\n");
		// send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		// client->getMsgBuffer().clear();

		std::cout << "i am privmsg\n";
	};
	void  personalMessage(std::string msg, std::string senderName, Client * receiver)
	{
		// receiver->setMsgBuffer("001 " + senderName + " " + msg + "\r\n");
		// send(receiver->getClientFd(), receiver->getMsgBuffer().c_str(), receiver->getMsgBuffer().length(), 0);
		// receiver->getMsgBuffer().clear();
		// receiver->setMsgBuffer("001 PRIVMSG " + senderName + " " + msg + "\r\n");
		
		receiver->setMsgBuffer(":" + senderName + " PRIVMSG " + receiver->getNickName() + " " + msg + "\r\n");
		std::cout << "fd : " << receiver->getClientFd() << std::endl;
	};

	void  channelPersonalMessage(std::string msg, std::string senderName, Client *client, std::string channelName)
	{
		// receiver->setMsgBuffer("001 " + senderName + " " + msg + "\r\n");
		// send(receiver->getClientFd(), receiver->getMsgBuffer().c_str(), receiver->getMsgBuffer().length(), 0);
		// receiver->getMsgBuffer().clear();
		// receiver->setMsgBuffer("001 PRIVMSG " + senderName + " " + msg + "\r\n");
		
		client->setMsgBuffer(":" + senderName + " PRIVMSG " + channelName + " " + msg + "\r\n");
		std::cout << "fd : " << client->getClientFd() << std::endl;
	};

	void channelMessage(std::string msg, Client *client, Channel * channel)
	{
		std::vector<Client *> clients = channel->getMyClientList();
		std::vector<Client *>::iterator clientsIt = clients.begin();
		while(clientsIt != clients.end())
		{
			if (client->getClientFd() != (*clientsIt)->getClientFd())
				channelPersonalMessage(msg, client->getNickName(), *clientsIt, channel->getChannelName());
			clientsIt++;
		}
	};																   // PRIVMSG <msgtarget> <text to be sent>
	void pass(std::vector<std::string> s) { std::cout << "i am pass\n"; }; // PASS <password>
	void part(std::vector<std::string> s) { std::cout << "i am part\n"; }; // PART <channel> *( "," <channel> ) [ <Part Message> ]
	void quit(std::vector<std::string> s)
	{
		//소켓지우고
		//리스트에서 지우고
		std::cout << "i am quitt\n";
	}; // QUIT [<Quit message>]
	void whois(std::vector<std::string> s, Client *client)
	{
		client->setMsgBuffer("311 jeonhyun jeonhyun 127.0.0.1 * :현정연\r\n");
		//send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		//client->getMsgBuffer().clear();
		std::cout << "i am whois\n";
	};
	void welcome(std::vector<std::string> cmd, Client *client, std::vector<Client *> clientList)
	{
		//닉 체크하고 최초 닉네임 설정..
		//유저 설정
		std::vector<std::string>::iterator cmd_it = cmd.begin();
		while (cmd_it != cmd.end())
		{
			std::vector<std::string> result = split_command(*cmd_it, ' ');
			if (result[0] == "PASS")
			{
				std::cout << "cap pass\n";
			}
			else if (result[0] == "NICK")
			{
				if (!nickValidate(result[1]))
				{
					std::cout << "닉네임규칙안맞음\n";
					return;
				}
				if (isDuplication(result[1], clientList))
				{
					std::cout << "nick dup\n";
					// 중복이면 마지막에 '_' 추가하기()->웰컴단계에서
					return;
				}
				client->setNickName(result[1]);
			}
			else if (result[0] == "USER")
			{

			}
			cmd_it++;
		}
		client->setMsgBuffer("001 " + client->getNickName() + " :Welcome to the Internet Relay Network " + client->getNickName() + "\r\n");
		// send(client->getClientFd(), client->getMsgBuffer().c_str(), client->getMsgBuffer().length(), 0);
		std::cout << "welcome\n";
		// client->getMsgBuffer().clear();
	}
};

#endif
