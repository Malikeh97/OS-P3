#include "util.h"

using namespace std;

//https://www.tutorialspoint.com/cplusplus/cpp_multithreading.htm
void create_thread(pthread_t *my_thread, int th_type) {
  int rc = pthread_create(my_thread, NULL, routine, (void *)th_type);
  if (rc) {
    cout << "Error:unable to create thread," << rc << endl;
    exit(-1);
  }
}

void *routine(void *thread_type) {
   long th_type;
   th_type = (long)thread_type;
   if(th_type == INPUTTHREAD)
      cout << "LOG:Input-getter thread has been created." << endl;
   else if(th_type == WEIGHTTHREAD)
      cout << "LOG:Weight-getter thread has been created." << endl;
  else if(th_type == OUTPUTTHREAD)
      cout << "LOG:Output thread has been created." << endl;
   else
      cout << "LOG:Middle thread " << th_type << " has been created." << endl;
   pthread_exit(NULL);
}

void join_thread(pthread_t *my_thread) {
  if (pthread_join(*my_thread, NULL) != 0)
        cout << "ERR:unable to join thread" << endl;
}
