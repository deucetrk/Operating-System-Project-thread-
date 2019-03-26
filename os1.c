/*
ITCS343 Project 1 Startup Code
Thanapon Noraset
*/
#include <sys/time.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct timespec start;
int E, N;
int *check;
sem_t *device;
int *learningTime;
uint64_t delta_us()
{
    struct timespec current;
    clock_gettime(CLOCK_MONOTONIC_RAW, &current);
    return (current.tv_sec - start.tv_sec) * 1000000 + (current.tv_nsec - start.tv_nsec) / 1000;
}


void *tachikoma(void *arg)
{
    int e;
    int *g2g = (int *)arg;
    int tid = *(int *)arg;
    int i,left,right;
      if(tid == 0) {
          left = N-1;
          right = tid+1; 
           
        } else if(tid == N-1) {
            left = N-2;
            right = 0;
        } else{
              left=tid-1;
              right=tid+1;
            }
    
    while (*g2g != -1)
    {

         if(check[left] ==0 && check[right] == 0) {
              sem_wait(device);
               check[tid] =1;
                 printf("LEARN[%"PRIu64"]: %d healper:%d, %d \n", delta_us(), tid, left, right);
        fflush(stdout);
        e = rand() % E + 1;
        sleep(e);
         learningTime[tid] += e;
        printf("DONE[%"PRIu64"]: %d\n", delta_us(), tid);
        fflush(stdout);
        check[tid] =0;
        sem_post(device);
        
            }
       
    }
    
    if (*g2g == -1) {
        printf("UPDATE[%"PRIu64"]: %d\n", delta_us(), tid);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    srand(42);
    int M, T, i;
    N = atoi(argv[1]); // number of Tachikoma robots
    M = atoi(argv[2]); // number of Simulators
    E = atoi(argv[3]); // max time each learning (in seconds)
    T = atoi(argv[4]); // max time of the program
    pthread_t threads[N];
    int tachikomaId[N];
    
      learningTime = (int *)malloc(sizeof(int) * N);

    device = sem_open("/fortytwo", O_CREAT, 0666, M);
    sem_close(device);
    sem_unlink("/fortytwo");
    device = sem_open("/fortytwo", O_CREAT, 0666, M);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    check = (int *)malloc(sizeof(int) *N);
    int fake[N];
  

    for(i=0; i<N; i++) {
        fake[i]=0;
    }
    for(i=0; i<N; i++) {
        check[i]=fake[i];
    }
    

   
    for (i = 0; i < N; i++)
    {
        tachikomaId[i] = i;
        learningTime[i] = 0;
        pthread_create(&threads[i], NULL, tachikoma, (void *)&tachikomaId[i]);
        sleep(0);
    }

   

    // Wait until time over
    sleep(T);

    // Signal the time over
    for (i = 0; i < N; i++)
    {
        tachikomaId[i] = -1;
    }

    // Wait tachikoma to shutdown
    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }
     for (i = 0; i < N; i++)
    {
        printf("%d\n", learningTime[i]);
    }
    free(learningTime);
    sem_close(device);
    sem_unlink("/fortytwo");

    printf("MASTER: Bye\n");
    return 0;
}
