/* copied from : http://www.csc.villanova.edu/~mdamian/threads/badcnt.txt */ 

#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define NITER 1000000

int cnt = 0;

static sem_t* pmutex; 

void * Count(void * a)
{
    int i, tmp;
    for(i = 0; i < NITER; i++)
    {
        sem_wait(pmutex); 
        tmp = cnt;      /* copy the global cnt locally */
        tmp = tmp+1;    /* increment the local copy */
        cnt = tmp;      /* store the local value into the global cnt */ 
        sem_post(pmutex); 
    }
    return NULL; 
}

int main(int argc, char * argv[])
{
    int initval=1; 
    int oflag = O_CREAT; //|O_EXCL; 
    mode_t mode=S_IRUSR | S_IWUSR; 
    pthread_t tid1, tid2;

    pmutex=sem_open("/rafs2sem1", oflag ,mode, initval); 
    if ( pmutex == SEM_FAILED ) {
      printf("Error opening semaphore: %d = %s\n",errno,strerror(errno)); 
      exit(1) ;
    } 
    if(pthread_create(&tid1, NULL, Count, NULL))
    {
      printf("\n ERROR creating thread 1");
      exit(1);
    }

    if(pthread_create(&tid2, NULL, Count, NULL))
    {
      printf("\n ERROR creating thread 2");
      exit(1);
    }

    if(pthread_join(tid1, NULL))	/* wait for the thread 1 to finish */
    {
      printf("\n ERROR joining thread");
      exit(1);
    }

    if(pthread_join(tid2, NULL))        /* wait for the thread 2 to finish */
    {
      printf("\n ERROR joining thread");
      exit(1);
    }

    if (cnt < 2 * NITER) 
        printf("\n BOOM! cnt is [%d], should be %d\n", cnt, 2*NITER);
    else
        printf("\n OK! cnt is [%d]\n", cnt);
    
    sem_close(pmutex);  
    pthread_exit(NULL);
}

