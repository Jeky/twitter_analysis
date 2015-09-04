#ifndef TWITTER_ANALYSIS_UTILS_UTILS_LOG_H
#define TWITTER_ANALYSIS_UTILS_UTILS_LOG_H

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <utility>
#include <array>

#define LOG_VAR(var) LOG(#var, " = ", var)

static std::ostream *__OUT = &std::cout;
static bool __DEBUG = false;

void getTime();

void LOG(bool start);

template <typename K, typename V>
std::ostream &operator<<(std::ostream &out,
                         const std::pair<K, V> &v) {
    out << "(" << v.first << ", " << v.second << ")";
    return out;
}

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &out,
                         const std::array<T, N> &v) {
    if (!v.empty()) {
        int i = 0;
        out << "[";
        for (auto &item : v) {
            out << item;
            if (i != v.size() - 1) {
                out << ", ";
            }
            i++;
        }
        out << "]";
    }
    return out;
}

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
    if (!v.empty()) {
        int i = 0;
        out << "[";
        for (auto &item : v) {
            out << item;
            if (i != v.size() - 1) {
                out << ", ";
            }
            i++;
        }
        out << "]";
    }
    return out;
}

template <typename T>
std::ostream &operator<<(std::ostream &out,
                         const std::unordered_set<T> &v) {
    if (!v.empty()) {
        int i = 0;
        out << "{";
        for (auto &item : v) {
            out << item;
            if (i != v.size() - 1) {
                out << ", ";
            }
            i++;
        }
        out << "}";
    }
    return out;
}

template <typename K, typename V>
std::ostream &operator<<(std::ostream &out,
                         const std::unordered_map<K, V> &v) {
    if (!v.empty()) {
        int i = 0;
        out << "{";
        for (auto &kv : v) {
            out << kv.first << " = " << kv.second;
            if (i != v.size() - 1) {
                out << ", ";
            }
            i++;
        }
        out << "}";
    }
    return out;
}

template <typename T, typename... Targs>
void LOG(bool start, T cur, Targs... args) {
    *__OUT << cur;
    LOG(false, args...);
}

template <typename T, typename... Targs>
void LOG(T cur, Targs... args) {
    getTime();
    *__OUT << "[INFO] " << cur;
    LOG(false, args...);
}

void ERROR(bool start);

template <typename T, typename... Targs>
void ERROR(bool start, T cur, Targs... args) {
    *__OUT << cur;
    ERROR(false, args...);
}

template <typename T, typename... Targs>
void ERROR(T cur, Targs... args) {
    getTime();
    *__OUT << "[ERROR]" << cur;
    ERROR(false, args...);
}

void DEBUG(bool start);

template <typename T, typename... Targs>
void DEBUG(bool start, T cur, Targs... args) {
    if (__DEBUG) {
        *__OUT << cur;
        DEBUG(false, args...);
    }
}

template <typename T, typename... Targs>
void DEBUG(T cur, Targs... args) {
    if (__DEBUG) {
        getTime();
        *__OUT << "[DEBUG]" << cur;
        DEBUG(false, args...);
    }
}

void setOutput(std::ostream &out);

void setDebug(bool debug);

#endif