#include "common.h"

int init_socket(struct sockaddr_in* serveraddr, unsigned short int port) {
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("(init) socket failed!");
		exit(-1);
	}
	serveraddr->sin_family = AF_INET;
	serveraddr->sin_port = htons(port);
	serveraddr->sin_addr.s_addr = INADDR_ANY;
	return sockfd;
}

void send_message(const char* msg, int sockfd, const struct sockaddr* address) {
	if(sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, address, sizeof(*address)) == -1) {
		perror("(send_message) sendto failed!");
		close(sockfd);
		exit(-1);
	}
}

socklen_t receive_message(int sockfd, char* buf, struct sockaddr* from) {
	int socklen = sizeof(*from);
	return recvfrom(sockfd, buf, BUF_SIZE, MSG_WAITALL, from, &socklen);
}