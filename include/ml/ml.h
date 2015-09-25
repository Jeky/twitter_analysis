#ifndef TWITTER_ANALYSIS_ML_H
#define TWITTER_ANALYSIS_ML_H

#include <sstream>
#include <algorithm>
#include <functional>

#include "../utils.h"

class Instance {
  public:
    Instance() {}

    void setClassValue(double value) { classValue = value; }

    const double getClassValue() const { return classValue; }

    const bool hasAttribute(const string &name) const {
        return values.find(name) != values.end();
    }

    double &operator[](const string &name) {
        if (!hasAttribute(name)) {
            values[name] = 0.0;
        }
        return values[name];
    }

    double at(const string &name) const {
        auto iter = values.find(name);
        return iter == values.end() ? 0.0 : iter->second;
    }

    template <typename Archive> void serialize(Archive &ar) {
        ar(classValue, values);
    }

    unordered_map<string, double> values;
    double classValue;
};

class Dataset {
  public:
    /**
     * Load dataset from a matrix file. In this file, each line
     * contains several numbers. Each Line in this file will be
     * converted into an Instance and the whole file will be converted
     * into a dataset.
     *
     * @param the filename of matrix data
     * @return the dataset pointer
     */
    static Dataset *loadDatasetMatrix(const string &filename);

    /**
     * Load dataset from saved file. This dataset should be saved by
     * saveObject() function.
     *
     * @param filename: the filename of saved dataset
     * @param label: the label of instances in this dataset
     * @see saveObject()
     * @return the dataset pointer
     */
    static Dataset *loadDataset(const string &filename, const double label) {
        Dataset *ds = loadObject<Dataset>(filename);
        for (int i = 0; i < ds->size(); i++) {
            (*ds)[i].setClassValue(label);
        }

        return ds;
    }

    Dataset() {}

    void addInstance(Instance &instance) { instances.push_back(instance); }

    const int size() const { return instances.size(); }

    void shuffle() { random_shuffle(instances.begin(), instances.end()); }

    Instance &at(const int index) { return instances[index]; }

    Instance &operator[](const int index) { return instances[index]; }

    template <typename Archive> void serialize(Archive &ar) { ar(instances); }

    void addDataset(const Dataset &d) {
        for (auto it = d.instances.begin(), end = d.instances.end(); it != end;
             it++) {
            instances.push_back(*it);
        }
    }

    void save(const string &path);

    Dataset &operator+=(const Dataset &d) {
        addDataset(d);
        return *this;
    }

    vector<Instance> instances;
};

#endif
