#include "classifier.h"

void NaiveBayes::reset(){
    clsProb.clear();
    clsFeatureProb.clear();
}


void NaiveBayes::train(Dataset &dataset){
    Map<double, double> clsWordCount;
    Set<String> featureSet;

    // collect features and compute class probability
    for(int i = 0; i < dataset.size(); i++){
        double cls = dataset[i].getClassValue();
        if(clsProb.find(cls) == clsProb.end()){
            clsFeatureProb[cls] = Map<String, double>();
        }

        mapAdd(clsProb, cls, 1.0);

        FOREACH<String, double>(dataset[i].values, [&](String &f, double &v){
            mapAdd(clsFeatureProb[cls], f, v);
            mapAdd(clsWordCount, cls, v);
            featureSet.insert(f);
        });
    };

    // compute class probability
    FOREACH<double, double>(clsProb, [&](double &cls, double &v){
        clsProb[cls] = log(v / dataset.size());
    });

    // compute class feature probability
    int featureSize = featureSet.size();
    FOREACH<double, Map<String, double>>(clsFeatureProb, 
        [&](double &cls, Map<String, double> &featureProb){
        for(const String &k : featureSet){
            double v = 0.0;
            if(featureProb.find(k) != featureProb.end()){
                v = featureProb[k];
            }

            featureProb[k] = log((v + 1.0) / (clsWordCount[cls] + featureSize));
        };
    });
}


double NaiveBayes::classify(Instance &ins){
    double cls = 0.0;
    double prob = -1.0;

    FOREACH<double, Map<String, double>>(clsFeatureProb, 
        [&](double &thisCls, Map<String, double> &featureProb){
        double thisProb = clsProb[thisCls];

        FOREACH<String, double>(ins.values, [&](String &f, double &v){
            if(clsFeatureProb[thisCls].find(f) != clsFeatureProb[thisCls].end()){
                thisProb += clsFeatureProb[thisCls][f] * v;
            }
        });

        if(thisProb > prob || prob == -1.0){
            cls = thisCls;
            prob = thisProb;
        }
    });

    return cls;
}