#ifndef TWITTER_ANALYSIS_ML_H
#define TWITTER_ANALYSIS_ML_H

#include <sstream>
#include <algorithm>

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
     * @param the filename of saved dataset
     * @see saveObject()
     * @return the dataset pointer
     */
    static Dataset *loadDataset(const string &filename);

    Dataset();

    void addInstance(Instance &instance);

    int size();

    void shuffle();

    Instance &at(const int index);

    Instance &operator[](const int index);

    vector<Instance>::iterator begin() { return instances.begin(); };

    vector<Instance>::iterator end() { return instances.end(); };

    template <typename Archive> void serialize(Archive &ar) {
        ar(instances);
    }

    void addDataset(const Dataset &d);

    Dataset &operator+=(const Dataset &d) {
        addDataset(d);
        return *this;
    }

  private:
    vector<Instance> instances;
};

#endif
