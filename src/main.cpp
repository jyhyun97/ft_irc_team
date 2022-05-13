#include <iostream>
#include <cstdlib>
#include "../include/Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./server <port> <password>\n";
        exit(1);
    }
    //TODO : 매개변수 예외처리(port, password);
    
    Server server(atoi(argv[1]), argv[2]); //생성자
	if (server.execute() < 0)
		return (1);
	return 0;
}