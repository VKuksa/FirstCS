#include "win_serv.h"

int main()
{
	Server server;

	if (!server.bind_to_port())
	{
		std::cout << " was the problem" << std::endl;
		exit(1);
	}

	server.work_on_client(0);
}
