/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swang <swang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:26:52 by swang             #+#    #+#             */
/*   Updated: 2022/06/02 16:31:45 by swang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <map>
#include "Util.hpp"

# define C_NRML "\033[0m"
# define C_RED  "\033[31m"
# define C_GREN "\033[32m" 
# define C_YLLW "\033[33m" 
# define C_BLUE "\033[34m" 



class Channel;
class Client;
class Command;

class Server
{
private:
	std::map<int, Client *> _clientList;
	std::map<std::string, Channel *> _channelList;
	int _port;
	std::string _password;
	int _serverSocketFd;
	sockaddr_in _serverSocketAddr;
	Command _command;
	pollfd _pollClient[OPEN_MAX];
	int _pollLet;
	int _maxClient;
	socklen_t _clientLen;
	int _clientFd;
	sockaddr_in _clientAddr;
	int pollingEvent();
	std::vector<std::pair<int, std::string> > parsing();

public:
	Server(int port, std::string password);
	~Server();
	std::map<int, Client *> &getClientList();
	std::map<std::string, Channel *> &getChannelList();
	Client *findClient(int fd);
	Client *findClient(std::string nick);
	Channel *findChannel(std::string name);
	std::string getPass();
	int sock_init();
	void check_cmd(std::vector<std::string> cmd_vec, Client *client);
	void addChannelList(std::string channelName, int fd);
	void relayEvent();
	int execute();
	void removeUnconnectClient(int fd);
};

#endif
