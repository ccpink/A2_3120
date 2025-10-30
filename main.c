#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_LINE 80

//Commands that need to run in terminal.
void execute_command(char *args[]);

int main(void) {
    int execlp_status;
    int wait_status;
    char usr_command[MAX_LINE];
    const char *delimiter= " ";
    pid_t pid;
    bool in_terminal = true;
    char *token;
    char recent_cmds[10][128] = {0};

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
        if (strcmp(command, "end") == 0) {
            //end both parent and child
            exit(0);
        }
        if (strcmp(command, "history") == 0) {
            if (recent_cmds[0][0] == '\0') {
                printf("ID\tPID\tCommand\n");
                for (int k = 0; k < 10; k++) {
                    //If there is nothing break loop as there is nothing left.
                    if (recent_cmds[k][0] == '\0'){
                        break;
                    }
                    printf("%d\t%s\n",k+1, recent_cmds[k]);
                }

            }
            //Go to next loop
            continue;
        }

        pid = fork();
        //Then check everything else

        if (pid == 0) {
            //Execute Command
            execute_command(args);
            printf("child never exited");
        }
        else if (pid < 0)
        {
            /* Error Occured */
            fprintf(stderr, "Error forking\n");
            exit(-1);
        }
        else
        {
            //Wait for child process to finish
            wait(&wait_status);
            if (wait_status == 0) {
                printf("Child Complete");
                for (int j = 8; j>=0; j--){
                    //Move all commands down one.
                    strcpy(recent_cmds[j+1], recent_cmds[j]);
                }
                //Put new command first slot
                snprintf(recent_cmds[0], sizeof(recent_cmds[0]), "%lld\t%s", pid, raw_input);
            } else {
                printf("There was an error");

            }

        }
    }
    return 0;
}

void execute_command(char *args[]){
    if (execvp(args[0], args) == -1){
        perror("execvp failed");
        exit(-1);
    }
}

