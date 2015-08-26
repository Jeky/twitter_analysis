#ifndef _ML_H_
#define _ML_H_

#include <sstream>

#include "../utils.h"

class Instance {
public:
    Instance();

    void setClassValue(double value);

    double getClassValue();

    bool hasAttribute(const string &name);

    double &operator[](const string &name);

    unordered_map<string, double>::iterator begin();

    unordered_map<string, double>::iterator end();

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(values);
    }

    unordered_map<string, double> values;
private:
    double classValue;
};


class Dataset {
public:
    static Dataset *loadDatasetMatrix(const string &filename);

    Dataset();

    void addInstance(Instance &instance);

    int size();

    void shuffle();

    Instance &at(const int index);

    Instance &operator[](const int index);

    vector<Instance>::iterator begin();

    vector<Instance>::iterator end();

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(instances);
    }

private:
    vector<Instance> instances;
};

#endif