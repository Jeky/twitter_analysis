#include "data.h"

/*
 * Tweet Member Functions
 */

const String Tweet::TWEET_PATH = PATH + String("tweets/");

Vector<Tweet> Tweet::loadTweets(long id) {
    String filename = TWEET_PATH + to_string(id);
    Vector<Tweet> tweets;

    readFile(filename, false, [&tweets](int index, string &line) {
        tweets.push_back(Tweet(line));
        return true;
    });

    return tweets;
}


Tweet::Tweet() { };


Tweet::Tweet(String &text) {
    this->text = text;
}


Tweet::Tweet(char const *text) {
    this->text = text;
}


String Tweet::getText() {
    return text;
}


ostream &operator<<(ostream &out, Tweet &t) {
    out << "Tweet[" << t.getText() << "]";
    return out;
}

/*
 * User Member Functions
 */

User::User() { };


User::User(long id, bool spammer) {
    this->id = id;
    this->spammer = spammer;
    this->tweets = Tweet::loadTweets(id);
}


long User::getId() {
    return id;
}


bool User::isSpammer() {
    return spammer;
}


Vector<Tweet> User::getTweets() {
    return tweets;
}


ostream &operator<<(ostream &out, User &u) {
    out << "User[" << u.getId() << ", Tweets Count = "
    << u.getTweets().size() << "]";
    return out;
}

/*
 * Data loading functions
 */

Map<long, User> *loadData(const String &dataPath, const String &path, bool spammer) {
    ifstream infile(dataPath);

    if (!infile.good()) {
        infile.close();

        Map<long, User> *users = new Map<long, User>();
        readFile(path, true, [&](int index, String &line) {
            long id = stol(line);
            (*users)[id] = User(id, spammer);
            return true;
        });

        saveObject(users, dataPath);

        return users;
    } else {
        infile.close();

        return loadObject<Map<long, User >>(dataPath);
    }
}


Map<long, User> *loadSpammers() {
    return loadData(SPAMMER_DATA_PATH,
                    PATH + String("spammers.id.list"), true);
}


Map<long, User> *loadNonSpammers() {
    return loadData(NONSPAMMER_DATA_PATH,
                    PATH + String("nonspammers.id.list"), false);
}


Dataset *user2Dataset(Map<long, User> *users, int gramLen) {
    Dataset *dataset = new Dataset();

    for (auto &kv : *users) {
        User u = kv.second;

        Instance ins;
        ins.setClassValue(u.isSpammer() ? SPAMMER_VALUE : NON_SPAMMER_VALUE);

        for (auto &t : u.getTweets()) {
            Vector<String> grams = toGrams(t.getText(), gramLen);
            for (auto &g : grams) {
                ins[g] += 1.0;
            };
        };

        dataset->addInstance(ins);
    };

    return dataset;
}
