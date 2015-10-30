#include "data.h"
#include "ml/text.h"
#include "ml/classifier.h"
#include "ml/evaluator.h"
#include "ml/features.h"

#include "utils.h"
#include <iterator>

void convertToDS() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadNonSpammers();

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

void testFeatureSelection() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    auto *all = spammerDS;
    all->addDataset(*nonSpammerDS);
    delete nonSpammerDS;

    FeatureSelector *selector = new BiClassMutualInformation();
    selector->train(all);
    selector->save(PATH + "selected-feature-mi.txt");

    delete selector;
    delete all;
}

void testFeatureRelation() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Evaluator eval;

    eval.featureSelectionValidate(spammerDS, nonSpammerDS,
    		PATH + "selected-feature-wapmi.txt",
			PATH + "wapmi-test.txt",
			100, 10000);

    eval.featureSelectionValidate(spammerDS, nonSpammerDS,
    		PATH + "selected-feature-mi.txt",
			PATH + "mi-test.txt",
			100, 10000);

    delete spammerDS;
    delete nonSpammerDS;
}

int main(int argc, char const *argv[]) {
	testFeatureRelation();

    return 0;
}
