#include <iostream>
#include <cstdlib>
#include "../include/Server.hpp"

static int check_port(char *port)
{
	int i = 0;
	while(port[i])
	{
		if (port[i] < '0' || '9' < port[i])
		{
			std::cout << "Check Port-number\n";
			return (-1);
		}
		i++;
	}
	return (0);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./server <port> <password>\n";
        exit(1);
    }
	if (check_port(argv[1]) == -1)
		exit(1);
    Server server(atoi(argv[1]), argv[2]); //생성자
	if (server.execute() < 0)
		return (1);
	return 0;
}
