#include "stdio.h"     // printf scanf
#include "unistd.h"    // slepp
#include "sys/types.h" // pid_t
#include "sys/wait.h"  // wait
#include "signal.h"    // signal kill
#include "stdlib.h"    // exit

int main(int argc, char* const argv[]) {
    pid_t child_pid, wpid;
    int status = 0;

    // Father code (before child processes start)

    for (int id = 0; id < 5; id++) {
        if ((!fork())) { // child code
            sleep((5 - id) * 2);
            printf("\tChild running, PID: [%d]\n", getpid());
            exit(id + 1);
        }
    }

    wpid = wait(&status); // [A] this way parent WAITS for ANY ONE of the child process to exit

    // while ((wpid = wait(&status)) > 0); // [B} this way, the parent WAITS for ALL IT's child processes to exit

    sleep(1);
    printf("\tParent, PID: [%d], synchronized with child death (ret Child Id: [%d] ....ret Status: [%d] )\n", getpid(), wpid, status);

    return EXIT_SUCCESS;
}