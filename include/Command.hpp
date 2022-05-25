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

class Server;

// ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
// ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

class Command
{
private:
	bool isLetter(char c);
	bool isNumber(char c);
	bool isSpecial(char c);
	bool isDuplication(std::string s, std::map<int, Client *> clientList);
	bool nickValidate(std::string s);
	Server *_server;
public:
	// Command();
	//Command(Server &server);
	Command(Server *server);

	void nick(std::vector<std::string> s, Client *client);																	   // NICK <parameter>
	void user(std::vector<std::string> s, Client *client);// USER <username> <hostname> <servername> <realname>
	void join(std::vector<std::string> s, Client *client);
	void kick(std::vector<std::string> s);// KICK <channel> <user> [<comment>]//KICK <channel> <user> [<comment>]
	void privmsg(std::vector<std::string> s, Client *client);
	void  personalMessage(std::string msg, std::string senderName, Client * receiver);

	void  channelPersonalMessage(std::string msg, std::string senderName, Client *client, std::string channelName);

	void channelMessage(std::string msg, Client *client, Channel *channel);																   // PRIVMSG <msgtarget> <text to be sent>
	void pass(std::vector<std::string> s); // PASS <password>
	void part(std::vector<std::string> s); // PART <channel> *( "," <channel> ) [ <Part Message> ]
	void quit(std::vector<std::string> s); // QUIT [<Quit message>]

	/*
	:irc.example.com 001 borja :Welcome to the Internet Relay Network borja!borja@polaris.cs.uchicago.edu
	:irc.example.com 433 * borja :Nickname is already in use.
	:irc.example.org 332 borja #cmsc23300 :A channel for CMSC 23300 students

	:<server> 311 <nick> <nick> <user> <host> * :<real name>
	*/

	// void whois(std::vector<std::string> s, Client *client)
	// {
	// 	client->appendMsgBuffer("311 " + client->getNickName() + " " + client->getNickName() + " ~" + client->getUserName() + " " + client->getServerName() + " * " + client->getRealName() + "\r\n");//todo~~

	// 	//ircname, 서버 어딘지, ~()@ Endofwhois
	// 	std::cout << "called" << s[0] << std::endl;
	// };
	void welcome(std::vector<std::string> cmd, Client *client, std::map<int, Client *> clientList);
};

#endif
