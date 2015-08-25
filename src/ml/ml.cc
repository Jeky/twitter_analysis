#include "ml/ml.h"

Instance::Instance() { }


void Instance::setClassValue(double value) {
    classValue = value;
}


double Instance::getClassValue() {
    return classValue;
}


bool Instance::hasAttribute(const String &name) {
    return values.find(name) != values.end();
}


double &Instance::operator[](const String &name) {
    if (!hasAttribute(name)) {
        values[name] = 0.0;
    }
    return values[name];
}


Map<String, double>::iterator Instance::begin() {
    return values.begin();
}


Map<String, double>::iterator Instance::end() {
    return values.end();
}


Dataset *Dataset::loadDatasetMatrix(const String &filename) {
    Dataset *ds = new Dataset();

    readFile(filename, false, [ds](int i, String &line) {
        stringstream ss(line);
        double buf;
        Instance instance;
        int colCount = 0;
        double cls;
        ss >> cls;
        instance.setClassValue(cls);

        while (ss >> buf) {
            instance[to_string(colCount)] = buf;
            colCount++;
        }

        ds->addInstance(instance);

        return true;
    });


    return ds;
}

Dataset::Dataset() { }


void Dataset::addInstance(Instance &instance) {
    instances.push_back(instance);
}


int Dataset::size() {
    return instances.size();
}


void Dataset::shuffle() {
}


Instance &Dataset::operator[](int index) {
    return instances[index];
}


Vector<Instance>::iterator Dataset::begin() {
    return instances.begin();
}


Vector<Instance>::iterator Dataset::end() {
    return instances.end();
}