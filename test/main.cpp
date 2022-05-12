#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/poll.h> 


//할 것
//서버 만들고
//여러 개 클라이언트에게서 메세지 받기?

int main(int ac, char *av[])
{
	int main_sock;
	sockaddr_in main_addr;
    // int socket_1;
    // sockaddr_in socket_1_addr;
	// socklen_t socket_1_size;
    // int socket_2;
    // sockaddr_in socket_2_addr;
    // socklen_t socket_2_size;
    char buf1[42];
    char buf2[42];

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
    //
    /*
    struct pollfd {
	    int     fd;
	    short   events;
	    short   revents;
    };
    */

    pollfd client[OPEN_MAX];
    int poll_let;
    int max_client = 0;
    int i = 0;
    
    client[0].fd = main_sock;
    client[0].events = POLLIN;//읽을 데이터가 있다는 이벤트

    // pollifd 구조체의 모든 fd를 -1로 초기화
    for (i = 1; i < OPEN_MAX; i++)
    {
        client[i].fd = -1;
    }

    socklen_t client_len;
    int client_fd;
    sockaddr_in client_addr;
    while (1)
    {
        poll_let = poll(client, max_client + i, -1);//반환값?
        if (poll_let == 0 || poll_let == -1)
            break;
        if (client[0].revents & POLLIN)
        {
            // pollin 이벤트 받으면은 실행
            client_len = sizeof(client_addr);
            client_fd = accept(main_sock, (struct sockaddr *)&client_addr, &client_len);
            for (i = 1; i < OPEN_MAX; i++){
                if (client[i].fd < 0)
                {
                    client[i].fd = client_fd;
                    break;
                }
            }   
            //TODO: i 최대값 확인하기
            client[i].events = POLLIN;
            if (i > max_client)
                max_client = i;
            if (--poll_let <= 0)
                continue;
        }
        //buf
        char buf[255];
        for (i = 1; i <= max_client; i++){
            if (client[i].fd < 0)
                continue;
            if (client[i].revents & (POLLIN | POLLERR)){
                memset(buf, 0x00, 255);
                //read하고 write하기
                if (read(client[i].fd, buf, 255) <= 0)
                {
                    client[i].fd = -1;
                }
                else
                {
                    for(int j = 0; j <= max_client; j++)
                    {
                        if (i != j)
                        {
                            write(client[j].fd, ">> ", 3);
                            write(client[j].fd, buf, 255);
                        }
                    }
                }
            }
        }
        //accept;
        //read/write;
    }

	// socket_1_size = sizeof(socket_1_addr);
	// socket_2_size = sizeof(socket_2_addr);
	// socket_1 = accept(main_sock, (struct sockaddr*)&socket_1_addr, (socklen_t *)&socket_1_size);
	// socket_2 = accept(main_sock, (struct sockaddr*)&socket_2_addr, (socklen_t *)&socket_2_size);
    
	// read(socket_1, buf1, 5);
	// read(socket_2, buf2, 5);
    // buf1[5] = '\0';
    // buf2[5] = '\0';

    // write(1, buf1, 5);
    // write(1, buf2, 5);

    close(main_sock);
    // close(socket_1);
    // close(socket_2);
    return (0);
}