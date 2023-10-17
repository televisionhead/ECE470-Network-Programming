#include "common.h"

//both
int recvLoop(int csoc, char* data, const int size) {
    int nleft, nread;
    char* ptr = data;
    nleft = size;
    while(nleft > 0) {
        nread = recv(csoc, ptr, nleft, 0);
        if(nread < 0) {
            printf("Read error!\n");
            break;
        }
        else if(nread == 0) {
            printf("Socket closed!\n");
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return size-nleft;
}

//client
void cget_balance(int commsoc, char* user) {
    char msg[MESSAGE_SIZE] = "";
    msg[0] = 'B';
    strcat(msg, user);
    send(commsoc, msg, MESSAGE_SIZE, 0);
    recvLoop(commsoc, msg, MESSAGE_SIZE);
    printf("\nYour balance is $%s\n", msg);
        printf("\n");
}

int is_yes(char* input) {
    char* yesarray[2] = { "y", "yes" };
    for(int i = 0; i < 2; i++) if(strcmp(input, yesarray[i]) == 0) return 1;
    return 0;
}

int create_socket() {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo("localhost", NULL, &hints, &res)) {
        printf("getaddrinfo error!\n");
        exit(-1);
    }
    int ret = -1;
    if((ret = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        printf("socket error!\n");
        exit(-1);
    }
    struct sockaddr_in* inaddr = (struct sockaddr_in*)(res->ai_addr);
    inaddr->sin_port = htons(PORT);
    if(connect(ret, res->ai_addr, res->ai_addrlen) < 0) {
        printf("connect error!\n");
        exit(-1);
    }
    return ret;
}

char welcome() {
    char answer[3];
    printf("Welcome to the Digital Wallet System. Do you already have an account? (y/n): ");
    scanf("%s", answer);
    char ret = 'L';
    if(!is_yes(answer)) {
        printf("Would you like to create one? (y/n): ");
        scanf("%s", answer);
        if(!is_yes(answer)) exit(0);
        ret = 'J';
    }
    return ret;
}

int login(char header, int commsoc, char* user, char* pass) {
    strcat(user, " ");
    strcat(user, pass);
    char msg[MESSAGE_SIZE] = "";
    msg[0] = header;
    strcat(msg, user);
    send(commsoc, msg, MESSAGE_SIZE, 0);
    recvLoop(commsoc, msg, MESSAGE_SIZE);
    printf("%s\n", msg);
    if(strstr(msg, "Error")) {
                send(commsoc, "X", MESSAGE_SIZE, 0);
                close(commsoc);
                exit(-1);
        }
        if(msg[0] == 'P') return 1; else return 0;
}

void crequest_payment(int commsoc, char* user) {
    char from[100];
    char amount[100];
    printf("\nWho would you like to request money from?: ");
    scanf("%s", from);
    printf("How much money would you like to request?: ");
    scanf("%s", amount);
    char msg[MESSAGE_SIZE] = "";
    msg[0] = 'R';
    char* str = "P: ";
    strcat(msg, str);
    strcat(msg, user);
    str = " requested $";
    strcat(msg, str);
    strcat(msg, amount);
    str = " from ";
    strcat(msg, str);
    strcat(msg, from);
    send(commsoc, msg, MESSAGE_SIZE, 0);
    recvLoop(commsoc, msg, MESSAGE_SIZE);
    printf("\n%s\n\n", msg);
}

//server
int start_server() {
    struct sockaddr_in sin;
    int ssoc;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((unsigned short)PORT);
    if((ssoc = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error!\n");
        exit(-1);
    }
    if(bind(ssoc, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        printf("bind error!\n");
        exit(-1);
    }
    if(listen(ssoc, 5) < 0) {
        printf("listen error!\n");
        exit(-1);
    }
    return ssoc;
}

FILE* open_file(char* path, char* mode) {
    FILE* ret;
    if((ret = fopen(path, mode)) == NULL) {
        printf("Error opening file\n");
        exit(-1);
    }
    return ret;
}

int user_exists(char* user) {
    FILE* f = open_file("users", "r");
    int curr = 0;
    char line[256];
    while(fgets(line, sizeof(line), f)) {
        if((curr == 0 || (curr % 4) == 0) && strstr(line, user)) return curr+1;
                curr++;
    }
    fclose(f);
    return 0;
}

char* add_user(char* user, char* pass) {
    if(user_exists(user)) return "Error: that username is already taken!\n";
    FILE* f = open_file("users", "a+");
    fputs(user, f);
    fputs("\n", f);
    fputs(pass, f);
    fputs("\n0\n\n", f);
    fclose(f);
    return "Your account has been created with a balance of $0.\n";
}

char* login_user(char* user, char* pass) {
    int linenum;
    if((linenum = user_exists(user)) == 0) return "Error: that username does not exist!\n";
    FILE* f = open_file("users", "r");
    char line[256];
    for(int i = 0; i < linenum+1; i++) fgets(line, sizeof(line), f);
    strcat(pass, "\n");
    if(strcmp(line, pass) != 0) return "Error: password incorrect!\n";
    fclose(f);
    return "You are now logged in.\n";
}

char* get_balance(char* user) {
    FILE* f = open_file("users", "r");
    int curr = 0;
    char* line = malloc(256);
    while(fgets(line, sizeof(line), f)) {
        if((curr == 0 || (curr % 4) == 0) && strstr(line, user)) {
            fgets(line, sizeof(line), f);
            fgets(line, sizeof(line), f);
            break;
        }
        curr++;
    }
    fclose(f);
    return line;
}

double get_balance_f(char* user) {
    FILE* f = open_file("users", "r");
    int curr = 0;
    char line[256];
    while(fgets(line, sizeof(line), f)) {
        if((curr == 0 || (curr % 4) == 0) && strstr(line, user)) {
            fgets(line, sizeof(line), f);
            fgets(line, sizeof(line), f);
            break;
        }
        curr++;
    }
    fclose(f);
        double ret = strtod(line, NULL);
        return ret;
}

char* request_payment(char* msg) {
    char* msgorig = malloc(strlen(msg)+1);
    strcpy(msgorig, msg);
    char* from = strtok(msg, " ");
    for(int i = 0; i < 5; i++) from = strtok(NULL, " ");
    if(!user_exists(from)) return "Error: that user does not exist!";
    FILE* f = open_file("ledger", "a+");
    fputs(msgorig, f);
    fputs("\n", f);
    fclose(f);
    return "Your payment request has been submitted.";
}

char* has_payment_request(char* user) {
    FILE* f = open_file("ledger", "r");
    char line[256];
    while(fgets(line, sizeof(line), f)) {
        if(line[0] == 'P') {
            char* origline = malloc(strlen(line)+1);
            strcpy(origline, line);
            char* name = strtok(line, " ");
            for(int i = 0; i < 5; i++) name = strtok(NULL, " ");
            if(strstr(name, user)) {
                fclose(f);
                return origline;
            }
        }
    }
    fclose(f);
    return NULL;
}

void set_bal(char* user, char* newbal) {
    FILE* r = open_file("users", "r");
    FILE* w = open_file("users_temp", "w");

    char line[256];
    int curr = 0;
    int lineno;
    while(fgets(line, sizeof(line), r)) {
        fputs(line, w);
        if((curr == 0 || (curr % 4) == 0) && strstr(line, user)) {
            fgets(line, sizeof(line), r);
            fputs(line, w);
            fgets(line, sizeof(line), r);
            fputs(newbal, w);
                        fputs("\n", w);
            curr = curr + 2;
        }
        curr++;
    }
    fclose(r);
    remove("users");
    fclose(w);
    rename("users_temp", "users");
}

void update_ledger(char* user, char character) {
    FILE* r = open_file("ledger", "r");
    FILE* w = open_file("ledger_temp", "w");
    char line[256];
    int curr = 0;
    int lineno;
    while(fgets(line, sizeof(line), r)) {
        if(line[0] == 'P') {
                        char* linecpy = malloc(strlen(line)+1);
                        strcpy(linecpy, line);
                        char* name = strtok(linecpy, " ");
                        for(int i = 0; i < 5; i++) name = strtok(NULL, " ");
                        if(strstr(name, user)) line[0] = character;
                        free(linecpy);
        }
                fputs(line, w);
        curr++;
    }
    fclose(r);
    remove("ledger");
    fclose(w);
    rename("ledger_temp", "ledger");
}

char* get_ledger(char* user) {
    char* ret = malloc(MESSAGE_SIZE);
    FILE* f = open_file("ledger", "r");
    char line[256];
    while(fgets(line, sizeof(line), f)) if(strstr(line, user)) strcat(ret, line);
    fclose(f);
    return ret;
}

char* request_refund(char* req) {
    FILE* f = open_file("ledger", "r");
    char line[256];
    int found = 0;
    while(fgets(line, sizeof(line), f)) if(strstr(line, req)) found = 1;
    if(!found) return "A completed ledger as described could not be found!";
    char* from = strtok(req, " ");
    from = strtok(NULL, " ");
    char* amt = strtok(NULL, " ");
    amt = strtok(NULL, " ");
    char* to = strtok(NULL, " ");
    to = strtok(NULL, " ");
    char msg[MESSAGE_SIZE] = "";
    char* str = "P: ";
    strcat(msg, str);
    strcat(msg, to);
    str = " requested ";
    strcat(msg, str);
    strcat(msg, amt);
    str = " from ";
    strcat(msg, str);
    strcat(msg, from);
    return request_payment(msg);
}

char* cancel_payment_request(char* request) {
    FILE* r = open_file("ledger", "r");
    FILE* w = open_file("ledger_temp", "w");
    char line[256];
    int deleted = 0;
    while(fgets(line, sizeof(line), r)) if(strstr(line, request)) deleted = 1; else fputs(line, w);
    fclose(r);
    remove("ledger");
    fclose(w);
    rename("ledger_temp", "ledger");
    if(!deleted) return "A pending transaction as described could not be found!";
    else return "Your pending transaction has been removed from the ledger.";
}