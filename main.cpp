#include "thread.h"

using namespace std;

int main(int argc, char const *argv[]) {
  int mid_th_num;
  InputThread* neur_in_getter;
  WeightThread* neur_w_getter;
  OutputThread* neur_out;

  if(argc != 2) {
    cout << "ERR:Invalid arguments passed.Try again!" << endl;
    exit(-1);
  }

  mid_th_num = atoi(argv[1]);
  vector<MiddleThread*> mid_threads(mid_th_num);

  //create all threads first
  neur_in_getter = new InputThread();
  neur_w_getter = new WeightThread();
  for(int i = 0; i < mid_th_num; i++) {
    mid_threads[i] = new MiddleThread(i);
  }
  neur_out = new OutputThread();

  //join all thread to guarantee termination
  neur_in_getter->join_thread();
  neur_w_getter->join_thread();
  for(int i = 0; i < mid_th_num; i++) {
    mid_threads[i]->join_thread();
  }
  neur_out->join_thread();

  return 0;
}

// int main(int argc, char const *argv[]) {
//   int mid_th_num;
//   pthread_t neur_in_getter;
//   pthread_t neur_w_getter;
//   pthread_t neur_out;
//
//   if(argc != 2) {
//     cout << "ERR:Invalid arguments passed.Try again!" << endl;
//     exit(-1);
//   }
//   mid_th_num = atoi(argv[1]);
//   vector<pthread_t> mid_threads(mid_th_num);
//
//   sem_t mutex;//test
//   sem_init(&mutex, 0, 1);//test
//   sem_t mutex2;//test
//   sem_init(&mutex2, 0, 1);//test
//
//   //create all threads first
//   create_thread(&neur_in_getter, INPUTTHREAD, &mutex);
//   create_thread(&neur_w_getter, WEIGHTTHREAD , &mutex);
//   for(int i = 0; i < mid_th_num; i++) {
//     create_thread(&mid_threads[i], i,  &mutex);
//   }
//   create_thread(&neur_out, OUTPUTTHREAD, &mutex);
//
//
//
//   //join all thread to guarantee termination
//   join_thread(&neur_in_getter);
//   join_thread(&neur_w_getter);
//   for(int i = 0; i < mid_th_num; i++) {
//     join_thread(&mid_threads[i]);
//   }
//   join_thread(&neur_out);
//
//   return 0;
// }
