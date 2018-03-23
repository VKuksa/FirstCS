#include <iostream>
#include <WinSock2.h>
#include <Windows.h>


class Client
{
public:
	Client();
	int connection_to_port(sockaddr_in &dest_addr);
	bool change_port(sockaddr_in &dest_addr);
	int get_port();
	void starting_response();
	void dialog_with_server(sockaddr_in & dest_addr);
	~Client();
private:
	size_t port;
	char buff[1024];
	SOCKET client_socket;
	const char *serv_addr = "127.0.0.1";
};

