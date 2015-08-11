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

        for(auto &kv : dataset[i]){
            mapAdd(clsFeatureProb[cls], kv.first, kv.second);
            mapAdd(clsWordCount, cls, kv.second);
            featureSet.insert(kv.first);
        };
    };

    // compute class probability
    for(auto &kv : clsProb){
        clsProb[kv.first] = log(kv.second / dataset.size());
    };

    // compute class feature probability
    int featureSize = featureSet.size();
    for(auto &kv : clsFeatureProb){
        double cls = kv.first;
        Map<String, double> featureProb = kv.second;

        for(auto &k : featureSet){
            double v = 0.0;
            if(featureProb.find(k) != featureProb.end()){
                v = featureProb[k];
            }

            featureProb[k] = log((v + 1.0) / (clsWordCount[cls] + featureSize));
        };
    };
}


double NaiveBayes::classify(Instance &ins){
    double cls = 0.0;
    double prob = -1.0;

    for(auto &kv : clsFeatureProb){
        double thisCls = kv.first;
        Map<String, double> featureProb = kv.second;
        double thisProb = clsProb[thisCls];

        for(auto &kv : ins){
            String f = kv.first;
            double v = kv.second;

            if(clsFeatureProb[thisCls].find(f) != clsFeatureProb[thisCls].end()){
                thisProb += clsFeatureProb[thisCls][f] * v;
            }
        };

        if(thisProb > prob || prob == -1.0){
            cls = thisCls;
            prob = thisProb;
        }
    };

    return cls;
}