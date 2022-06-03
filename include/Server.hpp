/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeonhyun <jeonhyun@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:26:52 by swang             #+#    #+#             */
/*   Updated: 2022/06/03 16:28:10 by jeonhyun         ###   ########.fr       */
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
	int _port;
	int _serverSocketFd;
	int _pollLet;
	int _maxClient;
	int _clientFd;
	std::string _password;
	socklen_t _clientLen;
	sockaddr_in _serverSocketAddr;
	sockaddr_in _clientAddr;
	pollfd _pollClient[OPEN_MAX];
	std::map<std::string, Channel *> _channelList;
	std::map<int, Client *> _clientList;
	Command _command;
	
	int pollingEvent();
	int sock_init();
	void relayEvent();
	void check_cmd(std::vector<std::string> cmd_vec, Client *client);

public:
	Server(int port, std::string password);
	~Server();
	std::map<int, Client *> &getClientList();
	std::map<std::string, Channel *> &getChannelList();
	std::string getPass();
	Client *findClient(int fd);
	Client *findClient(std::string nick);
	Channel *findChannel(std::string name);
	void addChannelList(std::string channelName, int fd);
	void removeUnconnectClient(int fd);
	int execute();
};

#endif
