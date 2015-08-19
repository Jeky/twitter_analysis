#include "utils/log.h"


void getTime() {
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm *ptm = std::localtime(&tt);
    *__OUT << "[" << std::put_time(ptm, "%F %T") << "]";
}


void LOG(bool start) {
    *__OUT << std::endl;
}


void ERROR(bool start) {
    *__OUT << std::endl;
    exit(-1);
}


void DEBUG(bool start) {
    if (__DEBUG) {
        *__OUT << std::endl;
    }
}

void setOutput(std::ostream &out) {
    __OUT = &out;
}


void setDebug(bool debug) {
    __DEBUG = debug;
}
