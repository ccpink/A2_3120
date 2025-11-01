#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_LINE 80

//Commands that need to run in terminal.
void execute_command(char *args[]);

int main(void) {
    int wait_status;
    char usr_command[MAX_LINE];
    const char *delimiter= " ";
    pid_t pid;
    bool in_terminal = true;
    char *token;
    char commands_run[10][128] = {0};
    char recent_commands[10][128] = {0};

    //While loop
    while (in_terminal){
        printf("CSCI3120>");
        fflush(stdout);

        fgets(usr_command, sizeof(usr_command), stdin);

        usr_command[strcspn(usr_command, "\n\r")] = 0;

        char command[MAX_LINE];
        char *args[MAX_LINE/2+1];
        char raw_input[MAX_LINE];
        strcpy(raw_input, usr_command);

        //Tokenize the string
        token = strtok(usr_command, delimiter);
        //separate the singular command and the args from one another
        int i = 0;
        while (token != NULL && i < 31) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL; // null-terminate for execvp()
        //If there is nothing
        if (i == 0) {
            continue;
        }

        strcpy(command, args[0]);


        //If it ends the program stop everything
        if (strcmp(command, "exit") == 0) {
            //end both parent and child
            exit(0);
        }


        pid = fork();
        //Then check everything else

        if (strcmp(command, "history") == 0) {
            if (commands_run[0][0] != '\0') {
                printf("ID\tPID\tCommand\n");
                for (int k = 0; k < 10; k++) {
                    //If there is nothing break loop as there is nothing left.
                    if (commands_run[k][0] == '\0'){
                        break;
                    }
                    printf("%d\t%s\n",k+1, commands_run[k]);
                }

            } else {
                printf("No commands in history\n");

            }
            //Go to next loop
            exit(1);
        }

        if (strcmp(command, "!!") == 0)
            { // Grab the most recent command
                if (recent_commands[0][0] != '\0') {
                    strcpy(usr_command, recent_commands[0]);
                    strcpy(raw_input, usr_command);

                    //Tokenize the string
                    token = strtok(usr_command, delimiter);
                    //separate the singular command and the args from one another
                    i = 0;
                    while (token != NULL && i < 31) {
                        args[i] = token;
                        token = strtok(NULL, " ");
                        i++;
                    }
                    args[i] = NULL; // null-terminate for execvp()
                    //If there is nothing
                    if (i == 0) {
                        exit(1);
                    }

                    strcpy(command, args[0]);

                }else {
                    printf("No commands in history\n");
                    exit(1);
                }
            }
        else if (command[0] == '!' && isdigit(command[1]) )
            { // Grab the Nth command 1-10
                char num[3];
                num[0] = command[1];
                if (command[2] != '\0')  // for two-digit numbers like !10
                {
                    num[1] = command[2];
                    num[2] = '\0';
                }
                else {
                    num[1] = '\0';
                }

                int n = atoi(num);

                if (n>0 && n<=10){
                    if (recent_commands[n-1][0] != '\0') {
                        //testing
                        printf("%s\n", recent_commands[n-1]);

                        strcpy(usr_command, recent_commands[n-1]);
                        strcpy(raw_input, usr_command);

                        //Tokenize the string
                        token = strtok(usr_command, delimiter);
                        //separate the singular command and the args from one another
                        i = 0;
                        while (token != NULL && i < 31) {
                            args[i] = token;
                            token = strtok(NULL, " ");
                            i++;
                        }
                        args[i] = NULL; // null-terminate for execvp()
                        //If there is nothing
                        if (i == 0) {
                            exit(1);
                        }

                        strcpy(command, args[0]);
                    }
                    else {
                        if (recent_commands[0][0] == '\0') {
                            printf("No commands in history\n");
                        }else {
                            printf("Such a command is not in history\n");
                        }
                        exit(1);
                    }

                } else {
                    printf("Input outside bounds!\n");
                    exit(1);
                }


            }

        if (pid == 0) {
            //Execute Command
            execute_command(args);
            printf("Error Never Executed");
        }
        else if (pid < 0)
        {
            /* Error Occured */
            fprintf(stderr, "Fork not executed!\n");
            exit(-1);
        }
        else
            {
            //Wait for child process to finish
            wait(&wait_status);
            if (wait_status == 0) {
                for (int j = 8; j>=0; j--){
                    //Move all commands down one.
                    strcpy(commands_run[j+1], commands_run[j]);
                    strcpy(recent_commands[j+1], recent_commands[j]);

                }
                //Put new command first slot
                snprintf(commands_run[0], sizeof(commands_run[0]), "%lld\t%s", pid, raw_input);
                strcpy(recent_commands[0], raw_input);
            } else {
                printf("There was an error");
            }

        }
    }
    return 0;
}

void execute_command(char *args[]){
    if (execvp(args[0], args) == -1){
        printf("Invalid Command\n");
        exit(-1);
    }
}

