#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define WAIT 0
#define READY 1
#define INPUTTHREAD -1
#define WEIGHTTHREAD -2
#define OUTPUTTHREAD -3

void create_thread(pthread_t *my_thread, int th_type);
void *routine(void *thread_type);
void join_thread(pthread_t *my_thread);
vector<string> split(string statement, char delimeter);
void input_thread();
void weight_thread();
void middle_thread();
void output_thread();


//semaphore_list
sem_t sem_terminal;
sem_t b_sem;
vector<sem_t> in_mid_sem;
vector<sem_t> w_mid_sem;


//input arrays
double bias;
vector<double> weights(128);
vector<double> inputs(128);
vector<int> data_state(128);
vector<int> weight_state(128);
int b_ready = WAIT;
int mid_th_num;

int main(int argc, char const *argv[]) {
  pthread_t neur_in_getter;
  pthread_t neur_w_getter;
  pthread_t neur_out;
  sem_init(&sem_terminal, 0, 1);
  sem_init(&b_sem, 0, 1);

  if(argc != 2) {
    cout << "ERR:Invalid arguments passed.Try again!" << endl;
    exit(-1);
  }
  mid_th_num = atoi(argv[1]);
  vector<pthread_t> mid_threads(mid_th_num);

  for(int i = 0; i < mid_th_num; i++) {
    sem_t new_sem;
    sem_init(&new_sem, 0, 1);
    in_mid_sem.push_back(new_sem);
  }
  for(int i = 0; i < mid_th_num; i++) {
    sem_t new_sem;
    sem_init(&new_sem, 0, 1);
    w_mid_sem.push_back(new_sem);
  }

  for(int i = 0; i < 128; i++) {
    data_state[i] = WAIT;
    weight_state[i] = WAIT;
  }

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

void input_thread() {
  ifstream in_file("inputs.txt");
  string line;
  int count = 0;
  if(in_file.is_open()) {
    getline(in_file, line);
    while(getline(in_file, line)) {
          string token = line.substr(line.find('{')+1);
          token = token.substr(0,token.find('}'));
          while(true){
            string tmp = token.substr(0,token.find(','));
            double num = atof (tmp.c_str());
            weights[count] = num;
            count++;
            token = token.substr(token.find(',')+1);
            if((signed)token.find(',') == -1){
              break;
            }
          }
        }
  }
  else {
    sem_wait (&sem_terminal);
    cout << "ERR: problem in opening the file" << endl;
    sem_post (&sem_terminal);
    exit(-1);
  }
}

void weight_thread() {
  ifstream w_file("weights.txt");
  string line;
  string bias_str;
  int count = 0;
  if(w_file.is_open()) {
    while(getline(w_file, line)) {
          string token = line.substr(line.find('{')+1);
          token = token.substr(0,token.find('}'));
          if((signed)token.find(':')!=-1){
            sem_wait (&b_sem);
            bias_str = token.substr(token.find(':')+1);
            bias = atof (bias_str.c_str());
            b_ready = READY;
            sem_post(&b_sem);
            continue;
          }
          while(true){
            string tmp = token.substr(0,token.find(','));
            double num = atof (tmp.c_str());
            sem_wait (&w_mid_sem[(count+1)/(128/mid_th_num)]);
            weights[count] = num;
            weight_state[count] = READY;
            sem_post (&w_mid_sem[(count+1)/(128/mid_th_num)]);
            count++;
            token = token.substr(token.find(',')+1);
            if((signed)token.find(',') == -1){
              break;
            }
          }
        }
  }
  else {
    sem_wait (&sem_terminal);
    cout << "ERR: problem in opening the file" << endl;
    sem_post (&sem_terminal);
    exit(-1);
  }
}

vector<string> split(string statement, char delimeter) {
	vector<string> result;
	string token;
	for(int i=0; i<statement.length(); i++)
  {
    if(statement[i] != delimeter)
      token += statement[i];
    else if(token.length()) {
      result.push_back(token);
      token = "";
    }
  }
	if(token.length())
		result.push_back(token);

	return result;
}

void middle_thread();
void output_thread();
