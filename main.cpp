#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>

using namespace std;



#define INPUTTHREAD -1
#define WEIGHTTHREAD -2
#define OUTPUTTHREAD -3

void create_thread(pthread_t *my_thread, int th_type);
void *routine(void *thread_type);
void join_thread(pthread_t *my_thread);
void input_thread();
void wait_thread();
void middle_thread();
void output_thread();

//semaphore_list
sem_t sem_terminal;


int main(int argc, char const *argv[]) {
  int mid_th_num;
  pthread_t neur_in_getter;
  pthread_t neur_w_getter;
  pthread_t neur_out;
  sem_init(&sem_terminal, 0, 1);

  if(argc != 2) {
    cout << "ERR:Invalid arguments passed.Try again!" << endl;
    exit(-1);
  }
  mid_th_num = atoi(argv[1]);
  vector<pthread_t> mid_threads(mid_th_num);

  //create all threads first
  create_thread(&neur_in_getter, INPUTTHREAD);
  create_thread(&neur_w_getter, WEIGHTTHREAD );
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


   if(th_type == INPUTTHREAD){
     sem_wait (&sem_terminal);
     cout << "LOG:Input-getter thread has been created." << endl;
     sem_post (&sem_terminal);

   }

   else if(th_type == WEIGHTTHREAD) {
     sem_wait (&sem_terminal);
     cout << "LOG:Weight-getter thread has been created." << endl;
     sem_post (&sem_terminal);
   }

  else if(th_type == OUTPUTTHREAD) {
    sem_wait (&sem_terminal);
    cout << "LOG:Output thread has been created." << endl;
    sem_post (&sem_terminal);
  }

   else {
     sem_wait (&sem_terminal);
     cout << "LOG:Middle thread " << th_type << " has been created." << endl;
     sem_post (&sem_terminal);
   }

   pthread_exit(NULL);
}

void join_thread(pthread_t *my_thread) {
  if (pthread_join(*my_thread, NULL) != 0)
        cout << "ERR:unable to join thread" << endl;
}
