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
		Client(int clientFd) : _clientFd(clientFd)
		{}
		std::string getMsgBuffer()
		{
			return _msgBuffer;
		};
		bool isRegist()
		{
			if (_nickName == "" || _userName == "" || _hostName == "" || _realName == "" || _serverName == "")
				return (false);
			return(true);
		}
		void clearMsgBuffer()
		{
			_msgBuffer = "";
		};
		void appendMsgBuffer(std::string msgBuffer)
		{
			_msgBuffer.append(msgBuffer);
		};
		int getClientFd(){
			return _clientFd;
		}
		void addChannelList(std::string channelName)
		{
			_myChannelList.push_back(channelName);
		}
		void setNickName(std::string nickName)
		{
			_nickName = nickName;
		};
		std::string getNickName()
		{
			return _nickName;
		};
		void setUser(std::string userName, std::string hostName, std::string serverName, std::string realName){
			_userName = userName;
			_hostName = hostName;
			_serverName = serverName;
			_realName = realName;
		};
		std::string getUserName(){
			return _userName;
		};
		std::string getHostName(){
			return _hostName;
		};
		std::string getServerName(){
			return _serverName;
		};
		std::string getRealName(){
			return _realName;
		};

		void DebugPrint(){
			std::cout << "nickName : " << _nickName << std::endl;
			std::cout << "userName : " << _userName << std::endl;
			std::cout << "hostName : " << _hostName << std::endl;
			std::cout << "serverName : " << _serverName << std::endl;
			std::cout << "realName : " << _realName << std::endl;
			std::cout << "clientFd : " << _clientFd << std::endl;
			std::cout << "myChannelList : " << std::endl;
			// print_channelList(_myChannelList);
			std::cout << "msgBuffer : " << _msgBuffer << std::endl;
		};

        // getmyChannelList();
		// setmyChannelList();
		// get각종이름();
		// set각종이름();
		// partMyChannel(); //vector.erase 클라이언트가 채널을 나갈 때
		//TODO : part명령어가 여러개의 채널을 동시에 떠날 수 있는지 체크하기
};



#endif
