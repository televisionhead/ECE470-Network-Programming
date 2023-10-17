#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include "common.h"

void welcome(int sockfd, struct sockaddr_in* serveraddr);
void handle_rounds(int numrounds, int sockfd, struct sockaddr_in* serveraddr);

#endif