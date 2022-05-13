#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/event.h>
#include <vector>

int main(int ac, char *av[])
{
	int main_sock;
	sockaddr_in main_addr;

	if(ac != 3)
	{
		std::cout << "Usage : ./server <port> <password>\n";
        return (1);
    }
	main_sock = socket(PF_INET, SOCK_STREAM, 0);
	main_addr.sin_family = AF_INET;
	main_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	main_addr.sin_port = htons(atoi(av[1]));
	
    bind(main_sock, (const sockaddr *)&main_addr, sizeof(main_addr));

	if(listen(main_sock, 5) == -1)
        return (1);
    std::cout << "listening\n";
    std::vector<struct kevent> event;
    //struct kevent event;//모니터 배열이든 벡터든 만들고..
    //struct kevent tevent;//트리거
    // -------  kqueue init  ----------
    int kqueue_fd = kqueue();
    struct kevent first_kq;
    EV_SET(&first_kq, main_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);    
    event.push_back(first_kq);
    
    socklen_t client_len;
    int client_fd;
    sockaddr_in client_addr;
    struct kevent eventList[100]; //진짜 이벤트가 발생한 애들을 모아두는 곳(?)
    int eventCount = 0;
    struct kevent *currEvent;
    char buf[255];

    while (42)
    {
        // fq에 새로 모니터링할 이벤트 등록, 아직 처리되지 않은 이밴트 개수 리턴
        eventCount = kevent(kqueue_fd, &event[0], event.size(), eventList, 100, 0);
        event.clear();
        // std::cout << "in while\n";
        // 이벤트리스트에 들어온 친구들 처리해주기
        for (int i = 0; i < eventCount; i++)
		{
            currEvent = &(eventList[i]);
            if (currEvent->flags & EV_ERROR)
                exit(1);
            else if (currEvent->filter == EVFILT_READ)
            {
                std::cout << "EVFILT_READ ?" << std::endl;
                if (currEvent->ident == main_sock){
                    //클라이언트 accept
                    client_len = sizeof(client_addr);
                    std::cout << "wait connecting\n";
                    client_fd = accept(main_sock, (struct sockaddr *)&client_addr, &client_len);

                    //이벤트 추가
                    struct kevent temp_event;
                    EV_SET(&temp_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
                    event.push_back(temp_event);
                    EV_SET(&temp_event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
                    event.push_back(temp_event);
                    std::cout << "client connect\n" << std::endl;
                    //정보 갱신
                }
                else{
                    memset(buf, 0x00, 255);
                    recv(currEvent->ident, buf, 100, MSG_DONTWAIT);//recv
                    std::cout << buf;
                    //클라이언트 read MSG
                }

            }
			else if (currEvent->filter == EVFILT_WRITE)
            {
                if (buf[0] != 0)
                {
                    // buf[0]='t';
                    // buf[1]='e';
                    // buf[2]='s';
                    // buf[3]='t';
                    // buf[4]='\0';
                    send(currEvent->ident, buf, 5, MSG_DONTWAIT);
                    memset(buf, 0x00, 255);
                }
                //send();
            }
        }
        
    }
    
    
    
//     int kqueue(void);
//     int kevent(int kq, const struct kevent *changelist, int nchanges,
//         struct kevent *eventlist, int nevents, const struct timespec *timeout);

    close(main_sock);
    return (0);
}