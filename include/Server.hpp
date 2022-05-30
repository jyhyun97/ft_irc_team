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
	// std::vector<Channel *> _channelList;					//채널목록;
	// std::vector<Client *> _clientList;					//전체 클라이언트목록;
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

	int pollingEvent(int &index);
	std::vector<std::pair<int, std::string> > parsing(); //명령어 파싱함수(); 잘라서 벡터로 반환
	// TODO : 나중에 CMD, ARG 등 #define
	// NICK, JOIN, USER, MSG, KICK, PASS, QUIT


public:
	Server(int port, std::string password);
	~Server();	//소멸자

	std::map<int, Client *> &getClientList();
	std::map<std::string, Channel *> &getChannelList();
	Client *findClient(int fd);
	Client *findClient(std::string nick);
	Channel *findChannel(std::string name);
	std::string getPass();
	// 각 명령어 함수;
	// 클라이언트 관리 함수(get / setClient);
	// 채널 관리 함수(get / setChannel);
	// TODO : 예외처리 클래스 구현

	/* 서버 생성시 해야하는 일
	1. 메인소켓생성
	2. 바인드
	3. 리슨
	4. accept ( pollfd )
	5. 파싱?
	*/
	int sock_init();
	void check_cmd(std::vector<std::string> cmd_vec, Client *client);

	void addChannelList(std::string channelName, int fd);
	void relayEvent();

	int execute(); // 소켓통신 실행함수();
};

#endif
