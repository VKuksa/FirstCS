#include "win_clnt.h"

int main(int argc, char* argv[])
{
	std::cout << "TCP Client" << std::endl;

	Client client;

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(client.get_port());

	if (!client.connection_to_port(dest_addr) == 0)
	{
		std::cout << " was the problem" << std::endl;
		exit(1);
	}

	client.starting_response();

	client.dialog_with_server(dest_addr);

	return -1;
}
