#include "ml/ml.h"

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
