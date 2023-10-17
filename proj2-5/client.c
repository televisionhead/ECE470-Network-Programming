#include "clientfuncs.h"

int main() {
	struct sockaddr_in serveraddr = {0};
	int sockfd = init_socket(&serveraddr, PORT_NUM);

	welcome(sockfd, &serveraddr);

	handle_rounds(NUM_ROUNDS, sockfd, &serveraddr);

	close(sockfd);
    return 0; 
}