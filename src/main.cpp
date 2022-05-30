#include <iostream>
#include <cstdlib>
#include "../include/Server.hpp"
#include <signal.h>

Server *server;

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

void	sigIntHandler(int num)
{
	if (num == SIGINT)
		delete server;
	system("leaks ircserv");
	exit(1);
}

void	sigQuitHandler(int num)
{
	if (num == SIGQUIT)
		delete server;
	system("leaks ircserv");
	exit(1);
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
	signal(SIGINT, sigIntHandler);
	signal(SIGQUIT, sigQuitHandler);
	server = new Server(atoi(argv[1]), argv[2]);
	if (server->execute() < 0)
	{
		delete server;
		return (1);
	}
	delete server;
	return 0;
}
