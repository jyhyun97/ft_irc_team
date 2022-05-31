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

void Command::nick(std::vector<std::string> s, Client *client)
{
    if (isDuplication(s[1], _server->getClientList()))
    {
		makeNumericReply(client->getClientFd(), "433", s[1]);
        return;
    }
    if (!nickValidate(s[1]))
    {
        // TODO : 응답 보내주기
        std::cout << "s1 : " << s[1] << "\n" << "닉네임규칙안맞음\n";
        return;
    }
	// TODO : 클라이언트가 접속한 모든 채널에 닉네임 변경을 알려야함
    // makeCommandReply
	client->appendMsgBuffer(":" + client->getNickName() + " NICK " + s[1] + "\r\n");
    client->setNickName(s[1]);
};

void Command::user(std::vector<std::string> s, Client *client)
{
    client->setUser(client->getNickName(), s[2], s[3], appendStringColon(4, s));
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

void Command::kick(std::vector<std::string> s, Client *client)
{
    std::vector<std::string> channelNames = split(s[1], ",");
	std::vector<std::string>::iterator channelNameIt = channelNames.begin();
	while (channelNameIt != channelNames.end())
    {
        Channel *channel = _server->findChannel(*channelNameIt);
        int operatorFd = channel->getMyOperator();
        if (operatorFd != client->getClientFd())
        {
            //TODO : #42seoul You're not channel operator
            channelNameIt++;
            continue;
        }
        // TODO : 유저도 list 돌기 ex) aa,bb,cc...
        Client *kickedClient = _server->findClient(s[2]);
        std::vector<int>::iterator it = channel->findClient(kickedClient->getClientFd());
        if (it == channel->getMyClientFdList().end()){
            channelNameIt++;
            continue;
        }
        std::string msg = ":" + client->getNickName() + " KICK " + *channelNameIt + " " + s[2] + " " + appendStringColon(3, s) + "\r\n";
        client->appendMsgBuffer(msg);
        leaveMessage(msg, client, channel);
        channel->removeClientList(kickedClient->getClientFd());
        //TODO : 방장이 방장 추방 -> 방장위임
        channelNameIt++;
    }
}
void Command::privmsg(std::vector<std::string> s, Client *client)
{
	std::vector<std::string> target = split(s[1], ",");
	std::vector<std::string>::iterator targetIt = target.begin();
	while (targetIt != target.end())
	{
		if((*targetIt)[0] == '#')
		{
            if (_server->findChannel(*targetIt) == NULL)
            {
                std::cout << "153" << std::endl;
                targetIt++;
                continue;
            }
            //client->appendMsgBuffer(":" + client->getNickName() + " PRIVMSG " + (*targetIt) + " " + appendStringColon(2, s) + "\r\n");
            channelMessage(appendStringColon(2, s), client, _server->findChannel(*targetIt));
		}
		else
		{
            if (_server->findChannel(*targetIt) == NULL)
            {
                std::cout << "163" << std::endl;
                targetIt++;
                continue;
            }
            client->appendMsgBuffer(":" + client->getNickName() + " PRIVMSG " + (*targetIt) + " " + appendStringColon(2, s) + "\r\n");
			personalMessage(appendStringColon(2, s), client->getNickName(), _server->findClient(*targetIt));
		}
		targetIt++;
	}
}


void  Command::personalMessage(std::string msg, std::string senderName, Client * receiver)
{
	receiver->appendMsgBuffer(":" + senderName + " PRIVMSG " + receiver->getNickName() + " " + msg + "\r\n");
}

void  Command::channelPersonalMessage(std::string msg, std::string senderName, Client *client, std::string channelName)
{
	client->appendMsgBuffer(":" + senderName + " PRIVMSG " + channelName + " " + msg + "\r\n");
	std::cout << "fd : " << client->getClientFd() << std::endl;
}

void Command::channelMessage(std::string msg, Client *client, Channel *channel)
{
	std::vector<int> clients = channel->getMyClientFdList();
	std::vector<int>::iterator clientsIt = clients.begin();
	while(clientsIt != clients.end())
	{
		if (client->getClientFd() != (*clientsIt))
			channelPersonalMessage(msg, client->getNickName(), _server->findClient(*clientsIt), channel->getChannelName());
        clientsIt++;
	}
}
void Command::leaveMessage(std::string msg, Client *client, Channel *channel)
{
	std::vector<int> clients = channel->getMyClientFdList();
	std::vector<int>::iterator clientsIt = clients.begin();
	while(clientsIt != clients.end())
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


void Command::pass(std::vector<std::string> s, Client *client)
{
    if (s[1] != _server->getPass())
    {
        client->appendMsgBuffer("464 " + client->getNickName() + " :Password incorrect\r\n");
        return;
    }
} // PASS <password>

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

void Command::welcome(std::vector<std::string> cmd, Client *client, std::map<int, Client *> clientList)
{
	//닉 체크하고 최초 닉네임 설정..
	//TODO : clientList 길이 체크하고 3개 명령어 다 처리한 후 welcome 전송
	//유저 설정
	std::vector<std::string>::iterator cmd_it = cmd.begin();
	while (cmd_it != cmd.end())
	{
		std::vector<std::string> result = split(*cmd_it, " ");
		if (result[0] == "PASS") //패스워드를 아예 안넣을 경우 pass 명령어를 타지 않음
		{
            if (result[1] != _server->getPass())
            {
                std::cout << "비번안맞음" << std::endl;
            	client->appendMsgBuffer(ERR_PASSWDMISMATCH);
            	client->appendMsgBuffer(" " + client->getNickName() + " :Password incorrect\r\n");
                
                // TODO : 위에 password 틀렸다는 메세지는 전송 안해도 괜찮을까?
                _server->getClientList().erase(client->getClientFd());
                close(client->getClientFd());
				return ;
            }
		}
		else if (result[0] == "NICK")
		{
			if (!nickValidate(result[1]))
			{
				std::cout << "닉네임규칙안맞음\n";
				// client->appendMsgBuffer(": 432 test :nick rule\r\n");
			}
			if (isDuplication(result[1], clientList))
			{
				std::cout << "nick dup\n";
				result[1].append("_");
				// client->appendMsgBuffer(": 433 test :nick dup\r\n");

                // 433     ERR_NICKNAMEINUSE
                // "<nick> :Nickname is already in use"
            }
			std::cout << "nick name check: " << result[1] << std::endl;
			client->setNickName(result[1]);
		}
		else if (result[0] == "USER")
		{
			user(result, client);
		}
		cmd_it++;
	}
	// print_clientList(clientList);
	if (client->getNickName().empty() == false && client->getUserName().empty() == false/* && PASS 여부*/)
    {
    	welcomeMsg(client->getClientFd(), RPL_WELCOME, ":Welcome to the Internet Relay Network", client->getNickName());
    }
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
	tmp->appendMsgBuffer(" :");
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