#include "data.h"

/*
 * Tweet Member Functions
 */

const string Tweet::TWEET_PATH = PATH + string("tweets/");


ostream &operator<<(ostream &out, Tweet &t) {
    out << "Tweet[" << t.getText() << "]";
    return out;
}


void User::loadTweets() {
    string filename = Tweet::TWEET_PATH + to_string(id);

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

unordered_map<long, User> *loadData(const string &dataPath, const string &path, bool spammer) {
    ifstream infile(dataPath);

    if (!infile.good()) {
        infile.close();

        unordered_map<long, User> *users = new unordered_map<long, User>();
        readFile(path, true, [&](int index, string &line) {
            long id = stol(line);
            User u(id, spammer);
            u.loadTweets();
            (*users)[id] = u;
            return true;
        });

        saveObject(users, dataPath);

        return users;
    } else {
        infile.close();

        return loadObject<unordered_map<long, User >>(dataPath);
    }
}


unordered_map<long, User> *loadSpammers() {
    return loadData(SPAMMER_DATA_PATH,
                    PATH + string("spammers.id.list"), true);
}


unordered_map<long, User> *loadNonSpammers() {
    return loadData(NONSPAMMER_DATA_PATH,
                    PATH + string("nonspammers.id.list"), false);
}


Dataset *user2Dataset(unordered_map<long, User> *users, int gramLen) {
    Dataset *dataset = new Dataset();

    for (auto &kv : *users) {
        User u = kv.second;

        Instance ins;
        ins.setClassValue(u.isSpammer() ? SPAMMER_VALUE : NON_SPAMMER_VALUE);

        for (auto &t : u.getTweets()) {
            vector<string> grams = toGrams(t.getText(), gramLen);
            for (auto &g : grams) {
                ins[g] += 1.0;
            };
        };

        dataset->addInstance(ins);
    };

    return dataset;
}
