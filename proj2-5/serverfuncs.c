#include "serverfuncs.h"

void bind_socket(int sockfd, const struct sockaddr* serveraddr) {
	if(bind(sockfd, serveraddr, sizeof(*serveraddr)) == -1) {
		perror("(bind_socket) bind failed!");
		close(sockfd);
		exit(-1);
	}
}

int name_taken(char* name, struct game* currgame) {
	for(int i = 0; i < currgame->numplayers; i++) if(strstr(name, currgame->players[i].name)) return 1;			
	return 0;
}

void add_player(char* name, struct game* currgame, struct sockaddr* from) {	
	char* msg;
	if(currgame->active) {
		msg = "There is currently a game active! Please try again later.";
	}
	else if(name_taken(name, currgame)) {
		msg = "That name is taken! Try again.";
	} else {
		struct player newplayer;
		name[strlen(name)-1] = '\0';
		newplayer.name = strdup(name);
		newplayer.address = from;
		newplayer.ready = 0;
		newplayer.points = 0;
		currgame->players[currgame->numplayers] = newplayer;
		currgame->numplayers++;
		msg = "You have joined the game!";
	}
	send_message(msg, currgame->sockfd, (const struct sockaddr*)from);
}

void global_message(char* message, struct game* currgame) {
	for(int i = 0; i < currgame->numplayers; i++) send_message(message, currgame->sockfd, (const struct sockaddr*)currgame->players[i].address);
}

void increment_points(struct game* currgame, struct sockaddr* from) {
	for(int i = 0; i < currgame->numplayers; i++) {
		if(strstr(currgame->players[i].address->sa_data, from->sa_data)) currgame->players[i].points++;
	}
}

void* check_answers(void* args) {
	struct game* currgame = (struct game*)args;
	struct sockaddr from = {0};
	while(1) {
		char buf[BUF_SIZE];
		char* reply;
		memset(buf, '\0', sizeof(buf));
		receive_message(currgame->sockfd, buf, &from);
		if(buf[0] == currgame->curranswer || buf[0] == tolower(currgame->curranswer)) {
			increment_points(currgame, &from);
			reply = "\033[0;32mYou got it right! You gained a point!\n";
		} else reply = "\033[0;31mWrong! You get no points.\n";
		send_message(reply, currgame->sockfd, (const struct sockaddr*)&from);
	}
}

void endgame_report(struct game* currgame) {
	char buf[BUF_SIZE];
	memset(buf, '\0', sizeof(buf));
	for(int i = 0; i < currgame->numplayers; i++) {
		strcat(buf, currgame->players[i].name);
		char str[10];
		sprintf(str, " %d\n", currgame->players[i].points);
		strcat(buf, str);
	}
	global_message(buf, currgame);
}

void start_game(struct game* currgame) {
	FILE* f;
	srand(time(0));
	char* files[22] = { "animals", "brain-teasers", "celebrities", "entertainment", "for-kids", "general",
		"geography", "history", "hobbies", "humanities", "literature", "movies", "music", "newest", "people",
		"rated", "religion-faith", "science-technology", "sports", "television", "video-games", "world" };
	char* filename = files[rand() % 22];
	char message[BUF_SIZE] = "\nThe game is starting! You have 15 seconds for each question!\n\nCategory: ";
	strcat(message, filename);
	strcat(message, "\n");
	global_message(message, currgame);
	sleep(5);
    if((f = fopen(filename, "r")) == NULL) {
    	perror("Failed to open file!");
    	exit(-1);
    }
	for(int i = 0; i < NUM_ROUNDS; i++) {
		char msg[BUF_SIZE];
		sprintf(msg, "%d. ", i+1);
    	char line[200];
    	char answer[100];
    	char question[1500];
    	int count = 0;
    	int set = 0;
    	int first = 0;
    	while(1) {
    		fgets(line, 200, f);
    		if(line[0] == '\n') {
    			if(set) break; else continue;
    		}
    		if(!set && strstr(line, "^")) {
				strcpy(answer, line);
				set = 1;
    		} else {
    			strcat(question, line);
    			if(set) {
    				char* temp = strdup(line);
    				char ch = line[0];
    				memmove(temp, temp+2, strlen(temp));
    				if(strstr(answer, temp)) currgame->curranswer = ch;
    			} 
    		}
    		count++;
    	}    	
    	pthread_t thread_id;
    	pthread_create(&thread_id, NULL, check_answers, currgame);
    	strcat(msg, question);
    	global_message(msg, currgame);
    	char msg2[BUF_SIZE] = "Answer: ";
    	memmove(answer, answer+2, strlen(answer));
    	strcat(msg2, answer);
    	sleep(20);
    	pthread_cancel(thread_id);
    	global_message(msg2, currgame);
    	if(i != NUM_ROUNDS-1) sleep(5);
   		memset(msg, '\0', sizeof(msg));
   		memset(msg2, '\0', sizeof(msg2));
   		memset(line, '\0', sizeof(line));
   		memset(answer, '\0', sizeof(answer));
   		memset(question, '\0', sizeof(question));
	}
	fclose(f);
	endgame_report(currgame);
}

void set_ready(char* name, struct game* currgame) {
	for(int i = 0; i < currgame->numplayers; i++) if(strstr(name, currgame->players[i].name)) currgame->players[i].ready = 1;
	//if(currgame->numplayers > 1) {
		int allready = 1;
		for(int i = 0; i < currgame->numplayers; i++) {
			if(!(currgame->players[i].ready)) {
				allready = 0;
				break;
			}
		}
		if(allready) start_game(currgame);
	//}
}

void handle_packets(struct game* currgame) {
	struct sockaddr from = {0};
	char buf[BUF_SIZE];
	memset(buf, '\0', sizeof(buf));
	while(receive_message(currgame->sockfd, buf, &from) > 0) {
		switch(buf[0]) {
			case 'J':
				memmove(buf, buf+1, strlen(buf));
				add_player(buf, currgame, &from);
				break;
			case 'R':
				memmove(buf, buf+1, strlen(buf));
				set_ready(buf, currgame);
				break;
			case 'r':
				memmove(buf, buf+1, strlen(buf));
				set_ready(buf, currgame);
				break;
			default:
				break;
		}
		memset(buf, '\0', sizeof(buf));
	}
}