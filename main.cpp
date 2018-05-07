#include "util.h"

using namespace std;
int main(int argc, char const *argv[]) {
  int mid_th_num;
  pthread_t neur_in_getter;
  pthread_t neur_w_getter;
  pthread_t neur_out;

  if(argc != 2) {
    cout << "ERR:Invalid arguments passed.Try again!" << endl;
    exit(-1);
  }
  mid_th_num = atoi(argv[1]);
  vector<pthread_t> mid_threads(mid_th_num);

  //create all threads first
  create_thread(&neur_in_getter, INPUTTHREAD);
  create_thread(&neur_w_getter, WEIGHTTHREAD);
  for(int i = 0; i < mid_th_num; i++) {
    create_thread(&mid_threads[i], i);
  }
  create_thread(&neur_out, OUTPUTTHREAD);

  //join all thread to guarantee termination 
  join_thread(&neur_in_getter);
  join_thread(&neur_w_getter);
  for(int i = 0; i < mid_th_num; i++) {
    join_thread(&mid_threads[i]);
  }
  join_thread(&neur_out);

  return 0;
}
