#include "ml/ml.h"
#include "ml/features.h"

int main(int argc, char const *argv[]) {
    Dataset *ds = Dataset::loadDatasetMatrix("/Users/jeky/Desktop/test.txt");

    FeatureSelector *selector = new BiClassMutualInformation();

    selector->train(ds);

    LOG(*selector->getTopFeatureList());

    delete selector;
    delete ds;

    return 0;
}
