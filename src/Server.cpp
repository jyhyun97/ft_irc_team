#include "../include/Server.hpp"
// #include <signal.h>

// #include <netinet/tcp.h>


int Server::pollingEvent(int &index){
	_clientLen = sizeof(_clientAddr);
	_clientFd = accept(_serverSocketFd, (struct sockaddr *)&_clientAddr, &_clientLen);

	if (_clientFd < 0) {
		std::cerr << "Error accepting client" << std::endl;
		return -1;
	}

	//TODO : accept 예외처리
	_clientList.insert(std::pair<int, Client *>(_clientFd, new Client(_clientFd)));
	std::cout << C_RED <<"\n\n*Accept Client fd: " << _clientList.find(_clientFd)->first << "*" << C_NRML <<std::endl;
	// std::cout << "connect client\n";
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
	if (--_pollLet <= 0)
		return (-1);
	return (0);
}

Server::Server(int port, std::string password) : _command(this){
	_pollLet = 0;
	_maxClient = 0;
	_port = port;
	_password = password;
	sock_init();
	_pollClient[0].fd = _serverSocketFd;
	_pollClient[0].events = POLLIN;//읽을 데이터가 있다는 이벤트

	// std::cout << "poll[0] fd" << _pollClient[0].fd << std::endl;
	//_command = new Command(*this);
	//_command = Command(this);

	// pollifd 구조체의 모든 fd를 -1로 초기화
	for (int i = 1; i < OPEN_MAX; i++){
		_pollClient[i].fd = -1;
	}
	//Client a(_serverSocketFd);//DUMMY
	// 인덱스로 접근하는거 고쳐야함

	//TODO : 생성자에서 try catch를 해도 되는가?
}

template <class T1, class T2>
void deleteMap(std::map<T1, T2> &map){
	typename std::map<T1, T2>::iterator it1 = map.begin();
	typename std::map<T1, T2>::iterator it2 = it1;
	
	while (it1 != map.end())
	{
		it1++;
		delete it2->second;
		it2 = it1;
	}
};


Server::~Server(){
	std::cout << "server destructer called\n";
	// 서버닫기전에 전부 처리
	// 맵 돌면서 모든 채널과 클라이언트 제거
	// deleteClientMap();
	// deleteChannelMap();
	// 	std::map<int, Client *> _clientList;
	// std::map<std::string, Channel *> _channelList;
	deleteMap(_clientList);
	deleteMap(_channelList);
}


std::map<int, Client *> &Server::getClientList() {
	return _clientList;
}

std::string Server::getPass()
{
	return _password;
}

std::map<std::string, Channel *> &Server::getChannelList() {
	return _channelList;
}

Client* Server::findClient(int fd) {
	return _clientList.find(fd)->second;
}

Client* Server::findClient(std::string nick) {
	std::map<int, Client *>::iterator it = _clientList.begin();
	for (; it != _clientList.end(); it++)
	{
		if (it->second->getNickName() == nick)
			return it->second;
	}
	return NULL;
};

Channel* Server::findChannel(std::string name) {
	if (_channelList.find(name) == _channelList.end())
		return NULL;
	return _channelList.find(name)->second;
}

int Server::sock_init(){
	_serverSocketFd = socket(PF_INET, SOCK_STREAM, 0); //PF_INET, SOCK_STREAM, 0
	_serverSocketAddr.sin_family = AF_INET;
	_serverSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serverSocketAddr.sin_port = htons(_port);

	// int option = 1;
	// setsockopt(_serverSocketFd, IPPROTO_TCP, TCP_NODELAY, (void *)&option, sizeof(option));
	//setsockopt(_serverSocketFd, IPPROTO_TCP, TCP_NODELAY, (const sockaddr *)&_serverSocketAddr, sizeof(_serverSocketAddr));

	// SO_REUSEADDR time-wait 중인 소켓에 재할당 가능하도록 설정
	int optval = true;
	socklen_t optlen = sizeof(optval);
	setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, optlen);

	if (bind(_serverSocketFd, (const sockaddr *)&_serverSocketAddr, sizeof(_serverSocketAddr)))
	{
		std::cerr << "Error binding socket" << std::endl;
		exit(1);
	}
	if (listen(_serverSocketFd, 15) == -1)
	{
		std::cerr << "Error listening socket" << std::endl;
		exit(3);
	}
	std::cout << C_GREN << "listening" << std::endl << C_NRML;
	return (0);
//TODO : 에러처리 나중에 어떻게 할 지 생각할 것
}

void Server::check_cmd(std::vector<std::string> cmd_vec, Client *client){
	if (cmd_vec[0] == "NICK")
		_command.nick(cmd_vec, client);
	else if (cmd_vec[0] == "JOIN")
		_command.join(cmd_vec, client);
	else if (cmd_vec[0] == "KICK")
		_command.kick(cmd_vec, client);
	else if (cmd_vec[0] == "PRIVMSG")
		_command.privmsg(cmd_vec, client);
	else if (cmd_vec[0] == "PING")
		_command.pong(cmd_vec, client);
	// else if (cmd_vec[0] == "PASS")
	// 	_command.pass(cmd_vec, client);
	else if (cmd_vec[0] == "PART")
		_command.part(cmd_vec, client);
	else if (cmd_vec[0] == "QUIT")
		_command.quit(cmd_vec, client);
	// else if (cmd_vec[0] == "WHOIS")
	// 	_command.whois(cmd_vec, client);
	else //미구현 커맨드 알림 또는 커맨드 무시
		std::cout << "undefined cmd\n";
}

void Server::addChannelList(std::string channelName, int fd)
{
	_channelList.insert(std::pair<std::string, Channel *>(channelName, new Channel(channelName, fd)));
}

void Server::relayEvent()
{
	char buf[512]; //
	for (int i = 1; i <= _maxClient; i++)
	{
		if (_pollClient[i].fd < 0)
			continue;
		if (_pollClient[i].revents & (POLLIN))
		{
			memset(buf, 0x00, 512);
			// read하고 write하기
			if (recv(_pollClient[i].fd, buf, 512, 0) <= 0) //
			{
				_pollClient[i].fd = -1;
			}
			else
			{
				_msgBuffer = std::string(buf);
				std::cout << C_YLLW << "---- recvMsgBuf --- \n";
				std::cout << _msgBuffer << std::endl;
				std::cout << "pollfd : " << _pollClient[i].fd << std::endl;
				std::cout << "ㄴ--- endRecvMsgBuf ---\n\n\n" << C_NRML << std::endl;

				Client * tmp = (_clientList.find(_pollClient[i].fd))->second;
				std::vector<std::string> cmd = split(_msgBuffer, "\r\n");
				// TODO : 명령어 순서대로 처리하는 함수 추가하기
				print_stringVector(cmd);
				// 클라이언트가 등록이 안되어있을 때
				if (!(tmp->getRegist() & REGI))
				{
					_command.welcome(cmd, (_clientList.find(_pollClient[i].fd))->second, _clientList);
				}
				else
				{
					// 이미 등록된 클라이언트에서 이벤트 발생
					std::vector<std::string>::iterator cmd_it = cmd.begin();
					while (cmd_it != cmd.end())
					{
						std::vector<std::string> result = split(*cmd_it, " ");
						check_cmd(result, tmp); //클라이언트를 가지고 갈 것?
						cmd_it++;
					}
				}
				_msgBuffer.clear();
			}
		}
		else if (_pollClient[i].revents & (POLLERR))
		{
			std::cout << "--- ERROR ---" << std::endl;
			exit(3);
		}
	}
	std::map<int, Client *>::iterator it = _clientList.begin();
	for (; it != _clientList.end(); it++)
	{
		if (it->second->getMsgBuffer().empty() == false)
		{ // send버퍼 있는 지 확인해서 있으면 send
			std::string str = it->second->getMsgBuffer();
			send(it->first, str.c_str(), str.length(), 0);
			std::cout << C_BLUE <<"----- sendMsg to <" << it->first << "> -------\n";
			std::cout << str;
			std::cout << "ㄴ--------------------------\n" << std::endl << C_NRML;
			str.clear();
			it->second->clearMsgBuffer();
		}
	}
}

int Server::execute(){
	int index = OPEN_MAX;
	//signal(SIGPIPE, SIG_IGN);
	while (42)
	{
		_pollLet = poll(_pollClient, _maxClient + index, -1); //반환값?
		if (_pollLet == 0 || _pollLet == -1)
			break;
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
}


void Server::removeUnconnectClient(int fd)
{
	Client *tmp = findClient(fd);
	
	std::string str = tmp->getMsgBuffer();
	send(fd, str.c_str(), str.length(), 0);
	
	std::cout << C_BLUE <<"----- in removeclient sendMsg to <" << fd << "> -------\n";
	std::cout << str;
	std::cout << "ㄴ--------------------------\n" << std::endl << C_NRML;
	str.clear();
	tmp->clearMsgBuffer();
	getClientList().erase(fd);
    close(fd);
    delete tmp;
}