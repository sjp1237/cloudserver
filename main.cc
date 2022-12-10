#include"service.hpp"
#include"hotspot.hpp"
#include<pthread.h>


void* hotspotRun(void* arg){
  pthread_detach(pthread_self());
  sjp::HotSpotManger::GetInstant()->Run();
  return NULL;
}


void* serviceRun(void* arg){
  pthread_detach(pthread_self());
  sjp::Service::GetInstant()->Run();
  return NULL;
}


void test(){
  pthread_t threadid1;
  pthread_t threadid2;
  pthread_create(&threadid1,NULL,hotspotRun,NULL);
  pthread_create(&threadid2,NULL,serviceRun,NULL);
  while(true){
     
  }
}

void hotspot_test(){
  sjp::HotSpotManger::GetInstant()->Run();
}
int main(){
  test();
  return 0;
}
