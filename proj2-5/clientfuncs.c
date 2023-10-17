#include "clientfuncs.h"

void welcome(int sockfd, struct sockaddr_in* serveraddr) {
	printf("\033[0;36m  ____                                          _   _     _              _____     _       _       \n");
	printf(" / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| | | |   (_)_ __   ___  |_   _| __(_)_   _(_) __ _ \n");
	printf("| |   / _ \\| '_ ` _ \\| '_ ` _ \\ / _` | '_ \\ / _` | | |   | | '_ \\ / _ \\   | || '__| \\ \\ / / |/ _` |\n");
	printf("| |__| (_) | | | | | | | | | | | (_| | | | | (_| | | |___| | | | |  __/   | || |  | |\\ V /| | (_| |\n");
	printf(" \\____\\___/|_| |_| |_|_| |_| |_|\\__,_|_| |_|\\__,_| |_____|_|_| |_|\\___|   |_||_|  |_| \\_/ |_|\\__,_|\033[0m\n\n");
	int accepted = 0;
	char name[50];
	char buf[BUF_SIZE];
	while(!accepted) {
		printf("Enter a username to begin: ");
		fgets(name, 50, stdin);
		char header[BUF_SIZE] = "J";
		strcat(header, name);
		send_message(header, sockfd, (const struct sockaddr*)serveraddr);
		memset(buf, '\0', sizeof(buf));
		receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
		if(buf[0] == 'Y') accepted = 1;
	}
	int ready = 0;
	while(!ready) {
		printf("Enter 'R' when you are ready to start: ");
		char header[BUF_SIZE];
		fgets(header, BUF_SIZE, stdin);
		if(header[0] == 'R' || header[0] == 'r') {
			strcat(header, name);
			send_message(header, sockfd, (const struct sockaddr*)serveraddr);
			ready = 1;
		}
	}
	memset(buf, '\0', sizeof(buf));
	receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
	printf("%s\n", buf);
}

void* get_input(void* args) {
	printf("Enter answer: ");
	fgets((char*)args, BUF_SIZE, stdin);
}

void handle_rounds(int numrounds, int sockfd, struct sockaddr_in* serveraddr) {
	char buf[BUF_SIZE];
	for(int i = 0; i < numrounds; i++) {
		memset(buf, '\0', sizeof(buf));
		receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
		printf("%s\n", buf);
		char answer[BUF_SIZE] = "";
   		pthread_t thread_id;
    	pthread_create(&thread_id, NULL, get_input, answer);
    	int counter = 0;
		while(counter < 15) {
			if(strlen(answer) != 0) break;
			sleep(1);
			counter++;
		}
		pthread_cancel(thread_id);
		send_message(answer, sockfd, (const struct sockaddr*)serveraddr);
		memset(buf, '\0', sizeof(buf));
		receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
		printf("\n%s\n", buf);
		memset(buf, '\0', sizeof(buf));
		receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
		printf("\033[0m%s\n", buf);
	}
	printf("\n --------------\n");
	printf("| \033[0;35mFINAL SCORES \033[0m|\n");
	printf(" --------------\n\n");
	memset(buf, '\0', sizeof(buf));
	receive_message(sockfd, buf, (struct sockaddr*)serveraddr);
	printf("    %s\n", buf);
}