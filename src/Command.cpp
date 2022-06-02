#include "../include/Server.hpp"
#include "../include/Command.hpp"
#include "../include/Define.hpp"

bool Command::isLetter(char c)
{
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
			return true;
		else
			return false;
};
bool Command::isNumber(char c)
{
    if ('0' <= c && c <= '9')
        return true;
    else
        return false;
};
bool Command::isSpecial(char c)
{
    if (c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}')
        return true;
    else
        return false;
};
bool Command::isDuplication(std::string s, std::map<int, Client *> clientList)
{
    std::map<int, Client *>::iterator it = clientList.begin();
    while (it != clientList.end())
    {
        if (it->second->getNickName() == s)
            return true;
        it++;
    }
    return false;
}
bool Command::nickValidate(std::string s)
{
    if (0 >= s.length() || s.length() > 9)
        return false;
    if (isLetter(s[0]) == false)
        return false;
    for (int i = 1; i < (int)(s.length()); i++)
    {
        if (!isLetter(s[i]) && !isSpecial(s[i]) && !isNumber(s[i]))
            return false;
    }
    return true;
}
// bool Command::channelValidate(std::string s)
// {
// 	//length 200글자 이하, 맨 앞글자는 &나 #으로 시작, ' ', ',', '^G'가 포함되지 않을 것
// 	if (s.length() <= 200 && (s[0] == '&' || s[0] == '#') &&
// 		s.find(' ') == std::string::npos && s.find(',') == std::string::npos && s.find(7) == std::string::npos)
// 		return true;
// 	return false;
// }

Command::Command(Server *server){
    _server = server;
}

void Command::pong(std::vector<std::string> s, Client *client){
    client->appendMsgBuffer("PONG " + s[1] + "\r\n");
}

//ERR_NONICKNAMEGIVEN  431   ???
//ERR_ERRONEUSNICKNAME  432
//ERR_NICKNAMEINUSE    433
//ERR_NICKCOLLISION     434  다른 서버와 닉네임 충돌이므로 지금은 필요 없음
void Command::nick(std::vector<std::string> s, Client *client)
{
	if (s.size() < 2)
	{
		makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "NICK :Not enough parameters");
		return;
	}
    if (isDuplication(s[1], _server->getClientList()))
    {
		// ERR_NICKNAMEINUSE
		makeNumericReply(client->getClientFd(), ERR_NICKNAMEINUSE, s[1] +" :Nickname is already in use");
        return;
    }
    if (!nickValidate(s[1]))
    {
		//ERR_ERRONEUSNICKNAME
		makeNumericReply(client->getClientFd(), ERR_ERRONEUSNICKNAME, s[1] + " :Erroneus nickname");
        return;
    }

	// TODO : _server.clientList 전체에 보내도 된다?
	std::vector<std::string> participatingChannelName = client->getMyChannelList();
	// 닉네임 변경한 클라이언트에게 바뀌었다는 정보 전송
	makeCommandReply(client->getClientFd(), "NICK", s[1]);
	if (participatingChannelName.size() != 0)
	{
		// 참여 중인 모든 채널에 닉네임 변경했다는 정보 전송
		std::vector<std::string>::iterator participatingChannelNameIt = participatingChannelName.begin();
		// fd 중복을 피하기 위해 set 사용
		std::set<int> fdList;
		while (participatingChannelNameIt != participatingChannelName.end())
		{
			Channel *channel = _server->findChannel(*participatingChannelNameIt);
			if (channel == NULL)
				continue;
			// 해당 채널에 참여 중인 클라이언트들에게 닉네임 변경 정보 전송
			std::vector<int> fdsInChannel = channel->getMyClientFdList();
			std::vector<int>::iterator fdsInChannelIt = fdsInChannel.begin();
			while(fdsInChannelIt != fdsInChannel.end())
			{
				if (client->getClientFd() != (*fdsInChannelIt))
				{
					fdList.insert(*fdsInChannelIt);
				}
				fdsInChannelIt++;
			}
			participatingChannelNameIt++;
		}
		std::set<int>::iterator fdsIt = fdList.begin();
		while (fdsIt != fdList.end())
		{
			Client *tmp = _server->findClient(*fdsIt);
			// makeCommandReply(*fdsIt, "NICK", s[1]); :뒤에 닉네임이 tmp 닉네임이 아니어서 이 함수 못 씀
			tmp->appendMsgBuffer(":" + client->getNickName() + " NICK " + s[1] + "\r\n");
			fdsIt++;
		}
		fdList.clear();
	}
	//client->appendMsgBuffer(":" + client->getNickName() + " NICK " + s[1] + "\r\n");
    client->setNickName(s[1]);
};


void Command::join(std::vector<std::string> s, Client *client)
{
	//ERR_NEEDMOREPARAMS	인자 부족할 시 O
	//ERR_BANNEDFROMCHAN	밴 기능 X
	//ERR_INVITEONLYCHAN	초대전용채널 기능 X 
	//ERR_BADCHANNELKEY		채널 비번 기능 X
	//ERR_CHANNELISFULL		채널 인원제한 기능 X
	//ERR_BADCHANMASK		지금은 사용되지 않는 번호라 X
	//ERR_NOSUCHCHANNEL		채널 없는 경우 추가하게 만들어 놔서 X
	//ERR_TOOMANYCHANNELS	채널 가입 제한 개수 X
	//RPL_TOPIC				토픽 기능 X

	//추가한 것
	//- 인자 부족한 경우 예외처리 추가(ERR_NEEDMOREPARAMS)
	//- 채널 이름 유효성 검사하는 channelValidate함수 추가
	//  -> 불가능. 애초에 메시지가 JOIN "#abc defa"이렇게 넣으면 JOIN #abc defa 로 들어온다
	//  -> 파싱 자체가 불가능해서 구현 안해도 될 듯 irssi도 구현 안 되어 있음

	if (s.size() < 2)
	{
		makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
    std::vector<std::string> joinChannel = split(s[1], ",");
    std::vector<std::string>::iterator it = joinChannel.begin();
    while (it != joinChannel.end())
    {
		std::map<std::string, Channel *>::iterator findChannelIt = _server->getChannelList().find(*it);
        if (findChannelIt != _server->getChannelList().end())
        {
            std::string channelName = (*findChannelIt).second->getChannelName();
            (*findChannelIt).second->addMyClientList(client->getClientFd());
            client->addChannelList(channelName);
			allInChannelMsg(client->getClientFd(), channelName, "JOIN", "");
        }
        else
        {
            _server->addChannelList(*it, client->getClientFd());
            _server->findChannel(*it)->addMyClientList(client->getClientFd());
            client->addChannelList(*it);
			allInChannelMsg(client->getClientFd(), *it, "JOIN", "");
        }
        nameListMsg(client->getClientFd(), *it);
        it++;
    }
}

// ERR_NEEDMOREPARAMS 461 "<command> :Not enough parameters"
// ERR_BADCHANMASK  476 "<channel> :Bad Channel Mask"
// ERR_USERNOTINCHANNEL 441 "<nick> <channel> :They aren't on that channel"
// ERR_NOSUCHCHANNEL  403 "<channel name> :No such channel"
// ERR_CHANOPRIVSNEEDED  482 "<channel> :You're not channel operator"
// ERR_NOTONCHANNEL  442 "<channel> :You're not on that channel"
void Command::kick(std::vector<std::string> s, Client *client)
{
	int sLength = s.size();
	if (sLength < 3)
	{
		makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return;
	}
    std::vector<std::string> channelNames = split(s[1], ",");
	std::vector<std::string>::iterator channelNameIt = channelNames.begin();
	while (channelNameIt != channelNames.end())
    {
		// 채널 검색
        Channel *channel = _server->findChannel(*channelNameIt);
		if (channel == NULL)
		{
			makeNumericReply(client->getClientFd(), ERR_NOSUCHCHANNEL, *channelNameIt + " :No such channel");
			channelNameIt++;
			continue;
		}

		std::vector<std::string> kickedUserNickName = split(s[2], ",");
		std::vector<std::string>::iterator kickedUserNickNameIt = kickedUserNickName.begin();
		Client *kickedClient;
		while (kickedUserNickNameIt != kickedUserNickName.end())
		{
			// 서버에 유저가 존재하는지 확인
			kickedClient = _server->findClient(*kickedUserNickNameIt);
			if (kickedClient == NULL)
			{
				//TODO : IRCnet 에서는  : No such nick/channel 이거 뜨는데 어떻게 해야하려나
				makeNumericReply(client->getClientFd(), "401", *kickedUserNickNameIt + " :No such nick/channel");
				// makeNumericReply(client->getClientFd(), "441", *kickedUserNickNameIt + " " + *channelNameIt + " :They aren't on that channel");
				kickedUserNickNameIt++;
				continue;
			}
			else
			{
				// 방장인지 확인
				int operatorFd = channel->getMyOperator();
				if (operatorFd != client->getClientFd())
				{
					makeNumericReply(client->getClientFd(), ERR_CHANOPRIVSNEEDED, *channelNameIt + " :You're not channel operator");
					kickedUserNickNameIt++;
					continue;
				}
				// 해당 채널 안에 클라이언트가 속해 있는지 확인
				if (!channel->checkClientInChannel(kickedClient->getClientFd())){
					makeNumericReply(client->getClientFd(), ERR_USERNOTINCHANNEL, *kickedUserNickNameIt + " " + *channelNameIt + " :They aren't on that channel");
					kickedUserNickNameIt++;
					continue;
				}
				// 해당 채널에서 제거
				std::string msg;
				if (sLength >= 3)
					msg = ":" + client->getNickName() + " KICK " + *channelNameIt + " " + *kickedUserNickNameIt + " " + appendStringColon(3, s) + "\r\n";
				else
					msg = ":" + client->getNickName() + " KICK " + *channelNameIt + " " + *kickedUserNickNameIt + "\r\n";
				client->appendMsgBuffer(msg);
				leaveMessage(msg, client, channel);
				channel->removeClientList(kickedClient->getClientFd());
				kickedClient->removeChannel(*channelNameIt);
				//TODO : 방장이 방장 추방 -> 방장위임
				if (channel->getMyClientFdList().empty() == true)
				{
					_server->getChannelList().erase(channel->getChannelName());
					delete channel;
				}
				else
				{
					channel->setMyOperator(*(channel->getMyClientFdList().begin()));
				}
			}
			kickedUserNickNameIt++;
		}

        channelNameIt++;
    }
}
void Command::privmsg(std::vector<std::string> s, Client *client)
{
	if (s.size() < 2)
	{
		makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return;
	}
	std::vector<std::string> target = split(s[1], ",");
	std::vector<std::string>::iterator targetNameIt = target.begin();
	while (targetNameIt != target.end())
	{
		if((*targetNameIt)[0] == '#')
		{
            if (_server->findChannel(*targetNameIt) == NULL)
            {
				//403 ERR_NOSUCHCHANNEL
				// IRCnet 에서는 401 로 통일된 듯
				makeNumericReply(client->getClientFd(), ERR_NOSUCHCHANNEL, *targetNameIt + " :No such channel");
				//client->appendMsgBuffer("401 " + client->getNickName() + " " + *targetNameIt + " :No such nick/channel\r\n");
                targetNameIt++;
                continue;
            }
            //client->appendMsgBuffer(":" + client->getNickName() + " PRIVMSG " + (*targetNameIt) + " " + appendStringColon(2, s) + "\r\n");
            channelMessage(appendStringColon(2, s), client, _server->findChannel(*targetNameIt));
		}
		else
		{
            if (_server->findClient(*targetNameIt) == NULL)
            {
				// TODO : 401 ERR_NOSUCHNICK
				makeNumericReply(client->getClientFd(), "401", *targetNameIt + " :No such nick/channel");
				//client->appendMsgBuffer("401 " + client->getNickName() + " " + *targetNameIt + " :No such nick/channel\r\n");
                targetNameIt++;
                continue;
            }
            client->appendMsgBuffer(":" + client->getNickName() + " PRIVMSG " + (*targetNameIt) + " " + appendStringColon(2, s) + "\r\n");
			//personalMessage(appendStringColon(2, s), client->getNickName(), _server->findClient(*targetNameIt));
			Client *receiver = _server->findClient(*targetNameIt);
			if (receiver != NULL)
				makePrivMessage(_server->findClient(*targetNameIt), client->getNickName(), receiver->getNickName(), appendStringColon(2, s));
		}
		targetNameIt++;
	}
}

void  Command::makePrivMessage(Client *client, std::string senderName, std::string receiver, std::string msg)
{
	if (client == NULL)
		return ;
	client->appendMsgBuffer(":" + senderName + " PRIVMSG " + receiver + " " + msg + "\r\n");
	std::cout << "fd : " << client->getClientFd() << std::endl;
}

void Command::channelMessage(std::string msg, Client *client, Channel *channel)
{
	std::vector<int> clientsInChannel = channel->getMyClientFdList();
	std::vector<int>::iterator clientsInChannelIt = clientsInChannel.begin();
	while(clientsInChannelIt != clientsInChannel.end())
	{
		if (client->getClientFd() != (*clientsInChannelIt))
			makePrivMessage(_server->findClient(*clientsInChannelIt), client->getNickName(), channel->getChannelName(), msg);
			//channelPersonalMessage(msg, client->getNickName(), _server->findClient(*clientsInChannelIt), channel->getChannelName());
        clientsInChannelIt++;
	}
}
void Command::leaveMessage(std::string msg, Client *client, Channel *channel)
{
	std::vector<int> clientsInChannel = channel->getMyClientFdList();
	std::vector<int>::iterator clientsIt = clientsInChannel.begin();
	while(clientsIt != clientsInChannel.end())
	{
		if (client->getClientFd() != (*clientsIt))
		{
			_server->findClient(*clientsIt)->appendMsgBuffer(msg);
			std::cout << msg;
			std::cout << _server->findClient(*clientsIt)->getNickName() << std::endl;
		}
		clientsIt++;
	}
}



void Command::part(std::vector<std::string> s, Client *client)
{
	//ERR_NEEDMOREPARAMS	인자 부족 O
	//ERR_NOSUCHCHANNEL		서버에 채널 없을 때 O
	//ERR_NOTONCHANNEL		클라이언트가 채널에 없을 때 O
	
	//추가한 것
	//- 인자 부족한 경우 예외처리 추가(ERR_NEEDMOREPARAMS)
	//- PART 메세지 보내는 부분 allInChannelMsg 함수로 교체
	//- 나가려는 채널이 서버에 없는 경우 예외처리 추가(ERR_NOSUCHCHANNEL)
	//- 채널이 서버에 있으나 클라이언트가 가입되어 있지 않은 경우 예외처리 추가(ERR_NOTONCHANNEL)
	if (s.size() < 2)
	{
		makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	std::vector<std::string> partChannel = split(s[1], ",");
	std::vector<std::string>::iterator partChannelIt = partChannel.begin();
	while (partChannelIt != partChannel.end())
    {
		std::vector<std::string>::iterator searchChannelNameIt = client->findChannel(*partChannelIt);
		if (searchChannelNameIt != client->getMyChannelList().end())
        {
			allInChannelMsg(client->getClientFd(), *searchChannelNameIt, "PART", appendStringColon(2, s));
			Channel *tmp = _server->findChannel(*partChannelIt);
            tmp->removeClientList(client->getClientFd());
			client->removeChannelList(searchChannelNameIt);

            if (tmp->getMyClientFdList().empty() == true)
            {
                _server->getChannelList().erase(tmp->getChannelName());
                delete tmp;
            }
            else
            {
                tmp->setMyOperator(*(tmp->getMyClientFdList().begin()));
            }
        }
        else
        {
			if (_server->getChannelList().find(*partChannelIt) == _server->getChannelList().end())
				makeNumericReply(client->getClientFd(), ERR_NOSUCHCHANNEL, *partChannelIt + "No such channel");
			else
				makeNumericReply(client->getClientFd(), ERR_NOTONCHANNEL, *partChannelIt + "You're not on that channel");
        }
        partChannelIt++;
    }
}

void Command::quit(std::vector<std::string> s, Client *client)
{
	//추가한 것
	//주석 삭제
	std::vector<std::string>::iterator channelListInClientClassIt = client->getMyChannelList().begin();
 	while (channelListInClientClassIt != client->getMyChannelList().end())
    {
        Channel *tmp = _server->findChannel(*channelListInClientClassIt);
        tmp->removeClientList(client->getClientFd());
		allInChannelMsg(client->getClientFd(), tmp->getChannelName(), "PART", appendStringColon(1,s));
        if (tmp->getMyClientFdList().empty() == true)
        {
            _server->getChannelList().erase(tmp->getChannelName());
            delete tmp;
        }
        channelListInClientClassIt++;
    }
    _server->getClientList().erase(client->getClientFd());
    close(client->getClientFd());
    delete client;
}



std::string Command::makeFullname(int fd)
{
	// 요청한 fd의 풀네임을 만들어 주는 함수 (임시로 fullname이라 칭함)
	Client *tmp = _server->findClient(fd);
	std::string test = (":" + tmp->getNickName() + "!" + tmp->getUserName() + "@" + tmp->getServerName());
	return (test);
	// return (":" + tmp->getNickName() + "!" + tmp->getUserName() + "@" + tmp->getServerName());//serverName?

};

void Command::welcomeMsg(int fd, std::string flag, std::string msg, std::string name)
{
	Client *tmp = _server->findClient(fd);
	tmp->appendMsgBuffer(flag);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(name);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(msg);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(name);
	tmp->appendMsgBuffer("\r\n");
}

void Command::allInChannelMsg(int target, std::string channelName, std::string command, std::string msg)
{
	Channel *channelPtr = _server->findChannel(channelName);
	std::vector<int> myClientList = channelPtr->getMyClientFdList();
	std::vector<int>::iterator It = myClientList.begin();
	for(; It < myClientList.end(); It++)
	{
		Client *tmp = _server->findClient(*It);
		tmp->appendMsgBuffer(makeFullname(target) + " " + command + " " + channelName + " " + msg + "\r\n");
	}
}

void Command::sendJoinMsg(int joinfd, std::string channelName)
{
	Channel *channelPtr = _server->findChannel(channelName);
	std::vector<int> myClientList = channelPtr->getMyClientFdList();
	std::vector<int>::iterator It = myClientList.begin();
	for(; It < myClientList.end(); It++)
	{
		Client *tmp = _server->findClient(*It);
		tmp->appendMsgBuffer(makeFullname(joinfd) + " JOIN " + channelName + "\r\n");
	}
}

void Command::nameListMsg(int fd, std::string channelName)
{
	Client *tmp = _server->findClient(fd);
	tmp->appendMsgBuffer(RPL_NAMREPLY);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(tmp->getNickName());
	tmp->appendMsgBuffer(" = " + channelName);
	Channel *channelPtr = _server->findChannel(channelName);
	std::vector<int> clientList = channelPtr->getMyClientFdList();
	std::vector<int>::iterator clientListIt = clientList.begin();
	std::string name;
	tmp->appendMsgBuffer(" :");
	for (; clientListIt < clientList.end() - 1; clientListIt++)
	{
		if (channelPtr->getMyOperator() == *clientListIt)
			tmp->appendMsgBuffer("@");
		name = (_server->findClient(*clientListIt))->getNickName();
		tmp->appendMsgBuffer(name);
		tmp->appendMsgBuffer(" ");
	}
	if (channelPtr->getMyOperator() == *clientListIt)
		tmp->appendMsgBuffer("@");
	name = (_server->findClient(*clientListIt))->getNickName();
	tmp->appendMsgBuffer(name);
	tmp->appendMsgBuffer("\r\n");

	// End of NAMES list
	tmp->appendMsgBuffer(RPL_ENDOFNAMES);
	tmp->appendMsgBuffer(" " + tmp->getNickName() +  " " + channelName + " :End of NAMES list" + "\r\n");
}



void Command::makeNumericReply(int fd, std::string flag, std::string str)
{
	Client *tmp = _server->findClient(fd);
	// TODO:: 서버도메인 추가해야함
	tmp->appendMsgBuffer(flag);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(tmp->getNickName());
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(str);
	// str을 만들어서 보내주면 됨
	tmp->appendMsgBuffer("\r\n");
}

void Command::makeCommandReply(int fd, std::string command, std::string str)
{
	Client *tmp = _server->findClient(fd);
	tmp->appendMsgBuffer(":");
	tmp->appendMsgBuffer(tmp->getNickName());
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(command);
	tmp->appendMsgBuffer(" ");
	tmp->appendMsgBuffer(str);
	tmp->appendMsgBuffer("\r\n");
}

void Command::welcome(std::vector<std::string> cmd, Client *client, std::map<int, Client *> clientList)
{
	std::vector<std::string>::iterator cmd_it = cmd.begin();
	while (cmd_it != cmd.end())
	{
		std::vector<std::string> result = split(*cmd_it, " ");
		if (result[0] == "PASS")
		{
			if (client->getRegist() & PASS)
			{
				makeNumericReply(client->getClientFd(), ERR_ALREADYREGISTRED, ":You are already checked Password");
				return ;
			}
			if (result.size() == 1)
			{
				makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, ":Not enough parameters...\nYou are not Input Password");
				return ;
			}
			if (result[1] == _server->getPass())
			{
				client->setRegist(PASS);
			}
			else
            {
				makeNumericReply(client->getClientFd(), ERR_PASSWDMISMATCH, ":Server Password Incorrect");
				_server->removeUnconnectClient(client->getClientFd());
				return ;
            } 
		}
		else if (client->getRegist() & PASS && result[0] == "NICK")
		{
			if (!nickValidate(result[1]))
			{
				makeNumericReply(client->getClientFd(), ERR_ERRONEUSNICKNAME, result[1] + " :Erroneus Nickname");
				return ;
			}
			if (isDuplication(result[1], clientList))
			{
				if (client->getNickName() == result[1]) // 닉중복인데 자기 자신과 중복
					return;
				std::string dup = result[1];
				result[1].append("_");
				makeNumericReply(client->getClientFd(), ERR_NICKNAMEINUSE, ":" + dup + " is already owned");
			}
			client->setNickName(result[1]);
			client->setRegist(NICK);
			makeCommandReply(client->getClientFd(), "NICK", result[1]);
		}
		else if (client->getRegist() & PASS && client->getRegist() & NICK && result[0] == "USER")
		{
			if (result.size() != 5)
			{
				makeNumericReply(client->getClientFd(), ERR_NEEDMOREPARAMS, "USER :Not enough parameters...\n/USER <username> <hostname> <servername> <:realname>");
				return ;
			}
			client->setUser(result[1], result[2], result[3], appendStringColon(4, result));
			client->setRegist(USER);
		}
		else if (result[0] != "CAP")
		{
			// 정상등록과정에서 벗어난 애들은 모두 여기로
			if (!(client->getRegist() & PASS))
			{
				// irssi에서 /connect시 PASS인자를 를 비워두고 connecet 하면 PASS 없이 NICK과 USER가 먼저 온다.
				// /connect 할 때 마다 새 소켓을 연결하므로 PASS 설정에 실패했다면 연결을 끊어버리는 방법으로 결정
				makeNumericReply(client->getClientFd(), ERR_NOTREGISTERED, ":You have not registered Server's Password");
				_server->removeUnconnectClient(client->getClientFd());
			}
			else if (!(client->getRegist() & NICK))
				makeNumericReply(client->getClientFd(), ERR_NOTREGISTERED, ":You have not registered Nickname");
			else if (!(client->getRegist() & USER))
				makeNumericReply(client->getClientFd(), ERR_NOTREGISTERED, ":You have not registered USER info");
			return ;
		}
		cmd_it++;
	}
	// 정상적인 등록 과정을 모두 거쳤다면
	if (client->getRegist() & PASS && client->getRegist() & NICK && client->getRegist() & USER)
    {
		welcomeMsg(client->getClientFd(), RPL_WELCOME, ":Welcome to the Internet Relay Network", client->getNickName());
		client->setRegist(REGI);
    }
}

void	Command::alreadyRegist(Client *client)
{
	makeNumericReply(client->getClientFd(), ERR_ALREADYREGISTRED, ":You are already registed");
}
