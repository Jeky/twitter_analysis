#include "log.h"


void getTime(){
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t (system_clock::now());

  struct std::tm * ptm = std::localtime(&tt);
  std::cout << "[" << std::put_time(ptm, "%F %T") << "]";
}


std::ostream &log(){
    getTime();
    *OUT << "[INFO]  ";
    return *OUT;
}


std::ostream &error(){
    getTime();
    *OUT << "[ERROR]";
    return *OUT;
}


std::ostream &debug(){
    getTime();
    *OUT << "[DEBUG]";
    return *OUT;
}


void setOutput(std::ostream &out){
    OUT = &out;
}


void setDebug(bool debug){
    DEBUG = debug;
}
