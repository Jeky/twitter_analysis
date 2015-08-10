#ifndef _ML_H_
#define _ML_H_

#include "utils.h"

class Instance{
public:
    Instance();
    void setClassValue(double value);
    double getClassValue(double value);
    bool hasAttribute(String &name);
    double &operator[](String &name);
    double &operator[](const char *name);
    
    template <typename Archive>
    void serialize(Archive &ar){
        ar(values);
    }
private:
    double classValue;
    Map<String, double> values;
};


class Dataset{
public:
    Dataset();
    void addInstance(Instance &instance);
    int size();
    void shuffle();
    Instance &operator[](int index);

    template <typename Archive>
    void serialize(Archive &ar){
        ar(instances);
    }
private:
    Vector<Instance> instances;
};


Dataset user2Dataset(Map<long, User> users);

#endif