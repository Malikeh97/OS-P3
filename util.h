#ifndef __util__
#define __util__

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>

#define INPUTTHREAD -1
#define WEIGHTTHREAD -2
#define OUTPUTTHREAD -3


void create_thread(pthread_t *my_thread, int th_type);
void *routine(void *thread_type);
void join_thread(pthread_t *my_thread);

#endif
