#include "win_clnt.h"



Client::Client(): port(7300)
{
	memset(buff, 0, sizeof(buff));

	WSADATA wsadata;

	if (WSAStartup(0x0202, &wsadata))
	{
		//if error with startup
		std::cout << "Error with WSAStartup " << WSAGetLastError();
	}
}

int Client::connection_to_port(sockaddr_in & dest_addr)
{
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//AF_INET - internet socket SOCK_STREAM - socket for TCP
	{
		//if error with socket
		std::cout << "Error with socket " << WSAGetLastError();
		WSACleanup();	//Deinit of Winsock
	}

	HOSTENT *hst;

	if (inet_addr(serv_addr) != INADDR_NONE)		//gettin host addr
		dest_addr.sin_addr.s_addr = inet_addr(serv_addr);
	else
		if (hst = gethostbyname(serv_addr))
			((unsigned long*)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			//if err with Server addr
			std::cout << "Invalid address " << serv_addr << std::endl;
			closesocket(client_socket);
			WSACleanup();
		}

	if (connect(client_socket, (sockaddr*)&dest_addr, sizeof(dest_addr)))
	{
		//if err with connect
		std::cout << "COnnecting error " << WSAGetLastError() << std::endl;
	}
	return 0;
}

bool Client::change_port(sockaddr_in & dest_addr)
{
	if (closesocket(client_socket) == 0)
	{
		dest_addr.sin_port = htons(port);
		Sleep(100);				//We should w8 before server will be able to accept clients
		connection_to_port(dest_addr);
		return true;
	}
	return false;
}

int Client::get_port()
{
	return port;
}

void Client::starting_response()
{
	if ((recv(client_socket, &buff[0], sizeof(buff), 0)) != SOCKET_ERROR)
	{
		std::cout << buff;
		const char* response = "Successfully connected";
		send(client_socket, response, sizeof(buff), 0);
	}
}

void Client::dialog_with_server(sockaddr_in & dest_addr)
{
	while ((recv(client_socket, &buff[0], sizeof(buff), 0)) != SOCKET_ERROR)
	{
		if (strncmp(buff, "#1port",6) == 0)
		{
			port = atoi(buff + 6);
			if (change_port(dest_addr) != false)
				send(client_socket, "success", strlen("success"), 0);
			else
				send(client_socket, "failure", strlen("success"), 0);
		}
		else
		{
			std::cout << "\nServer: " << buff;

			std::cout << "\nClient: ";
			fgets(buff, 1024, stdin);
			if (strncmp(buff, "quit", 4) == 0)
			{
				std::cout << "Exit";
				closesocket(client_socket);
				WSACleanup();
				exit(0);
			}
			if (strncmp(buff, "NewPort-", 8) == 0)
			{
				port = atoi(buff + 8);
				char portmsg[11] = "#1port";
				memcpy(portmsg + 6, buff + 8, 4);
				send(client_socket, &portmsg[0], sizeof(portmsg), 0);
				change_port(dest_addr);
			}
			else
				send(client_socket, &buff[0], sizeof(buff), 0);
		}
	}
}

Client::~Client()
{
	closesocket(client_socket);
	WSACleanup();
}
