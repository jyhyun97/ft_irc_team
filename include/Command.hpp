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
#include <string>


class Command
{
    public :
    void nick(std::vector<std::string> s){std::cout << "i am nick\n";};//NICK <parameter>
    void user(std::vector<std::string> s){std::cout << "i am user\n";};//USER <username> <hostname> <servername> <realname>
    void join(std::vector<std::string> s){std::cout << "i am join\n";};//JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
	void kick(std::vector<std::string> s){std::cout << "i am kick\n";};//KICK <channel> <user> [<comment>]//KICK <channel> <user> [<comment>]
	void privmsg(std::vector<std::string> s){std::cout << "i am privmsg\n";};//PRIVMSG <msgtarget> <text to be sent>
	void pass(std::vector<std::string> s){std::cout << "i am pass\n";};//PASS <password>
	void part(std::vector<std::string> s){std::cout << "i am part\n";};//PART <channel> *( "," <channel> ) [ <Part Message> ]
	void quit(std::vector<std::string> s){std::cout << "i am quitt\n";};//QUIT [<Quit message>]
	void help(std::vector<std::string> s){std::cout << "i am help\n";};//HELP
	void whois(std::vector<std::string> s, Client client){
		client.setMsgBuffer("311 jeonhyun jeonhyun 127.0.0.1 * :현정연\r\n");
		send(client.getClientFd(), client.getMsgBuffer().c_str(), client.getMsgBuffer().length(), 0);
		client.getMsgBuffer().clear();
		std::cout << "i am whois\n";
	};
	void welcome(std::vector<std::string> s, Client client){
		client.setMsgBuffer("001 babo :Welcome to the Internet Relay Network babo\r\n");
		send(client.getClientFd(), client.getMsgBuffer().c_str(), client.getMsgBuffer().length(), 0);
		client.getMsgBuffer().clear();
	}

};

#endif