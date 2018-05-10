#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>
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
void input_thread();
void weight_thread();
void middle_thread(long th_type);
void output_thread();
void log(string s);

//semaphore_list
sem_t sem_terminal;//4
sem_t b_sem;//3
vector<sem_t> data_r_mid_sem;//1
vector<sem_t> w_mid_sem;//2
vector<sem_t> out_done;//
vector<sem_t> create_mids;//


//input arrays
double bias;
vector<double> weights(128);
vector<double> inputs(128);
vector<double> sum_list;
int mid_th_num;
bool input_complete = false;
double result = 0.0;
int t = 0;


//flags
vector<int> data_ready;//1
vector<int> weight_ready;//2
vector<bool> out_completed;
vector<bool> computed;
bool b_ready = false;//2

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
    w_mid_sem.push_back(new_sem);
  }

  for(int i = 0; i < mid_th_num; i++) {
    sem_t new_sem;
    sem_init(&new_sem, 0, 1);
    data_r_mid_sem.push_back(new_sem);
  }

  for(int i = 0; i < mid_th_num; i++) {
    sem_t new_sem;
    sem_init(&new_sem, 0, 1);
    out_done.push_back(new_sem);
  }

  for(int i = 0; i < mid_th_num + 2; i++) {
    sem_t new_sem;
    sem_init(&new_sem, 0, 1);
    create_mids.push_back(new_sem);
  }

  for(int i = 0; i < mid_th_num; i++) {
    sum_list.push_back(0.0);
  }

  for (int i = 0; i < mid_th_num;i++) {
    data_ready.push_back(WAIT);
    weight_ready.push_back(WAIT);
    computed.push_back(false);
    out_completed.push_back(false);
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
     sem_wait(&create_mids[0]);
     input_thread();
   }

   else if(th_type == WEIGHTTHREAD) {
     sem_wait (&sem_terminal);
     cout << "LOG:Weight-getter thread has been created." << endl;
     sem_post (&sem_terminal);
     sem_wait(&create_mids[1]);
     sem_post(&create_mids[0]);
     weight_thread();
   }

  else if(th_type == OUTPUTTHREAD) {
    sem_wait (&sem_terminal);
    cout << "LOG:Output thread has been created." << endl;
    sem_post (&sem_terminal);
    sem_post(&create_mids[create_mids.size()-1]);
    output_thread();
  }

   else {
     sem_wait (&sem_terminal);
     cout << "LOG:Middle thread " << th_type << " has been created." << endl;
     sem_post (&sem_terminal);
     sem_wait(&create_mids[(int)((long)th_type) + 2]);
     sem_post(&create_mids[(int)((long)th_type) + 1]);
     middle_thread(th_type);
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
  double num;
  int turn = 0;
  int count = 0;
  if(in_file.is_open()) {
    getline(in_file, line);
    while(getline(in_file, line)) {
          string token = line.substr(line.find('{')+1);
          token = token.substr(0,token.find('}'));
          while(true){
            if(t != 0 && count == 0) {
              turn = 1;
            }

            if(token.find(',') != string::npos) {
              string tmp = token.substr(0,token.find(','));
              num = atof (tmp.c_str());
              token = token.substr(token.find(',')+1);
            }
           else {
             string tmp = token;
             num = atof (tmp.c_str());
             break;
           }


            if((turn != 0) && ((count+1)%(128/mid_th_num)==1)) {
              while(true) {
                sem_wait(&data_r_mid_sem[(count+1)/(128/mid_th_num)]);
                if(data_ready[(count+1)/(128/mid_th_num)] == WAIT)
                  break;
              }
            }

            inputs[count] = num;

            sem_wait (&sem_terminal);//test
            cout << "in:" << num << endl;//test
            sem_post (&sem_terminal);//test

            if((count+1)%128 == 0) {
              data_ready[mid_th_num-1] = READY;
              sem_post(&data_r_mid_sem[mid_th_num-1]);
            }

            else if((count+1) % (128/mid_th_num) == 0) {
              data_ready[count/(128/mid_th_num)] = READY;
              sem_post(&data_r_mid_sem[count/(128/mid_th_num)]);
            }
            count = (count+1)%128;
            t = 1;
          }
        }
        input_complete = true;
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
  double num;
  string bias_str;
  int count = 0;
  if(w_file.is_open()) {
    while(getline(w_file, line)) {//test
          string token = line.substr(line.find('{')+1);
          token = token.substr(0,token.find('}'));
          if((signed)token.find(':')!=-1){
            bias_str = token.substr(token.find(':')+1);
            while(true) {
              sem_wait(&b_sem);
              if(b_ready == false)
                break;
            }
            bias = atof (bias_str.c_str());

            sem_wait (&sem_terminal);//test
            cout << "b:" << bias << endl;//test
            sem_post (&sem_terminal);//test

            b_ready = true;
            sem_post(&b_sem);
            continue;
          }

          while(true){
            if(token.find(',') != string::npos){
              string tmp = token.substr(0,token.find(','));
              num = atof (tmp.c_str());
              token = token.substr(token.find(',')+1);
            }
            else{
              string tmp = token;
              num = atof (tmp.c_str());
              break;
            }
            if((count+1)%(128/mid_th_num)==1) {
              while(true) {
                sem_wait(&w_mid_sem[(count+1)/(128/mid_th_num)]);
                if(weight_ready[(count+1)/(128/mid_th_num)] == WAIT)
                  break;
              }
            }
            weights[count] = num;

            sem_wait (&sem_terminal);//test
            cout << "w:" << num << endl;//test
            sem_post (&sem_terminal);//test

            if(count == 127) {
              weight_ready[mid_th_num-1] = READY;
              sem_post(&w_mid_sem[mid_th_num-1]);
            }

            else if((count+1) % (128/mid_th_num) == 0) {
              weight_ready[count/(128/mid_th_num)] = READY;
              sem_post(&w_mid_sem[count/(128/mid_th_num)]);
            }
            count++;
            // token = token.substr(token.find(',')+1);
            // if((signed)token.find(',') == -1){
            //   break;
            // }
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


void middle_thread(long th_type) {
  int turn = 0;
  while(true) {
    while(true) {
      sem_wait(&data_r_mid_sem[(int)th_type]);
      if(data_ready[(int)th_type] == READY)
        break;
    }

    if(turn == 0) {
      while(true) {
        sem_wait(&w_mid_sem[(int)th_type]);
        if(weight_ready[(int)th_type] == READY)
          break;
      }
    }
    if((int)th_type != mid_th_num-1) {
      if(turn != 0) {
        while(true) {
          sem_wait(&out_done[(int)th_type]);
          if(out_completed[(int)th_type] == true && computed[(int)th_type] == false)
            break;
        }
      }

      for(int i = (int)th_type * (128/mid_th_num); i < (th_type+1) * (128/mid_th_num); i++) {
        sum_list[(int)th_type] += (weights[i]*inputs[i]);
      }
      data_ready[(int)th_type] = WAIT;
      out_completed[(int)th_type] = false;
      computed[(int)th_type] = true;
      sem_post(&out_done[(int)th_type]);
      sem_post(&data_r_mid_sem[(int)th_type]);
    }
    else {
      if(turn != 0) {
        while(true) {
          sem_wait(&out_done[(int)th_type]);
          if(out_completed[(int)th_type] == true && computed[(int)th_type] == false)
            break;
        }
      }
      for(int i = (int)th_type * (128/mid_th_num); i < 128; i++) {
        sum_list[(int)th_type] += (weights[i]*inputs[i]);
      }
      data_ready[(int) th_type] = WAIT;
      out_completed[(int)th_type] = false;
      computed[(int)th_type] = true;
      sem_post(&out_done[(int)th_type]);
      sem_post(&data_r_mid_sem[(int)th_type]);
    }
    if(input_complete)
      return;
    turn = 1;
  }

}
void output_thread() {
  int turn = 0;
  while(true) {
    for(int i = 0; i < mid_th_num; i++) {
      while(true) {
        sem_wait(&out_done[i]);
        if(computed[i] == true && out_completed[i] == false)
          break;
        }
        result += sum_list[i];
    }
    if(turn == 0) {
    while(true) {
      sem_wait(&b_sem);
      if(b_ready)
        break;
    }
  }
  result += bias;
  ofstream outfile;
  outfile.open("outputs.txt", ios_base::app);
  ostringstream strs;
  result = atan (result);
  strs << result;
  string str = strs.str();
  outfile << str << endl;
  for(int i =0; i < mid_th_num; i++) {
    sum_list[i] = 0;
    computed[i] = false;
    out_completed[i] = true;
    sem_post(&out_done[i]);
  }

  result = 0;
  if(input_complete)
    return;
  turn = 1;
}

}

void log(string s) {
  sem_wait (&sem_terminal);//test
  cout << s << endl;//test
  sem_post (&sem_terminal);//test
}
