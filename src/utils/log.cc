#include "utils/log.h"

void getTime() {
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm *ptm = std::localtime(&tt);
    *__OUT << "[" << std::put_time(ptm, "%F %T") << "]";
}

void LOG(bool start) { *__OUT << std::endl; }

void ERROR(bool start) {
    *__OUT << std::endl;
    exit(-1);
}

void DEBUG(bool start) {
    if (__DEBUG) {
        *__OUT << std::endl;
    }
}

void setOutput(std::ostream &out) { __OUT = &out; }

void setDebug(bool debug) { __DEBUG = debug; }

void TIMER_START(const std::string &name) {
    if (_timer.find(name) == _timer.end()) {
        _timer[name] = std::clock();
    } else {
        _timer[name] = std::clock() - _timer[name];
    }
}

void TIMER_END(const std::string &name) {
    _timer[name] = std::clock() - _timer[name];
}

void SHOW_TIMER() {
    for (auto &kv : _timer) {
        LOG(kv.first, " = ", kv.second / (double)(CLOCKS_PER_SEC / 1000),
            " ms");
    }
}

void CLEAR_TIMER() {
    _timer.clear();
}
