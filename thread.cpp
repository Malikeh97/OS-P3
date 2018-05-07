#include "thread.h"

using namespace std;

Thread::Thread() {
  int rc = pthread_create(pth, NULL, routine, NULL);
  if (rc) {
    cout << "Error:unable to create thread," << rc << endl;
    exit(-1);
  }
}

void Thread::join_thread() {
  if (pthread_join(*pth, NULL) != 0)
        cout << "ERR:unable to join thread" << endl;
}


// void* InputThread::routine(void*) {
//
// }
//
// void* WeightThread::routine(void*) {
//
// }
//
// void* MiddleThread::routine(void*) {
//
// }
//
// void* OutputThread::routine(void*) {
//
// }
