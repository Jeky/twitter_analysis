#include "data.h"
#include "utils.h"

void convertToDS() {
    Map<long, User> *spammers = loadSpammers();
    Map<long, User> *nonSpammers = loadNonSpammers();

    Dataset *spammerDS = user2Dataset(spammers, 1);
    delete spammers;
    Dataset *nonSpammerDS = user2Dataset(nonSpammers, 1);
    delete nonSpammers;

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
