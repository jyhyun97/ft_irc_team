#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include "Client.hpp"


class Client;

class Channel{

private :
	std::string _channelName;//채널이름;
    std::vector<int> _myClientFdList;//접속중인 클라이언트 목록;
	std::string _operaterNickName; //방장이름
	int checkChannelName();//채널이름 중복 체크함수
	int setOperator(Client &client); //채널 방장 설정함수, 객체를 받아서 그 객체에게도 방장이 되었음을 알려줘야함.
	// Server *server;
public :
	Channel(std::string channelName) : _channelName(channelName){};//이 안에서 방장 설정 함수 실행
	~Channel(){};//소멸함수
	std::string getChannelName(){
		return (_channelName);
	};
	std::vector<int> getMyClientFdList()
	{
		return _myClientFdList;
	}
	Client getClient();
	void setClient();
	int leaveEvent(Client &client); //나간 클라이언트
	void addMyClientList(int fd)
	{
		_myClientFdList.push_back(fd);
	};
	//서버에서 클라이언트가 나가는 이벤트가 일어나면?
	//서버에서 그 사실을 채널로 전달 후 채널에서 방장인 지 확인 후 방장 변경 실행(주도권이 채널)
};

#endif
