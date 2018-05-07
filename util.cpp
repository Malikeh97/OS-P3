#include "util.h"

using namespace std;

sem_t* my_mutex;

void create_thread(pthread_t *my_thread, int th_type, sem_t* mutex) {
  my_mutex = mutex;
  int rc = pthread_create(my_thread, NULL, routine, (void *)th_type);
  if (rc) {
    cout << "Error:unable to create thread," << rc << endl;
    exit(-1);
  }
}

void *routine(void *thread_type) {
   long th_type;
   th_type = (long)thread_type;


   if(th_type == INPUTTHREAD){
     sem_wait (my_mutex);
     cout << "LOG:Input-getter thread has been created." << endl;
     sem_post (my_mutex);
   }

   else if(th_type == WEIGHTTHREAD) {
     cout << "LOG:Weight-getter thread has been created." << endl;
   }

  else if(th_type == OUTPUTTHREAD) {
    sem_wait (my_mutex);
    cout << "LOG:Output thread has been created." << endl;
    sem_post (my_mutex);
  }

   else {
     sem_wait (my_mutex);
     cout << "LOG:Middle thread " << th_type << " has been created." << endl;
     sem_post (my_mutex);
   }

   pthread_exit(NULL);
}

void join_thread(pthread_t *my_thread) {
  if (pthread_join(*my_thread, NULL) != 0)
        cout << "ERR:unable to join thread" << endl;
}
