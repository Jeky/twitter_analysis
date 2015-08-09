#ifndef _UTILS__LOG_H_
#define _UTILS__LOG_H_

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <cereal/types/string.hpp>

static std::ostream *OUT = &std::cout;
static bool DEBUG = false;

void getTime();
std::ostream &log();
std::ostream &error();
std::ostream &debug();
void setOutput(std::ostream &out);
void setDebug(bool debug);


#endif