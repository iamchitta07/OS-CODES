#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/stat.h"

int main(int argc, char* const argv[]) {
    mode_t modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
    int fd = open("02-basic.txt", O_CREAT | O_WRONLY | O_APPEND, modes);

    char msg[] = "Hello! User!!\n";

    write(fd, msg, sizeof(msg)-1);
    close(fd);

    return EXIT_SUCCESS;
}