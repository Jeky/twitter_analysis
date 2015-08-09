#ifndef _UTILS__LOG_H_
#define _UTILS__LOG_H_

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <cereal/types/string.hpp>

static std::ostream *__OUT = &std::cout;
static bool __DEBUG = false;

std::ostream &LOG();
std::ostream &ERROR();
std::ostream &DEBUG();
void setOutput(std::ostream &out);
void setDebug(bool debug);


#endif