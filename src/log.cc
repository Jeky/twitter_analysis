#include "log.h"


void getTime(){
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t (system_clock::now());

  struct std::tm * ptm = std::localtime(&tt);
  *__OUT << "[" << std::put_time(ptm, "%F %T") << "]";
}


std::ostream &LOG(){
    getTime();
    *__OUT << "[INFO]  ";
    return *__OUT;
}


std::ostream &ERROR(){
    getTime();
    *__OUT << "[ERROR]";
    return *__OUT;
}


std::ostream &DEBUG(){
    getTime();
    *__OUT << "[DEBUG]";
    return *__OUT;
}


void setOutput(std::ostream &out){
    __OUT = &out;
}


void setDebug(bool debug){
    __DEBUG = debug;
}
