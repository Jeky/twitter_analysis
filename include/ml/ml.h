#ifndef _ML_H_
#define _ML_H_

#include <sstream>

#include "../utils.h"

class Instance {
public:
    Instance();

    void setClassValue(double value);

    double getClassValue();

    bool hasAttribute(const String &name);

    double &operator[](const String &name);

    Map<String, double>::iterator begin();

    Map<String, double>::iterator end();

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(values);
    }

    Map<String, double> values;
private:
    double classValue;
};


class Dataset {
public:
    static Dataset *loadDatasetMatrix(const String &filename);

    Dataset();

    void addInstance(Instance &instance);

    int size();

    void shuffle();

    Instance &operator[](int index);

    Vector<Instance>::iterator begin();

    Vector<Instance>::iterator end();

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(instances);
    }

private:
    Vector<Instance> instances;
};

#endif