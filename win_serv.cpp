#include "win_serv.h"
#define SEND_BACK 1			//send "success" back

Server::Server():port(7300)
{
	memset(buff, 0, sizeof(buff));
	std::cout << "TCP Server" << std::endl;

	WSADATA wsadata;

	if (WSAStartup(0x0202, &wsadata))
	{
		//if error with startup
		std::cout << "Error with WSAStartup " << WSAGetLastError();
	}
}

bool Server::bind_to_port()
{
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//AF_INET - internet socket SOCK_STREAM - socket for TCP
	{
		//if error with socket
		std::cout << "Error with socket " << WSAGetLastError();
		WSACleanup();	//Deinit of Winsock
	}

	sockaddr_in local_addr;			//Better version of sockaddr
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);	//conversion local short int into network format
	local_addr.sin_addr.s_addr = 0;			//receiving connections on all IPs


	if (bind(server_socket, (sockaddr *)&local_addr, sizeof(local_addr)))	//connecting
	{
		//Error with connection
		std::cout << "Error with bind " << WSAGetLastError();
		closesocket(server_socket);
		WSACleanup();
		return false;
	}

	if (listen(server_socket, 0x5))	//queue 0x5
	{
		//Error with connecting
		std::cout << "Error with listen " << WSAGetLastError();
		closesocket(server_socket);
		WSACleanup();
		return false;
	}
	return true;
}

void Server::work_on_client(int flag)
{
	SOCKET client_socket;
	sockaddr_in client_addr;

	int client_addr_size = sizeof(client_addr);

	if ((client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		static int num_of_calls = 0;

		if (num_of_calls == 0 && flag == 0)
		{
			say_hello_to_client(client_socket);
			num_of_calls++;
		}
		else if(flag == SEND_BACK)
		{
			Sleep(150);
			send(client_socket, "success", 9, 0);
		}

		char buff_c[1024];
		memset(buff_c, 0, sizeof(buff_c));

		while (int bytes_recv = recv(client_socket, &buff_c[0], sizeof(buff_c), 0) &&
			bytes_recv != SOCKET_ERROR)
		{
			if (strncmp(buff_c, "#1port", 6) == 0)			//Port change by "NewPort-
			{
				port = atoi(buff_c + 6);
				change_port(client_socket, client_addr, &buff_c[0], SEND_BACK);
			}
			else
			{
				std::cout << "\nClient: " << buff_c;
				std::cout << "\nServer: ";
				fgets(buff_c, 1024, stdin);

				if (strncmp(buff_c, "NewPort-", 8) == 0)
				{
					port = atoi(buff_c + 8);
					char portmsg[11] = "#1port";
					memcpy(portmsg + 6, buff_c + 8, 4);
					change_port(client_socket, client_addr, &portmsg[0], 0);
				}
				else
					send(client_socket, &buff_c[0], sizeof(buff_c), 0);
			}
		}
	}
	closesocket(client_socket);
}

void Server::say_hello_to_client(SOCKET client_socket)
{
	char s_hello[53] = "Hello Client. Connection established via port ";
	char * p_buff = new char[6];
	char * hello = new char[53];
	hello = s_hello;
	p_buff = _itoa(port, p_buff, 10);
	p_buff[4] = '\n';
	p_buff[5] = 0;
	strcat(hello, p_buff);

	send(client_socket, hello, strlen(hello), 0);
}

void Server::change_port(SOCKET client_socket, sockaddr_in client_addr, char* buff_c, int flag)
{
	send(client_socket, buff_c, 12, 0);
	closesocket(server_socket);
	bind_to_port();
	work_on_client(flag);
}

Server::~Server()
{
	closesocket(server_socket);
	WSACleanup();
}
