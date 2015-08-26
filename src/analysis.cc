#include "data.h"
#include "utils.h"

void convertToDS() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadNonSpammers();

    LOG("User loaded. Convert users to Dataset...");

    Dataset *spammerDS = user2Dataset(spammers, 1);
    delete spammers;
    Dataset *nonSpammerDS = user2Dataset(nonSpammers, 1);
    delete nonSpammers;

    LOG("Save datasets...");
    saveObject(spammerDS, PATH + "spammer.dat");
    delete spammerDS;
    saveObject(nonSpammerDS, PATH + "nonspammer.dat");
    delete nonSpammerDS;
}

void printDatasetStatistic(Dataset *spammer, Dataset *nonSpammer) {

}

int main(int argc, char const *argv[]) {
    convertToDS();
    return 0;
}
