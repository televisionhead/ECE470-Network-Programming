#include "serverfuncs.h"

int main() {
	struct game currgame;
	struct sockaddr_in serveraddr = {0};
	currgame.sockfd = init_socket(&serveraddr, PORT_NUM);	
	bind_socket(currgame.sockfd, (const struct sockaddr*)&serveraddr);
	currgame.address = &serveraddr;
	currgame.numplayers = 0;
	currgame.active = 0;

	handle_packets(&currgame);

    close(currgame.sockfd);
    return 0;
}