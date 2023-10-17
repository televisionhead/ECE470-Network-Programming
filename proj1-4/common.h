#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 2001
#define MESSAGE_SIZE 500

//both
int recvLoop(int csoc, char* data, const int size);

//client
void cget_balance(int commsoc, char* user);
int is_yes(char* input);
int create_socket();
char welcome();
int login(char header, int commsoc, char* user, char* pass);
void crequest_payment(int commsoc, char* user);

//server
int start_server();
FILE* open_file(char* path, char* mode);
int user_exists(char* user);
char* add_user(char* user, char* pass);
char* login_user(char* user, char* pass);
char* request_payment(char* msg);
char* get_balance(char* user);
double get_balance_f(char* user);
char* has_payment_request(char* user);
void set_bal(char* user, char* newbal);
void update_ledger(char* user, char character);
char* get_ledger(char* user);
char* request_refund(char* req);
char* cancel_payment_request(char* request);

#endif