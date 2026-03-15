#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#ifndef BUF_SIZE /* Allow "cc -D" to override definition */
#define BUF_SIZE 10
#endif

//========ITERATE determines number of system calls
#define ITERATE  100 // Total BUF_SIZE * ITERATE 10,0,00,000 (10 million Characters)


int main (int argc, char *argv[]) {
	int n, fd, openFlags; 
    long i;
	mode_t filePerms; 
    pid_t x;
	char *t=(char*)"0",buf[BUF_SIZE];   //  user space buffer

	if(argc<1) {
        printf("\n\t Insufficient parameters...\n"); 
        exit(EXIT_FAILURE); 
    }
   //=================File opening 
   openFlags = O_CREAT | O_WRONLY | O_TRUNC;
   filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */
   fd = open(argv[1], openFlags, filePerms);
   if (fd == -1)
		printf("\n ERROR: opening file %s", argv[1]);

    //=================Writing in user space buffer 
    buf[0]='\0';
    for(i=0;i<BUF_SIZE;i++)  // buffer size 1000
    	strcat(buf,t);
    //printf("\n Test string %s size=%ld", buf,strlen(buf));
   
   //=================Writing in kernel space buffer  
    for(i=0;i<ITERATE;i++) { // 100000; 100 million bytes; 100,0,00,000
    	if (write(fd, buf, BUF_SIZE) != strlen(buf))
			printf("\n ERROR: couldn't write whole buffer");

	    //============ Immediate data sync to disk
		//fsync(fd);  // Option 1: calling fsync() along with data would also force the metadata to be updated (sync) to disk; two disk operations required
		//fdatasync(fd);  // Option 2: calling fdatasync() only forces the data to be updated (sync) to disk; one disk operation required
		//sync(fd);
	}
    
    if (close(fd) == -1)
		printf("\n ERROR: close input");

	//for(;;){}
	//printf("\n\t Hello\n");
	
}