#include "ml/ml.h"

Instance::Instance() {}

void Instance::setClassValue(double value) { classValue = value; }

double Instance::getClassValue() { return classValue; }

bool Instance::hasAttribute(const string &name) {
    return values.find(name) != values.end();
}

double &Instance::operator[](const string &name) {
    if (!hasAttribute(name)) {
        values[name] = 0.0;
    }
    return values[name];
}

unordered_map<string, double>::iterator Instance::begin() {
    return values.begin();
}

unordered_map<string, double>::iterator Instance::end() {
    return values.end();
}

Dataset *Dataset::loadDatasetMatrix(const string &filename) {
    Dataset *ds = new Dataset();

    readFile(filename, false, [ds](int i, string &line) {
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

Dataset *Dataset::loadDataset(const string &filename) {
    return loadObject<Dataset>(filename);
}

Dataset::Dataset() {}

void Dataset::addInstance(Instance &instance) {
    instances.push_back(instance);
}

int Dataset::size() { return instances.size(); }

void Dataset::shuffle() {
    random_shuffle(instances.begin(), instances.end());
}

Instance &Dataset::at(int index) { return instances[index]; }

Instance &Dataset::operator[](int index) { return instances[index]; }

void Dataset::addDataset(const Dataset &d) {
    for (auto &i : d.instances) {
        this->instances.push_back(i);
    }
}