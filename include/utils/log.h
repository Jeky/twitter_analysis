#ifndef _UTILS__LOG_H_
#define _UTILS__LOG_H_

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <cereal/types/string.hpp>

static std::ostream *__OUT = &std::cout;
static bool __DEBUG = false;

void getTime();

void LOG(bool start);


template <typename T, typename... Targs>
void LOG(bool start, T cur, Targs... args){
    *__OUT << cur;
    LOG(false, args...);
}


template <typename T, typename... Targs>
void LOG(T cur, Targs... args){
    getTime();
    *__OUT << "[INFO] " << cur;
    LOG(false, args...);
}


void ERROR(bool start);

template <typename T, typename... Targs>
void ERROR(bool start, T cur, Targs... args){
    *__OUT << cur;
    ERROR(false, args...);
}


template <typename T, typename... Targs>
void ERROR(T cur, Targs... args){
    getTime();
    *__OUT << "[ERROR]" << cur;
    ERROR(false, args...);
}


void DEBUG(bool start);

template <typename T, typename... Targs>
void DEBUG(bool start, T cur, Targs... args){
    if(__DEBUG){
        *__OUT << cur;
        DEBUG(false, args...);
    }
}


template <typename T, typename... Targs>
void DEBUG(T cur, Targs... args){
    if(__DEBUG){
        getTime();
        *__OUT << "[DEBUG]" << cur;
        DEBUG(false, args...);
    }
}

void setOutput(std::ostream &out);
void setDebug(bool debug);


#endif