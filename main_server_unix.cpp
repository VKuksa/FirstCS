#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#define SEND_BACK 1      //send "success" back
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <unistd.h> // for close connection
#include <cstring>
#include <stdlib.h>


class Server
{
public:
  Server();
  bool bind_to_port();
  void work_on_client();
  void change_port(int client_socket, sockaddr_in client_addr, char* buff_c);
  ~Server();

private:
  int server_socket;
  int port;
  char buff[1024];
};

Server::Server():port(7300)
  {
    memset(buff, 0, sizeof(buff));
    std::cout << "TCP Server" << std::endl;
  }

bool Server::bind_to_port()
  {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  //AF_INET - internet socket SOCK_STREAM - socket for TCP
    {
      //if error with socket
      perror("Error with socket ");
      //WSACleanup();  //Deinit of Winsock
    }
    std::cout << "Socket created ¹" << server_socket << std::endl;
    sockaddr_in local_addr;      //Better version of sockaddr
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);  //conversion local short int into network format
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);      //receiving connections on all IPs

    std::cout << ntohs(local_addr.sin_port) << " port" << std::endl;
    std::cout << port << " private port" << std::endl;
    if (bind(server_socket, (struct sockaddr *)&local_addr, sizeof(local_addr))<0)  //connecting
    {
      //Error with connection
      perror("Error with bind ");
      close(server_socket);
      return false;
    }
    std::cout << "Server binded" << std::endl;
    if (listen(server_socket, 0x5) < 0)  //queue 0x5
    {
      //Error with connecting
      perror("Error with listen ");
      close(server_socket);
      return false;
    }
        std::cout << "Server is listening" << std::endl;
    return true;
  }

 void Server::work_on_client()
  {
    int client_socket;
    sockaddr_in client_addr;

    socklen_t client_addr_size = sizeof(client_addr);

    if ((client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_addr_size)))
    {
        send(client_socket, "success", 9, 0);
        std::cout << "Accepted client" << std::endl;
      char buff_c[1024];
      memset(buff_c, 0, sizeof(buff_c));

      while (int bytes_recv = recv(client_socket, &buff_c[0], sizeof(buff_c), 0) &&
        bytes_recv != -1)
      {
        if (strncmp(buff_c, "#1port", 6) == 0)      //Port change by "NewPort-
        {
          port = atoi(buff_c + 6);
          change_port(client_socket, client_addr, &buff_c[0]);
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
            change_port(client_socket, client_addr, &portmsg[0]);
          }
          else
            send(client_socket, &buff_c[0], sizeof(buff_c), 0);
        }
      }
    }
    close(client_socket);
  }

void Server::change_port(int client_socket, sockaddr_in client_addr, char* buff_c)
{
  send(client_socket, buff_c, 12, 0);
  close(server_socket);
  bind_to_port();
  work_on_client();
}

Server::~Server()
  {
    close(server_socket);
  }

int main()
{
  Server server;

  if (!server.bind_to_port())
  {
    perror(" was the problem");
    exit(1);
  }

  server.work_on_client();
}
