#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

pthread_mutex_t lock;
FILE *f1;

void* resolveRace(void* args) {
    bool* odd = (bool*)args;
    pthread_mutex_lock(&lock);
        int x;
        f1 = fopen("02-acc_bal.txt", "r");
        fscanf(f1, "bal: %d", &x);
        fclose(f1);
        
        if (*odd) x++; 
        else x+=2;

        f1 = fopen("02-acc_bal.txt", "w+");
        fprintf(f1, "bal: %d", x);
        fclose(f1);
    pthread_mutex_unlock(&lock);
    free(odd);
    return NULL;
}

int main(int argc, char* const argv[]) {
    pthread_t threads[500];
    pthread_mutex_init(&lock, NULL);

    for(int i=0;i<500;i++) {
        bool *odd = (bool*)malloc(sizeof(bool));
        *odd = i&1;
        pthread_create((threads+i), NULL, resolveRace, (void*)odd); 
    }
    
    for(int i=0;i<500;i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&lock);

    printf("Code Executed!!\n");
    return EXIT_SUCCESS;
}