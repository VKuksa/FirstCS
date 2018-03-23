#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

class Server
{
public:
	Server();
	bool bind_to_port();
	void work_on_client(int flag);
	void say_hello_to_client(SOCKET);
	void change_port(SOCKET, sockaddr_in, char*, int);
	~Server();
private:
	SOCKET server_socket;
	int port;
	char s_hello[46] = "Connection established via port ";
	char buff[1024];
};

