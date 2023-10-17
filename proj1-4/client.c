#include "common.h"

int main() {
        char header = welcome();
        int commsoc = create_socket();
        int choice;

        char user[100];
    char pass[100];
    printf("\nPlease enter your username: ");
    scanf("%s", user);
    printf("Please enter your password: ");
    scanf("%s", pass);
        printf("\n");

        char* username = malloc(strlen(user)+1);
        strcpy(username, user);

        int pending = login(header, commsoc, user, pass);

        while(1) {
                printf("1. Get current balance\n");
                printf("2. Request a payment\n");
                printf("3. Request a refund\n");
                printf("4. Print transactions\n");
                printf("5. Pay current pending request\n");
                printf("6. Refuse current pending request\n");
                printf("7. Cancel a pending payment request\n");
                printf("8. Logout\n");
                printf("\nWhich operation would you like to do? (1,2,3,4,5,6,7,8): ");

                scanf("%d", &choice);

                switch(choice) {
                        case 1:
                                cget_balance(commsoc, username);
                                break;
                        case 2:
                                crequest_payment(commsoc, username);
                                break;
                        case 3:
                                printf("\nWho would you like to request a refund from?: ");
                                char from[100];
                                char amt[50];
                                scanf("%s", from);
                                printf("What was the amount of this transaction?: ");
                                scanf("%s", amt);
                                char msg3[MESSAGE_SIZE] = "";
                        msg3[0] = 'G';
                        char* str = "C: ";
                        strcat(msg3, str);
                        strcat(msg3, from);
                        str = " requested $";
                        strcat(msg3, str);
                        strcat(msg3, amt);
                        str = " from ";
                        strcat(msg3, str);
                        strcat(msg3, username);
                                send(commsoc, msg3, MESSAGE_SIZE, 0);
                                recvLoop(commsoc, msg3, MESSAGE_SIZE);
                                printf("\n%s\n\n", msg3);
                                break;
                        case 4:
                                printf("");
                                char msgg[MESSAGE_SIZE] = "";
                                msgg[0] = 'T';
                                strcat(msgg, username);
                                send(commsoc, msgg, MESSAGE_SIZE, 0);
                                recvLoop(commsoc, msgg, MESSAGE_SIZE);
                                printf("\n%s\n", msgg);
                                break;
                        case 5:
                                pending = login(header, commsoc, user, pass);
                                if(!pending) {
                                        printf("You have no pending transactions!\n\n");
                                        break;
                                }
                                char msg[MESSAGE_SIZE] = "";
                                msg[0] = 'P';
                                strcat(msg, username);
                                send(commsoc, msg, MESSAGE_SIZE, 0);
                                recvLoop(commsoc, msg, MESSAGE_SIZE);
                                printf("%s\n", msg);
                                break;
                        case 6:
                                pending = login(header, commsoc, user, pass);
                                if(!pending) {
                                        printf("You have no pending transactions!\n\n");
                                        break;
                                }
                                char msg2[MESSAGE_SIZE] = "";
                                msg2[0] = 'N';
                                strcat(msg2, username);
                                send(commsoc, msg2, MESSAGE_SIZE, 0);
                                recvLoop(commsoc, msg2, MESSAGE_SIZE);
                                printf("%s\n", msg2);
                                break;
                        case 7:
                                printf("\nWho did you request payment from that you would like to cancel?: ");
                                char fromm[100];
                                char amount[50];
                                scanf("%s", fromm);
                                printf("What was the amount of this payment request?: ");
                                scanf("%s", amount);
                                char msg4[MESSAGE_SIZE] = "";
                                msg4[0] = 'C';
                                char* strr = "P: ";
                                strcat(msg4, strr);
                                strcat(msg4, username);
                                strr = " requested $";
                                strcat(msg4, strr);
                                strcat(msg4, amount);
                                strr = " from ";
                                strcat(msg4, strr);
                                strcat(msg4, fromm);
                                send(commsoc, msg4, MESSAGE_SIZE, 0);
                                recvLoop(commsoc, msg4, MESSAGE_SIZE);
                                printf("\n%s\n\n", msg4);
                                break;
                        default:
                                send(commsoc, "X", MESSAGE_SIZE, 0);
                                close(commsoc);
                                return 0;
                                break;
                }
        }

        free(username);
        close(commsoc);
        return 0;
}