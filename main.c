#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(void) {
    int execlp_status;
    int wait_status;
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        execlp_status = execlp("ls", "ls", "-1", NULL);
        if (execlp_status == -1) {
            printf("Error executing execlp\n");
            exit(1);
        }
    }else if (pid < 0) { /* Error Occured */
        fprintf(stderr, "Error forking\n");
    } else {
        wait(&wait_status);
        printf("Child Complete");
    }


    return 0;
}