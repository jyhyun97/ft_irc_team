#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include "Channel.hpp"

class Channel;

class Client{
	private :
		std::string _nickName;
		std::string _userName;
		std::string _hostName;
		std::string _serverName;
		std::string _realName;
		int _clientFd;
		std::vector<std::string> _myChannelList;
		std::string _msgBuffer; //*클라이언트에게 보낼 메세지버퍼;
		// Server *server;
	public :
		Client(int clientFd);
		std::string getMsgBuffer();
		bool isRegist();
		void clearMsgBuffer();
		void appendMsgBuffer(std::string msgBuffer);
		int getClientFd();
		void addChannelList(std::string channelName);
		void setNickName(std::string nickName);
		std::string getNickName();
		void setUser(std::string userName, std::string hostName, std::string serverName, std::string realName);
		std::string getUserName();
		std::string getHostName();
		std::string getServerName();
		std::string getRealName();
		void DebugPrint();
        // getmyChannelList();
		// setmyChannelList();
		// get각종이름();
		// set각종이름();
		// partMyChannel(); //vector.erase 클라이언트가 채널을 나갈 때
		//TODO : part명령어가 여러개의 채널을 동시에 떠날 수 있는지 체크하기
};



#endif
