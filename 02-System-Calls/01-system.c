#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *const argv[]) {

    system("ls >> 01-system.txt");
    system("cat 01-system.txt");

    return EXIT_SUCCESS;
}