#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define NUM_THREADS 64
#define NUM_CPUS 8

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
pthread_t mainthread;
threadParams_t threadParams[NUM_THREADS];


void *counterThread(void *threadp)
{
    int sum=0, i, rc, cpuidx;
    cpu_set_t cpuset;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    pthread_t mythread;

    mythread = pthread_self();
  
    CPU_ZERO(&cpuset);
    cpuidx=(threadParams->threadIdx)%NUM_CPUS;
    CPU_SET(cpuidx, &cpuset);

    rc = pthread_setaffinity_np(mythread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        perror("pthread_setaffinity_np");


    for(i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;

    rc = pthread_getaffinity_np(mythread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        perror("pthread_getaffinity_np");

    printf("\nThread idx=%d, sum[0...%d]=%d, running on CPU=%d, CPUs =", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu());

       for (cpuidx = 0; cpuidx < CPU_SETSIZE; cpuidx++)
           if (CPU_ISSET(cpuidx, &cpuset))
               printf(" %d", cpuidx);
}


int main (int argc, char *argv[])
{
   int rc;
   int i, j, cpuidx;
   cpu_set_t cpuset;

   CPU_ZERO(&cpuset);

   // get affinity set for main thread
   mainthread = pthread_self();

   // Check the affinity mask assigned to the thread 
   rc = pthread_getaffinity_np(mainthread, sizeof(cpu_set_t), &cpuset);
   if (rc != 0)
       perror("pthread_getaffinity_np");
   else
   {
       printf("running on CPU=%d, CPUs =", sched_getcpu());

       for (j = 0; j < CPU_SETSIZE; j++)
           if (CPU_ISSET(j, &cpuset))
               printf(" %d", j);

       printf("\n");
   }

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
