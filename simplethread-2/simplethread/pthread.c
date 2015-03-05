#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 64

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


void *counterThread(void *threadp)
{
    int sum=0, i, rc;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;
 
    printf("\nThread idx=%d, sum[0...%d]=%d, running on CPU=%d", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu());
}


int main (int argc, char *argv[])
{
   int rc;
   int i;
   cpu_set_t cpuset;
   pthread_t mythread;

   mythread = pthread_self();

   rc = pthread_getaffinity_np(mythread, sizeof(cpu_set_t), &cpuset);
   if (rc != 0)
       perror("pthread_getaffinity_np");

   printf("CPU mask includes:");
   for (i = 0; i < CPU_SETSIZE; i++)
       if (CPU_ISSET(i, &cpuset))
           printf(" %d", i);
   printf("\n");
   
   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );

   }

   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("\nTEST COMPLETE\n");
}
