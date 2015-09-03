#include "data.h"

ostream &operator<<(ostream &out, Tweet &t) {
    out << "Tweet[" << t.getText() << "]";
    return out;
}


void User::loadTweets() {
    string filename = (spammer ? SPAMMER_TWEET_PATH : NON_SPAMMER_TWEET_PATH) + to_string(id);

    readFile(filename, false, [&](int index, string &line) {
        tweets.push_back(Tweet(line));
        return true;
    });
}


ostream &operator<<(ostream &out, User &u) {
    out << "User[" << u.getId() << ", Tweets Count = " << u.getTweets().size() << "]";
    return out;
}

/*
 * Data loading functions
 */

unordered_map<long, User> *loadData(const string &dataPath, const string &path, const bool spammer,
                                    function<void(unordered_map<long, User> *)> postDataHandler) {
    ifstream infile(dataPath);

    if (!infile.good()) {
        LOG("Cannot Find ", dataPath, ". Loading File from ", path);
        infile.close();

        unordered_map<long, User> *users = new unordered_map<long, User>();
        readFile(path, true, [&](int index, string &line) {
            long id = stol(line);
            User u(id, spammer);
            u.loadTweets();
            (*users)[id] = u;
            return true;
        });

        postDataHandler(users);
        saveObject(users, dataPath);

        return users;
    } else {
        LOG("Loading Data from ", path);
        infile.close();

        return loadObject<unordered_map<long, User >>(dataPath);
    }
}

unordered_map<long, User> *loadData(const string &dataPath, const string &path, const bool spammer) {
    return loadData(dataPath, path, spammer, [](unordered_map<long, User> *users) { });
}


unordered_map<long, User> *loadSpammers() {
    return loadData(SPAMMER_DATA_PATH, SPAMMER_ID_LIST, true);
}


unordered_map<long, User> *loadSampledNonSpammers() {
    return loadData(NON_SPAMMER_DATA_PATH, SAMPLED_NON_SPAMMER_ID_LIST, false,
                    [](unordered_map<long, User> *users) {
                        for (auto &kv : *users) {
                            random_shuffle(kv.second.getTweets().begin(),
                                           kv.second.getTweets().end());
                            kv.second.getTweets().erase(kv.second.getTweets().begin() + SAMPLE_TWEET_SIZE,
                                                        kv.second.getTweets().end());
                        }
                    });
}


void sampleNonSpammers() {
    LOG("Start Sampling Normal Users...");
    unordered_set<long> spammerIds;
    readFile(SPAMMER_ID_LIST, [&](int i, string &line) {
        spammerIds.insert(stol(line));
        return true;
    });
    LOG_VAR(spammerIds.size());

    vector<long> nonSpammerIds;
    readFile(NON_SPAMMER_ID_LIST, [&](int i, string &line) {
        long id = stol(line);
        if (spammerIds.find(id) == spammerIds.end()) {
            User u(id, false);
            u.loadTweets();
            if (u.getTweets().size() >= SAMPLE_TWEET_SIZE) {
                nonSpammerIds.push_back(id);
            }
        }
        if (nonSpammerIds.size() % 100 == 0 && nonSpammerIds.size() != 0) {
            LOG("Sampled ", nonSpammerIds.size(), " users");
        }
        return true;
    });
    LOG_VAR(nonSpammerIds.size());

    random_shuffle(nonSpammerIds.begin(), nonSpammerIds.end());

    writeFile(SAMPLED_NON_SPAMMER_ID_LIST, [&](ofstream &out) {
        for (int i = 0; i < spammerIds.size(); i++) {
            out << nonSpammerIds[i] << endl;
        }
    });
}


Dataset *user2Dataset(unordered_map<long, User> *users, int gramLen) {
    Dataset *dataset = new Dataset();

    for (auto &kv : *users) {
        Instance ins;
        ins.setClassValue(kv.second.isSpammer() ? SPAMMER_VALUE : NON_SPAMMER_VALUE);

        for (auto &t : kv.second.getTweets()) {
            vector<string> *grams = toGrams(t.getText(), gramLen);
            for (auto &g : *grams) {
                ins[g] += 1.0;
            };
            delete grams;
        };

        dataset->addInstance(ins);
    };

    return dataset;
}
