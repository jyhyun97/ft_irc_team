/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeonhyun <jeonhyun@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 16:27:05 by swang             #+#    #+#             */
/*   Updated: 2022/06/03 14:01:09 by jeonhyun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include "Client.hpp"


class Client;

class Channel{

private :
	int _operator;
	std::string _channelName;
    std::vector<int> _myClientFdList;
	int checkChannelName();
public :
	Channel(std::string channelName, int fd);
	~Channel();
	int getMyOperator();
	void setMyOperator(int fd);
	std::string getChannelName();
	std::vector<int> getMyClientFdList();
	Client getClient();
	void setClient();
	int leaveEvent(Client &client);
	void addMyClientList(int fd);
	void removeClientList(int fd);
	std::vector<int>::iterator findMyClientIt(int fd);
	bool checkClientInChannel(int fd);
};

#endif
