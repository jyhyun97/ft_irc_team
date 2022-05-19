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

class Channel;
class Client;
// class Command;

std::vector<std::string> CrlfSplit(std::string &line, std::string s)
{
	std::vector<std::string> tab;
	std::string cmd_buf;
	size_t start = 0;
	size_t pos;

	while ((pos = line.find(s)) != std::string::npos){
		tab.push_back(line.substr(start, pos));
		line = line.substr(pos + 2);
	}
	for (size_t i = 0; i < tab.size(); i++)
	{
		std::cout << "[[" <<  tab[i] << "]]" << std::endl;
	}
	return tab;
}

std::vector<std::string> split(std::string &line, char c)
{
	std::vector<std::string> tab;
	std::string word_buf;
	bool space = false;

	for (int i = 0; i < (int)line.size(); i++)
	{
		if (line[i] == c && space == false)
		{
			space = true;
			if (word_buf.size() > 0)
				tab.push_back(word_buf);
			word_buf.clear();
		}
		else
		{
			word_buf += line[i];
			if (line[i] != c)
				space = false;
		}
	}
	if (word_buf.size() > 0)
		tab.push_back(word_buf);
	return tab;
}

class Server
{
private:
	std::vector<Channel *> _channelList;					//채널목록;
	std::vector<Client *> _clientList;					//전체 클라이언트목록;
	int _port;											//포트 번호
	std::string _password;								// 서버 비밀번호
	std::string _msgBuffer;									// 클라이언트가 보내는 메세지 버퍼;

	int _serverSocketFd;//서버소켓fd;
	sockaddr_in _serverSocketAddr;//서버소켓주소;
	Command _command;

	pollfd _pollClient[OPEN_MAX];
	int _pollLet;
	int _maxClient;

	socklen_t _clientLen;
	int _clientFd;
	sockaddr_in _clientAddr;

	int pollingEvent(int &index){
		_clientLen = sizeof(_clientAddr);
		_clientFd = accept(_serverSocketFd, (struct sockaddr *)&_clientAddr, &_clientLen);
		//TODO : accept 예외처리
		//Client tmp(_clientFd);
		_clientList.push_back(new Client(_clientFd));
		std::cout << "client fd: " << _clientList.back()->getClientFd() << std::endl;
		std::cout << "connect client\n";
		//비번 확인

		for (index = 1; index < OPEN_MAX; index++)
		{
			if (_pollClient[index].fd < 0)
			{
				_pollClient[index].fd = _clientFd;
				break;
			}
		}
		// TODO: i 최대값 확인하기
		_pollClient[index].events = POLLIN;
		if (index > _maxClient)
			_maxClient = index;
		//std::cout << "pollLet " << _pollLet << std::endl;
		if (--_pollLet <= 0)
			return (-1);
		return (0);
	}
	std::vector<std::pair<int, std::string> > parsing(); //명령어 파싱함수(); 잘라서 벡터로 반환
	// TODO : 나중에 CMD, ARG 등 #define
	// NICK, JOIN, USER, MSG, KICK, BAN, PASS, QUIT, HELP


public:
	Server(int port, std::string password) : _port(port), _password(password)
	{
		sock_init();
		//poll_init();
		_pollLet = 0;
		_maxClient = 0;
		_pollClient[0].fd = _serverSocketFd;
		_pollClient[0].events = POLLIN;//읽을 데이터가 있다는 이벤트

		// pollifd 구조체의 모든 fd를 -1로 초기화
		for (int i = 1; i < OPEN_MAX; i++){
			_pollClient[i].fd = -1;
		}
		//Client a(_serverSocketFd);//DUMMY
		_clientList.push_back(new Client(_serverSocketFd));
		//TODO : 생성자에서 try catch를 해도 되는가?
	}
	~Server(){};	//소멸자
	// 각 명령어 함수;
	// 클라이언트 관리 함수(get / setClient);
	// 채널 관리 함수(get / setChannel);
	// TODO : 어떤 명령어 할 것인지 조사
	// TODO : 예외처리 클래스 구현

	/* 서버 생성시 해야하는 일
	1. 메인소켓생성
	2. 바인드
	3. 리슨
	4. accept ( pollfd )
	5. 파싱?
	*/
	int sock_init()
	{
		_serverSocketFd = socket(PF_INET, SOCK_STREAM, 0); //PF_INET, SOCK_STREAM, 0
		_serverSocketAddr.sin_family = AF_INET;
		_serverSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_serverSocketAddr.sin_port = htons(_port);

		if (bind(_serverSocketFd, (const sockaddr *)&_serverSocketAddr, sizeof(_serverSocketAddr)))
			exit(1);
		if (listen(_serverSocketFd, 5) == -1)
			exit(3);
		std::cout << "listening" << std::endl;
		return (0);
	//TODO : 에러처리 나중에 어떻게 할 지 생각할 것
	};
	void check_cmd(std::vector<std::string> cmd_vec, Client *client){
		if (cmd_vec[0] == "NICK")
			_command.nick(cmd_vec, _clientList, client);
		else if (cmd_vec[0] == "USER")
			_command.user(cmd_vec);
		else if (cmd_vec[0] == "JOIN")
			_command.join(cmd_vec, client, _channelList);
		else if (cmd_vec[0] == "KICK")
			_command.kick(cmd_vec);
		else if (cmd_vec[0] == "PRIVMSG")
			_command.privmsg(cmd_vec, client, _clientList, _channelList);
		else if (cmd_vec[0] == "PASS")
			_command.pass(cmd_vec);
		else if (cmd_vec[0] == "PART")
			_command.part(cmd_vec);
		else if (cmd_vec[0] == "QUIT")
			_command.quit(cmd_vec);
		else if (cmd_vec[0] == "WHOIS")
			_command.whois(cmd_vec, client);
		else if (cmd_vec[0] == "CAP")//CAP LS
			_command.welcome(cmd_vec, client);
		else //미구현 커맨드 알림 또는 커맨드 무시
			std::cout << "undefined cmd\n";
	};


	void addChannelList(std::string channelName)
	{
		_channelList.push_back(new Channel(channelName));
	}
	void relayEvent()
	{
		char buf[512]; //
		for (int i = 1; i <= _maxClient; i++)
		{

			if (_pollClient[i].fd < 0)
				continue;
			if (_pollClient[i].revents & (POLLIN)){
				memset(buf, 0x00, 512);
				// read하고 write하기
				if (recv(_pollClient[i].fd, buf, 512, 0) <= 0) //
				{
					_pollClient[i].fd = -1;
				}
				else
				{
					_msgBuffer = std::string(buf);
					std::cout << "--- recvMsgBuf --- \n" << _msgBuffer << std::endl;
					std::cout << "--- endRecvMsgBuf --- " << std::endl;

					std::vector<std::string> cmd = CrlfSplit(_msgBuffer, "\r\n");
					//TODO : 명령어 순서대로 처리하는 함수 추가하기
					std::vector<std::string>::iterator cmd_it = cmd.begin();
					while (cmd_it != cmd.end())
					{
						std::vector<std::string> result = split(*cmd_it, ' ');
						check_cmd(result, _clientList[i]); //클라이언트를 가지고 갈 것?
						cmd_it++;
					}
					_msgBuffer.clear();
				}
			}
			else if (_pollClient[i].revents & (POLLERR)){
				std::cout <<"--- ERROR ---" << std::endl;
				exit(3);
			}
			if (_clientList[i]->getMsgBuffer().empty() == false){//send버퍼 있는 지 확인해서 있으면 send
				// std::cout << "--- pollout ---" << std::endl;
				std::string tmp = _clientList[i]->getMsgBuffer();
				send(_pollClient[i].fd, tmp.c_str(), tmp.length(), 0);
				std::cout << "sendMsg : " << tmp << std::endl;
				tmp.clear();
				_clientList[i]->setMsgBuffer("");
				// std::cout << "--- endpollout ---" << std::endl;
			}
		}
	}

	int execute()
	{
		int index = OPEN_MAX;
		while (42)
		{
			_pollLet = poll(_pollClient, _maxClient + index, -1); //반환값?
			//std::cout << "176" << std::endl;
			if (_pollLet == 0 || _pollLet == -1)
				break;
			//std::cout << "179" << std::endl;
			// pollin 이벤트 받으면은 실행
			if (_pollClient[0].revents & POLLIN)
			{
				if ((pollingEvent(index)) == -1){
					continue;
				}
			}
			relayEvent();//recvEvent, sendEvent;
		}
		close(_serverSocketFd);
		return (0);
	}; // 소켓통신 실행함수();
};

#endif
