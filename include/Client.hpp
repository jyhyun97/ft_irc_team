#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include "Channel.hpp"

class Channel;

class Client{
	private :
		std::string _nickName;
        std::string stuff;//각종이름;//nick real username hostname 등..등.....
		//TODO : 사용되는 이름 조사하기
		int _clientFd;
        std::vector<Channel *> _myChannelList;
        std::string _msgBuffer; //*클라이언트에게 보낼 메세지버퍼;
    public :
		Client(int clientFd) : _clientFd(clientFd){
		}
		std::string getMsgBuffer(){
			return _msgBuffer;
		};
		void setMsgBuffer(std::string msgBuffer){
			_msgBuffer = msgBuffer;
		};
		int getClientFd(){
			return _clientFd;
		}
		void addChannelList(Channel *channel)
		{
			_myChannelList.push_back(channel);
		}
		void setNickName(std::string nickName)
		{
			_nickName = nickName;
		};
		std::string getNickName()
		{
			return _nickName;
		};
        // getmyChannelList();
		// setmyChannelList();
		// get각종이름();
		// set각종이름();
		// partMyChannel(); //vector.erase 클라이언트가 채널을 나갈 때 
		//TODO : part명령어가 여러개의 채널을 동시에 떠날 수 있는지 체크하기 
};



#endif