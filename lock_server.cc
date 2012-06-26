// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
 pthread_mutex_init(&mutex_, NULL);
 pthread_cond_init (&condVar_, NULL);
}

lock_server::~lock_server()
{
 pthread_mutex_destroy(&mutex_);
 pthread_cond_destroy(&condVar_);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int& r) {

  pthread_mutex_lock(&mutex_);
  it = locks_.find(lid);
  if(it != locks_.end() ) { //Same lock was requested before

    while(!(it->second)) {  //Another thread is holding the lock
      pthread_cond_wait(&condVar_, &mutex_);		
    }		 
    it->second = false;	 	
  } else {  //New lock
  locks_.insert(pair<lock_protocol::lockid_t,bool> (lid, false)); //Grant the lock
  }
  lock_protocol::status ret = lock_protocol::OK;
  r = nacquire;
  pthread_mutex_unlock(&mutex_);
  return ret;
}

lock_protocol::status 
lock_server::release(int clt, lock_protocol::lockid_t lid, int& r) {

  pthread_mutex_lock(&mutex_);
  it = locks_.find(lid);
  if(it->second == false) {
    it->second = true; // Release the lock
    pthread_cond_signal(&condVar_);	
  }
  lock_protocol::status ret = lock_protocol::OK;
  r = nacquire;
  pthread_mutex_unlock(&mutex_);
  return ret;
}

