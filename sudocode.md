### main.cpp
``` C++
int main(int argc, char **argv){
    매개변수 예외처리(port, password);
    server(port, password); //생성자
	server.실행함수();
	return 0;
}
```


### 서버.cpp
``` C++
class server{
    private : 
    std::vector<Channel> 채널목록;
    std::vector<Client> 전체 클라이언트목록;
	std::string 비번;
    char * 클라이언트가 보내는 메세지 버퍼;
	// const char *s = "Hello, World!";
	// std::string str(s);
	생성자();
    소멸자();
    명령어 파싱함수(); //

    public :
    각 명령어 함수;
	클라이언트 추가함수(get/set_client);
	채널 추가함수 (get/set_channel);
	메세지 버퍼 get();
}
```


### 채널.cpp
``` C++
class channel{

private : 
	std::string 채널이름;
    std::vector<Client> 접속중인 클라이언트 목록;
	std::string 방장이름
	채널이름 체크함수();
	방장설정 함수(클라이언트 닉네임);
public :
	생성자();//이 안에서 방장 설정 함수 실행
	방장 변경 함수();
	get/set 클라이언트();
}
```

### 클라이언트.cpp
``` C++
class client{
	private :
		bool isOperator; //방장이면 true
        std::string 각종이름;
        std::vector<Channel> 클라이언트가 접속중인 채널목록
        char *클라이언트에게 보낼 메세지버퍼;
    public : 
        get/set 채널목록();
		leave 채널(); //vector.erase
}


```