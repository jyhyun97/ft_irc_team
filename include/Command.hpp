#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <map>
#include "Channel.hpp"
#include "Client.hpp"
#include "Util.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include "Define.hpp"

class Server;

class Command
{
private:
	bool isLetter(char c);
	bool isNumber(char c);
	bool isSpecial(char c);
	bool isDuplication(std::string s, std::map<int, Client *> clientList);
	bool nickValidate(std::string s);
	Server *_server;
	std::string makeFullname(int fd);
public:
	// Command();
	//Command(Server &server);
	Command(Server *server);

	// 1 swang
	void welcome(std::vector<std::string> cmd, Client *client, std::map<int, Client *> clientList);
	void user(std::vector<std::string> s, Client *client);
	void pass(std::vector<std::string> s, Client *client);
	
	// 2 hyahn
	void privmsg(std::vector<std::string> s, Client *client);
	void nick(std::vector<std::string> s, Client *client);
	void kick(std::vector<std::string> s, Client *client);
	
	// 3 jeonhyun
	void join(std::vector<std::string> s, Client *client);
	void part(std::vector<std::string> s, Client *client);
	void quit(std::vector<std::string> s, Client *client);
	
	// void notice
	void pong(std::vector<std::string> s, Client *client);
	

	void personalMessage(std::string msg, std::string senderName, Client * receiver);
	void channelPersonalMessage(std::string msg, std::string senderName, Client *client, std::string channelName);
	void channelMessage(std::string msg, Client *client, Channel *channel);   // PRIVMSG <msgtarget> <text to be sent>
	void leaveMessage(std::string msg, Client *client, Channel *channel);
	//todo : private
	
	

	// void whois(std::vector<std::string> s, Client *client)
	// {
	// 	client->appendMsgBuffer("311 " + client->getNickName() + " " + client->getNickName() + " ~" + client->getUserName() + " " + client->getServerName() + " * " + client->getRealName() + "\r\n");//todo~~

	// 	//ircname, 서버 어딘지, ~()@ Endofwhois
	// 	std::cout << "called" << s[0] << std::endl;
	// };


	void makeNumericReply(int fd, std::string flag, std::string str);
	void makeCommandReply(int fd, std::string command, std::string str);
	void welcomeMsg(int fd, std::string flag, std::string msg, std::string name);
	void sendJoinMsg(int joinfd, std::string channelName);
	void allInChannelMsg(int target, std::string channelName, std::string command, std::string msg);
	void nameListMsg(int fd, std::string channelName);
};

#endif
