#ifndef __thread__
#define __thread__

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>
#include <functional>

class Thread {
public:
  virtual void* routine(void*)=0;
	Thread();
  void join_thread();
	pthread_t* get_thread() { return pth; }
private:
	pthread_t* pth;
};

class InputThread : public Thread {
public:
	InputThread():  Thread() {}
  virtual void* routine(void*) { std::cout<<"input"<<std::endl;}
};

class WeightThread : public Thread {
public:
	WeightThread(): Thread() {}
  virtual void* routine(void*) { std::cout<<"input"<<std::endl; }
};

class MiddleThread : public Thread {
public:
	MiddleThread(int i): ID(i), Thread() {}
  virtual void* routine(void*) {std::cout<<"input"<<std::endl;}
private:
  int ID;
};

class OutputThread : public Thread {
public:
	OutputThread(): Thread() {}
  virtual void* routine(void*) {std::cout<<"input"<<std::endl;}
};

#endif
