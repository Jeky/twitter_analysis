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

    void
    eachFeature(function<void(const string &, const double &)> iterFunc) const {
        for (auto it = values.begin(); it != values.end(); ++it) {
            iterFunc(it->first, it->second);
        }
    }

    template <typename Archive> void serialize(Archive &ar) {
        ar(classValue, values);
    }

  private:
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

    void eachInstance(function<void(const Instance &)> iterFunc) const {
        for (auto it = instances.begin(); it != instances.end(); ++it) {
            iterFunc(*it);
        }
    }

    void addInstance(Instance &instance) { instances.push_back(instance); }

    const int size() const { return instances.size(); }

    void shuffle() { random_shuffle(instances.begin(), instances.end()); }

    Instance &at(const int index) { return instances[index]; }

    Instance &operator[](const int index) { return instances[index]; }

    template <typename Archive> void serialize(Archive &ar) { ar(instances); }

    void addDataset(const Dataset &d) {
        for (auto &i : d.instances) {
            this->instances.push_back(i);
        }
    }

    Dataset &operator+=(const Dataset &d) {
        addDataset(d);
        return *this;
    }

  private:
    vector<Instance> instances;
};

#endif
