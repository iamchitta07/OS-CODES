#include "unistd.h"
#include "sys/wait.h"
#include "sys/types.h"

int main() {

    pid_t pid = fork();

    pid != 0 ? wait(NULL) : execl("/bin/ls", "ls", NULL);;
    
    return 0;
}