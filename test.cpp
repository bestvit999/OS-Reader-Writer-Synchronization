#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

// data set
sem_t wrt = 1;
sem_t mux = 1;
int readcount = 0;

// reader & writer
void *reader(void *argc);
void *writer(void *argc);

int main()
{
    pthread_t my_thread[5];

    pthread_create(&my_thread[1], NULL, &reader, (void *)1);
    pthread_create(&my_thread[2], NULL, &reader, (void *)2);
    pthread_create(&my_thread[3], NULL, &reader, (void *)3);
    pthread_create(&my_thread[4], NULL, &writer, (void *)4);
    pthread_create(&my_thread[5], NULL, &reader, (void *)5);


    pthread_exit(NULL);
}

void *reader(void *argc)
{
    sem_wait(&mux);
    readcount++;
    if (readcount == 1)
    {
        // somebody is reading, so writer need to wait.
        sem_wait(&wrt);
    }
    sem_post(&mux);
    // critical section //
    printf("read : %d\n",argc);
    // end of critical section //

    // remain section //
    sem_wait(&mux);
    readcount--;
    if (readcount == 0)
    {
        sem_post(&wrt);
    }
    sem_post(&mux);
}

void *writer(void *argc)
{
    sem_wait(&wrt);

    /* critical section */
    printf("write : %d\n",argc);
    /* end of critical section */

    // remain section //
    sem_post(&wrt);
}