#ifndef SERVERFUNCS_H
#define SERVERFUNCS_H

#include "common.h"

struct player {
	const char* name;
	struct sockaddr* address;
	int ready;
	int points;
};

struct game {
	int sockfd;
	struct sockaddr_in* address;

	int numplayers;
	struct player players[1000];
	int active;

	char curranswer;
};

void bind_socket(int sockfd, const struct sockaddr* serveraddr);
int name_taken(char* name, struct game* currgame);
void add_player(char* name, struct game* currgame, struct sockaddr* from);
void global_message(char* message, struct game* currgame);
void increment_points(struct game* currgame, struct sockaddr* from);
void* check_answers(void* args);
void endgame_report(struct game* currgame);
void start_game(struct game* currgame);
void set_ready(char* name, struct game* currgame);
void handle_packets(struct game* currgame);

#endif