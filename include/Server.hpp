#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include "Channel.hpp"
#include "Client.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <unistd.h>

class Channel;
class Client;

class Server
{
private:
	std::vector<Channel> _channelList;					//채널목록;
	std::vector<Client> _clientList;					//전체 클라이언트목록;
	int _port;											//포트 번호
	std::string _password;								// 서버 비밀번호
	std::string _msgBuffer;									// 클라이언트가 보내는 메세지 버퍼;

	int _serverSocketFd;//서버소켓fd;
	sockaddr_in _serverSocketAddr;//서버소켓주소;

	std::vector<std::pair<int, std::string> > parsing(); //명령어 파싱함수(); 잘라서 벡터로 반환
	// TODO : 나중에 CMD, ARG 등 #define
	// NICK, JOIN, USER, MSG, KICK, BAN, PASS, QUIT, HELP


public:
	Server(int port, std::string password) : _port(port), _password(password)
	{
		sock_init();
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
		return (0);
		//TODO : 에러처리 나중에 어떻게 할 지 생각할 것
	};
	int execute()
	{
		pollfd pollClient[OPEN_MAX];//
		int poll_let;
		int max_client = 0;
		int i = 0;
		
		pollClient[0].fd = _serverSocketFd;
		pollClient[0].events = POLLIN;//읽을 데이터가 있다는 이벤트

		// pollifd 구조체의 모든 fd를 -1로 초기화
		for (i = 1; i < OPEN_MAX; i++)
			pollClient[i].fd = -1;

		socklen_t client_len;
		int client_fd;
		sockaddr_in client_addr;
		while (1)
		{
			poll_let = poll(pollClient, max_client + i, -1); //반환값?
			if (poll_let == 0 || poll_let == -1)
				break;
			if (pollClient[0].revents & POLLIN)
			{
				// pollin 이벤트 받으면은 실행
				client_len = sizeof(client_addr);
				client_fd = accept(_serverSocketFd, (struct sockaddr *)&client_addr, &client_len);
				std::cout << "connect client\n";
				for (i = 1; i < OPEN_MAX; i++)
				{
					if (pollClient[i].fd < 0)
					{
						pollClient[i].fd = client_fd;
						break;
					}
				}
				// TODO: i 최대값 확인하기
				pollClient[i].events = POLLIN;
				if (i > max_client)
					max_client = i;
				if (--poll_let <= 0)
					continue;
			}
			// buf
			char buf[512];//
			for (i = 1; i <= max_client; i++)
			{
				if (pollClient[i].fd < 0)
					continue;
				if (pollClient[i].revents & (POLLIN | POLLERR))
				{
					memset(buf, 0x00, 512);
					// read하고 write하기
					if (recv(pollClient[i].fd, buf, 512, 0) <= 0)//
					{
						pollClient[i].fd = -1;
					}
					else
					{
						_msgBuffer = std::string(buf);
						for (int j = 0; j <= max_client; j++)
						{
							if (i != j)
							{
								send(pollClient[j].fd, ">> ", 3, 0);
								send(pollClient[j].fd, _msgBuffer.c_str(), _msgBuffer.length(), 0);
							}
						}
					}
				}
			}
		}
		close(_serverSocketFd);
		return (0);
	}; // 소켓통신 실행함수();
};

#endif