#include "../include/Server.hpp"
#include "../include/Command.hpp"

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
    // clientList 돌면서 s랑 똑같은 게 있는 지 찾아서 t/f 반환
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

Command::Command(Server *server){
    _server = server;
}

void Command::nick(std::vector<std::string> s, Client *client)
{
    if (isDuplication(s[1], _server->getClientList()))
    {
        std::cout << "nick dup in nick command\n";
        // 변경시 중복이면 중복프로토콜메시지(닉변경실패)
        return;
    }
    if (!nickValidate(s[1]))
    {
        std::cout << "s1 : " << s[1] << "\n"
                  << "닉네임규칙안맞음\n";
        return;
    }

    client->appendMsgBuffer(":" + client->getNickName() + " NICK " + s[1] + "\r\n");
    client->setNickName(s[1]);
    //중복 체크 ->응답
    //닉네임 규칙 체크 ->응답

    //맞으면 닉네임 바꾸고 응답 보내기
}; // NICK <parameter>
void Command::user(std::vector<std::string> s, Client *client)
{
    client->setUser(client->getNickName(), s[2], s[3], appendStringColon(4, s));
	// ㄴ> 닉네임 중복시 nick 파트에서 _를 붙이지만 /USER를 통해 자동으로 들어오는 문자열엔 반영이 되어있지 않음
    std::cout << "user called" << std::endl;

}; // USER <username> <hostname> <servername> <realname>
void Command::join(std::vector<std::string> s, Client *client)
{
    // JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
    // [JOIN] [#foo,#bar]
    std::vector<std::string> joinChannel = split(s[1], ",");
    std::vector<std::string>::iterator it = joinChannel.begin();
    while (it != joinChannel.end())
    {
        //클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신//join

        // std::vector<Channel *>::iterator channelBegin = channelList.begin();
        // std::vector<Channel *>::iterator channelEnd = channelList.end();
        // std::vector<Channel *>::iterator findIter = findChannel(channelBegin, channelEnd, *it);

        std::map<std::string, Channel *>::iterator findChannelIt = _server->getChannelList().find(*it);

        if (findChannelIt != _server->getChannelList().end())
        {
            // 채널 객체가 이미 존재하는 경우
            std::string channelName = (*findChannelIt).second->getChannelName();
            (*findChannelIt).second->addMyClientList(client->getClientFd());
            client->addChannelList(channelName);
			sendJoinMsg(client->getClientFd(), channelName);
            std::cout << "채널에 추가됨\n";
        }
        else
        {
            // 새 채널 만들기; 서버.채널리스트에(( 추가))
            _server->addChannelList(*it, client->getClientFd());
            // 위에서 생성된 채널 클래스 객체에 유저 추가
            _server->findChannel(*it)->addMyClientList(client->getClientFd());
            // 클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신
            client->addChannelList(*it);
			sendJoinMsg(client->getClientFd(), *it);
        }
        // TODO : 닉네임 제대로 넣기
        nameListMsg(client->getClientFd(), *it);
        it++;
    // 353 newmember = #채널 :@방장 유저 유저 유저
    }
    //채널의 클라이언트 리스트 돌면서 다 메세지 보내기
    // print_channelList(channelList);

    // //connect 127.0.0.1 6667 0000
    //파싱해서 채널 이름 따오고
    //서버.채널리스트에 없으면 채널 객체 만들어서 채널 리스트에 추가
    //클라이언트.채널리스트 갱신, 채널.클라이언트리스트 갱신
}

//KICK #Finnish John :Speaking English
//:WiZ KICK #Finnish John
//:방장 kick #채널 강퇴유저
void Command::kick(std::vector<std::string> s, Client *client)
{
    //client가 방장인 지 확인
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
        // 인자로 받은 채널에 kick 당하는 유저가 존재하는지 확인
        // s[2] 유저정보 split 해야 할까?
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
        //그 채널에 참여한 모든 유저에게 메시지 보내기
        //채널에서 해당 유저 삭제
        
        //TODO : 방장이 방장 추방 -> 방장위임
        
        channelNameIt++;
    }


		// 떠날채널목록을 다 돌면서
	// 	std::vector<std::string>::iterator searchChannelNameIt = client->findChannel(*channelNameIt);
	// 	if (searchChannelNameIt != client->getMyChannelList().end()) // 클라이언트가 접속한 채널이 맞으면
    //     {
    
    //     }
    //     channelNameIt++;
    // }
}
// KICK <channel> <user> [<comment>]//KICK <channel> <user> [<comment>]
void Command::privmsg(std::vector<std::string> s, Client *client)
{
	// privmsg <target> :text
	std::vector<std::string> target = split(s[1], ",");
	std::vector<std::string>::iterator targetIt = target.begin();
	while (targetIt != target.end())
	{
		if((*targetIt)[0] == '#')
		{

			// std::vector<Channel *>::iterator channelIt = findChannel(channelList.begin(), channelList.end(), *targetIt);
			//TODO :find 실패시 예외처리
            // TODO : 방장은 닉네임 앞에 @ 붙여서 보내기
            channelMessage(appendStringColon(2, s), client, _server->findChannel(*targetIt));
            // std::string msg = ":" + client->getNickName() + " PRIVMSG " + _server->findChannel(*targetIt)->getChannelName() + " " + appendStringColon(2, s) + "\r\n" ;
			// channelMessage(msg, client, _server->findChannel(*targetIt));
		}
		else
		{
			// std::vector<Client *>::iterator clientIt = findClient(clientList.begin(), clientList.end(), *targetIt);
			//TODO :find 실패시 예외처리
			personalMessage(appendStringColon(2, s), client->getNickName(), _server->findClient(*targetIt));
		}
		targetIt++;
	}
	std::cout << "i am privmsg\n";
}


void  Command::personalMessage(std::string msg, std::string senderName, Client * receiver)
{
	receiver->appendMsgBuffer(":" + senderName + " PRIVMSG " + receiver->getNickName() + " " + msg + "\r\n");
	//std::cout << "fd : " << receiver->getClientFd() << std::endl;
}

void  Command::channelPersonalMessage(std::string msg, std::string senderName, Client *client, std::string channelName)
{
	client->appendMsgBuffer(":" + senderName + " PRIVMSG " + channelName + " " + msg + "\r\n");
	std::cout << "fd : " << client->getClientFd() << std::endl;
}

void Command::channelMessage(std::string msg, Client *client, Channel *channel)
{
    //if (client->getClientFd() == channel->getMyOperator())
    //    client->getNickName().insert(0, "@");
	std::vector<int> clients = channel->getMyClientFdList();
	std::vector<int>::iterator clientsIt = clients.begin();
	while(clientsIt != clients.end())
	{
		if (client->getClientFd() != (*clientsIt))
			channelPersonalMessage(msg, client->getNickName(), _server->findClient(*clientsIt), channel->getChannelName());
        clientsIt++;
	}
}
//:syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch
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
        std::cout << "비번안맞음" << std::endl;
        client->appendMsgBuffer("464 " + client->getNickName() + " :Password incorrect\r\n");

        return;
    }
    std::cout << "called" << s[0] << std::endl;
} // PASS <password>

void Command::part(std::vector<std::string> s, Client *client)
{
    //[PART] [#aa,#bb]
	//if (아무 인자도 입력하지 않음)
		// 에러 메세지 전송 처리
	std::vector<std::string> partChannel = split(s[1], ",");
	std::vector<std::string>::iterator partChannelIt = partChannel.begin();
	while (partChannelIt != partChannel.end())
    {
	
		// 떠날채널목록을 다 돌면서
		std::vector<std::string>::iterator searchChannelNameIt = client->findChannel(*partChannelIt);
		if (searchChannelNameIt != client->getMyChannelList().end()) // 클라이언트가 접속한 채널이 맞으면
        {
            
            std::cout << "part channel name: " << *searchChannelNameIt << std::endl;
			Channel *tmp = _server->findChannel(*partChannelIt);
            std::cout << "my operator " << tmp->getMyOperator() << std::endl;
            tmp->removeClientList(client->getClientFd());
            //채널에서도 클라이언트 지우기
			// :떠난사람!떠난사람@도메인 part 채널이름 :메세지
			client->appendMsgBuffer(":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getServerName() + " PART " + *searchChannelNameIt + "\r\n");
            std::string msg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getServerName() + " PART " + *searchChannelNameIt + "\r\n";
            leaveMessage(msg, client, tmp);
			client->removeChannelList(searchChannelNameIt);
            
            //TODO : 채널 떠났다고 채널에 있는 사람들에게 알려주기 left
            //TODO : 만약 채널에 더이상 사람이 없으면 if (channel.size() == 0) 서버에서 채널 삭제하기..
            
            // TODO : channel 에 아무도 없으면 채널도 삭제 
            if (tmp->getMyClientFdList().empty() == true)
            {
                _server->getChannelList().erase(tmp->getChannelName());
            }
            else
            {
                tmp->setMyOperator(*(tmp->getMyClientFdList().begin()));
             std::cout << "my operator change to " << tmp->getMyOperator() << std::endl;

            }
        }
        else //없으면 에러메세지
        {
			// if (아예 없는 채널을 떠나려고함)
			// else if (서버에 있는데 참여하지 않은 채널을 떠나려고함)
        }
        partChannelIt++;
    }
} // PART <channel> *( "," <channel> ) [ <Part Message> ]

void Command::quit(std::vector<std::string> s, Client *client)
{
	//소켓지우고
	// 나가려는 client 가 참여한 채널 리스트 이터레이터
	std::vector<std::string>::iterator channelListInClientClassIt = client->getMyChannelList().begin();
 	while (channelListInClientClassIt != client->getMyChannelList().end())
    {
        Channel *tmp = _server->findChannel(*channelListInClientClassIt);
        // channel class -> myclientList 에서 삭제
        tmp->removeClientList(client->getClientFd());
        std::string msg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getServerName() + " PART " + tmp->getChannelName() + " " + appendStringColon(1, s) + "\r\n";
        // 채널 내 자신을 제외한 사람들에게 메세지 전송
		//std::cout << "leave msg check " << msg <<std::endl;
        leaveMessage(msg, client, tmp);
        std::cout << "254 channel name "<< tmp->getChannelName() << std::endl;
        // TODO : channel 에 아무도 없으면 채널도 삭제 
        if (tmp->getMyClientFdList().empty() == true)
        {
            _server->getChannelList().erase(tmp->getChannelName());
        }
        channelListInClientClassIt++;
    }
    //_server.clientList 에서 지우기
    _server->getClientList().erase(client->getClientFd());
    close(client->getClientFd());
    //TODO : 현재 클라이언트 소켓 삭제
} // QUIT [<Quit message>]
 
//:syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch
//: 닉넴 ! 유저네임 @ 서버네임 QUIT : 메세지
//

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
                //객체 삭제...
                //소켓 삭제...
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
	return (":" + tmp->getNickName() + "!" + tmp->getUserName() + "@" + tmp->getHostName());
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

// void Command::makeReply(int fd, std::string flag, std::string msg, std::string name)
// {
// 	요청한 fd의 클라이언트에 메세지를 조합해서 저장하는 함수, 모듈화 시도했으나 잘 안됨
// 	Client *tmp = _server->findClient(fd);
// 	tmp->appendMsgBuffer(flag);
// 	tmp->appendMsgBuffer(" ");
// 	tmp->appendMsgBuffer(name);
// 	tmp->appendMsgBuffer(" ");
// 	tmp->appendMsgBuffer(msg);
// 	tmp->appendMsgBuffer("\r\n");
// }
