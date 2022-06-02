/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swang <swang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:27:02 by swang             #+#    #+#             */
/*   Updated: 2022/06/02 16:28:16 by swang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include "Channel.hpp"

#define PASS 1
#define NICK 2
#define USER 4
#define REGI 8

class Channel;

class Client{
	private :
		std::string _nickName;
		std::string _userName;
		std::string _hostName;
		std::string _serverName;
		std::string _realName;
		int _clientFd;
		unsigned char _regist;
		std::vector<std::string> _myChannelList;
		std::string _msgBuffer;
		std::string _recvBuffer;
	public :
		Client(int clientFd);
		std::string getMsgBuffer();
		bool isRegist();
		void clearMsgBuffer();
		void appendMsgBuffer(std::string msgBuffer);
		std::string &getRecvBuffer();
		void appendRecvBuffer(std::string recvBuffer);
		void clearRecvBuffer();
		int getClientFd();
		void addChannelList(std::string channelName);
		void setNickName(std::string nickName);
		std::string getNickName();
		void setUser(std::string userName, std::string hostName, std::string serverName, std::string realName);
		std::string getUserName();
		std::string getHostName();
		std::string getServerName();
		std::string getRealName();
		void DebugPrint();
		std::vector<std::string> &getMyChannelList();
		std::vector<std::string>::iterator findChannel(std::string item);
		void removeChannelList(std::vector<std::string>::iterator it);
		void removeChannel(std::string serverName);
		void setRegist(int bit);
		unsigned char getRegist();
};



#endif