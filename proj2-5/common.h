#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#define PORT_NUM 5000
#define BUF_SIZE 2000
#define NUM_ROUNDS 3

int init_socket(struct sockaddr_in* serveraddr, unsigned short int port);
void send_message(const char* msg, int sockfd, const struct sockaddr* address);
socklen_t receive_message(int sockfd, char* buf, struct sockaddr* from);

#endif