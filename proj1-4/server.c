#include "common.h"

void listen_loop(int ssoc) {
        struct sockaddr_in fsin;
        int size = sizeof(fsin);
        int commsoc;
        char buf[MESSAGE_SIZE];
        char* response = malloc(MESSAGE_SIZE);
        char* user, *pass;

        NEWCONNECTION:

        if((commsoc = accept(ssoc, (struct sockaddr*)&fsin, &size)) < 0) {
                printf("accept error!\n");
                exit(-1);
        }

        while(1) {
                response = "";
                recvLoop(commsoc, buf, MESSAGE_SIZE);

                switch(buf[0]) {
                        case 'C':
                                memmove(buf, buf+1, strlen(buf));
                                response = cancel_payment_request(buf);
                                break;
                        case 'G':
                                memmove(buf, buf+1, strlen(buf));
                                response = request_refund(buf);
                                break;
                        case 'T':
                                memmove(buf, buf+1, strlen(buf));
                                response = get_ledger(buf);
                                break;
                        case 'N':
                                memmove(buf, buf+1, strlen(buf));
                                update_ledger(buf, 'X');
                                response = "You have denied a pending transaction. The ledger has been updated.\n";
                                break;
                        case 'J':
                                memmove(buf, buf+1, strlen(buf));
                                user = strtok(buf, " ");
                                pass = strtok(NULL, " ");
                                response = add_user(user, pass);
                                break;
                        case 'L':
                                memmove(buf, buf+1, strlen(buf));
                                user = strtok(buf, " ");
                                pass = strtok(NULL, " ");
                                response = login_user(user, pass);
                                char* request;
                                if(response[0] == 'Y' && (request = has_payment_request(user))) response = request;
                                break;
                        case 'B':
                                memmove(buf, buf+1, strlen(buf));
                                char strbal[50];
                                snprintf(strbal, 50, "%f", get_balance_f(buf));
                                response = strbal;
                                break;
                        case 'R':
                                memmove(buf, buf+1, strlen(buf));
                                response = request_payment(buf);
                                break;
                        case 'P':
                                memmove(buf, buf+1, strlen(buf));
                                char* req = has_payment_request(user);
                                char* reqcopy = malloc(strlen(req)+1);
                                strcpy(reqcopy, req);
                                char* to = strtok(reqcopy, " ");
                                to = strtok(NULL, " ");
                                double bal = get_balance_f(buf);
                                char* requestbalance = strtok(req, " ");
                                for(int i = 0; i < 3; i++) requestbalance = strtok(NULL, " ");
                                requestbalance++;
                                double balreq = strtod(requestbalance, NULL);
                                if(balreq > bal) {
                                        response = "You do not have enough money in your account to make that payment!\n";
                                        break;
                                }
                                double res = bal-balreq;
                                char param[50];
                                snprintf(param, 50, "%f", res);
                                set_bal(user, param);
                                double floaty = get_balance_f(to);
                                floaty += balreq;
                                snprintf(param, 50, "%f", floaty);
                                set_bal(to, param);
                                update_ledger(user, 'C');
                                response = "You have paid the current pending transaction!\n";
                                break;
                        case 'X':
                                goto NEWCONNECTION;
                                break;
                }

                send(commsoc, response, MESSAGE_SIZE, 0);
        }
        close(commsoc);
}

int main() {
        int ssoc = start_server();

        listen_loop(ssoc);

        return 0;
}