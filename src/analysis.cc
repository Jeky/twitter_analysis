#include "data.h"
#include "ml/text.h"
#include "ml/classifier.h"
#include "ml/evaluator.h"
#include "ml/features.h"

#include "utils.h"
#include <iterator>

void convertToDS() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadSampledNonSpammers();

    LOG("User loaded. Convert users to Dataset...");

    auto *spammerDS = user2Dataset(spammers);
    delete spammers;

    auto *nonSpammerDS = user2Dataset(nonSpammers);
    delete nonSpammers;

    LOG("Save datasets...");
    saveObject(spammerDS, SPAMMER_DS);

    delete spammerDS;

    saveObject(nonSpammerDS, NON_SPAMMER_DS);
    delete nonSpammerDS;
}

void testClassification() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Classifier *cls = new NaiveBayes();
    Evaluator eval;

    eval.crossValidate(10, cls, spammerDS, nonSpammerDS);
    for (auto &item : eval.getConfusionMatrixVector()) {
        LOG(item);
    }
    LOG_VAR(eval.getAccuracy());
    LOG_VAR(eval.getRecall());
    LOG_VAR(eval.getPrecision());
    LOG_VAR(eval.getF1());

    delete cls;
    delete spammerDS;
    delete nonSpammerDS;
}


int main(int argc, char const *argv[]) {
    convertToDS();
    testClassification();

    return 0;
}
