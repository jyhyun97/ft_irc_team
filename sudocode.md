### main.cpp
``` C++
int main(int argc, char **argv){
    매개변수 예외처리(port, password);
    Server(port, password); //생성자
	if (Server.execute() < 0)
		return (1);
	return 0;
}
```


### 서버.cpp
``` C++

class Server{
    private : 
    std::vector<Channel> _channelList;//채널목록;
    std::vector<Client> _clientList; //전체 클라이언트목록;
	std::string _password; // 서버 비밀번호
    char *_msgBuffer; // 클라이언트가 보내는 메세지 버퍼;
	Server(int port, std::string password); //생성자
    ~Server(); //소멸자
    std::vector<std::pair<int, std::string> > parsing(); //명령어 파싱함수(); 잘라서 벡터로 반환
	//TODO : 나중에 CMD, ARG 등 #define

    public :
    각 명령어 함수;
	클라이언트 관리 함수(get/setClient);
	채널 관리 함수 (get/setChannel);
	//TODO : 어떤 명령어 할 것인지 조사
	//TODO : 예외처리 클래스 구현
	
	int execute(); // 소켓통신 실행함수();
}
```


### 채널.cpp
``` C++
class Channel{

private : 
	std::string _channelName;//채널이름;
    std::vector<Client> _clientList;//접속중인 클라이언트 목록;
	std::string _operaterNickName; //방장이름
	int checkChannelName();//채널이름 중복 체크함수
	int setOperator(Client &client); //채널 방장 설정함수, 객체를 받아서 그 객체에게도 방장이 되었음을 알려줘야함.
public :
	Channel();//이 안에서 방장 설정 함수 실행
	~Channel();//소멸함수
	Client getClient();
	void setClient();
	int leaveEvent(Client &client); //나간 클라이언트
	//서버에서 클라이언트가 나가는 이벤트가 일어나면?
	//서버에서 그 사실을 채널로 전달 후 채널에서 방장인 지 확인 후 방장 변경 실행(주도권이 채널)
}
```

### 클라이언트.cpp
``` C++
class Client{
	private :
		bool _isOperator; //방장이면 true
        std::string 각종이름;
		//TODO : 사용되는 이름 조사하기
        std::vector<Channel> _myChannelList;
        char *_msgBuffer; //*클라이언트에게 보낼 메세지버퍼;
    public : 
        getmyChannelList();
		setmyChannelList();
		get각종이름();
		set각종이름();
		partMyChannel(); //vector.erase 클라이언트가 채널을 나갈 때 
		//TODO : part명령어가 여러개의 채널을 동시에 떠날 수 있는지 체크하기 
}
```