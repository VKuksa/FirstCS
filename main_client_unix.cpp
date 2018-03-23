#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <unistd.h> // for close connection
#include <cstring>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>


class Client
{
public:
  Client();
  int connection_to_port(sockaddr_in & dest_addr);
  bool change_port(sockaddr_in & dest_addr);
  int get_port();
  void starting_response();
  void dialog_with_server(sockaddr_in & dest_addr);
  ~Client();
private:
  size_t port;
  char buff[1024];
  int client_socket;
  const char *serv_addr = "127.0.0.1";
};

Client::Client() : port(7300)
  {
    memset(buff, 0, sizeof(buff));
  }

int Client::connection_to_port(sockaddr_in & dest_addr)
  {
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  //AF_INET - internet socket SOCK_STREAM - socket for TCP
    {
      //if error with socket
      perror("Error with socket ");
    }
    std::cout << "Socket created " << client_socket << std::endl;

    if(dest_addr.sin_addr.s_addr = inet_addr(serv_addr) == -1)
    {
        perror("Error with server adress");
    }

    if (connect(client_socket, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0)
    {
      //if err with connect
      perror("COnnecting error ");
    }
    std::cout << "Connected" << std::endl;
    return 0;
  }

bool Client::change_port(sockaddr_in & dest_addr)
  {
    if (close(client_socket) == 0)
    {
      dest_addr.sin_port = htons(port);
      sleep(1);        //We should w8 before server will be able to accept clients
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
    if ((recv(client_socket, &buff[0], sizeof(buff), 0)) != -1)
    {
      std::cout << buff;
      const char* response = "Successfully connected";
      send(client_socket, response, sizeof(buff), 0);
    }
  }

void Client::dialog_with_server(sockaddr_in & dest_addr)
  {
    while ((recv(client_socket, &buff[0], sizeof(buff), 0)) != -1)
    {
      if (strncmp(buff, "#1port", 6) == 0)
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
          close(client_socket);
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
    close(client_socket);
  }

int main(int argc, char* argv[])
{
  std::cout << "TCP Client" << std::endl;

  Client client;

  sockaddr_in dest_addr;
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(client.get_port());

  if (!client.connection_to_port(dest_addr) == 0)
  {
    perror(" was the problem");
    exit(1);
  }

  client.starting_response();

  client.dialog_with_server(dest_addr);

  return -1;
}
