#include "data.h"

ostream &operator<<(ostream &out, Tweet &t) {
    out << "Tweet[" << t.getText() << "]";
    return out;
}

static const string RT_PREFIX = "rt";
static const string VIA_RT_PREFIX = "via";

bool Tweet::isRetweet() const { // see
    // https://dev.twitter.com/overview/api/entities-in-twitter-objects
    string s = text;
    toLowerString(s);
    return s.compare(0, RT_PREFIX.size(), RT_PREFIX) == 0 ||
           s.compare(0, VIA_RT_PREFIX.size(), VIA_RT_PREFIX) == 0;
}

static const regex URL_MATCHER(".*((http)|(https))://(\\w+|\\.|/)+.*");

bool Tweet::containsUrl() const {
    return regex_match(text.begin(), text.end(), URL_MATCHER);
}

void User::loadTweets() {
    string filename =
        (spammer ? SPAMMER_TWEET_PATH : NON_SPAMMER_TWEET_PATH) + to_string(id);

    readFile(filename, false, [&](int index, string &line) {
        for (int i = 0; i < 3; i++) {
            unescapeHTML(line);
        }
        tweets.push_back(Tweet(line));
        return true;
    });
}

ostream &operator<<(ostream &out, User &u) {
    out << "User[" << u.getId() << ", Tweets Count = " << u.getTweets().size()
        << "]";
    return out;
}

/*
 * Data loading functions
 */

unordered_map<long, User> *
loadData(const string &dataPath, const string &path, const bool spammer,
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
        LOG("Loading Data from ", dataPath);
        infile.close();

        return loadObject<unordered_map<long, User>>(dataPath);
    }
}

unordered_map<long, User> *loadData(const string &dataPath, const string &path,
                                    const bool spammer) {
    return loadData(dataPath, path, spammer,
                    [](unordered_map<long, User> *users) {});
}

unordered_map<long, User> *loadSpammers() {
    return loadData(SPAMMER_DATA_PATH, SPAMMER_ID_LIST, true);
}

unordered_map<long, User> *loadNonSpammers() {
    return loadData(NON_SPAMMER_DATA_PATH, NON_SPAMMER_ID_LIST, false);
}

// unordered_map<long, User> *loadSampledNonSpammers() {
//     return loadData(NON_SPAMMER_DATA_PATH, SAMPLED_NON_SPAMMER_ID_LIST,
//     false,
//                     [](unordered_map<long, User> *users) {
//         for (auto &kv : *users) {
//             random_shuffle(kv.second.getTweets().begin(),
//                            kv.second.getTweets().end());
//             kv.second.getTweets().erase(kv.second.getTweets().begin() +
//                                             SAMPLE_TWEET_SIZE,
//                                         kv.second.getTweets().end());
//         }
//     });
// }

// void sampleNonSpammers() {
//     LOG("Start Sampling Normal Users...");
//     unordered_set<long> spammerIds;
//     readFile(SPAMMER_ID_LIST, [&](int i, string &line) {
//         spammerIds.insert(stol(line));
//         return true;
//     });
//     LOG_VAR(spammerIds.size());

//     vector<long> nonSpammerIds;
//     readFile(NON_SPAMMER_ID_LIST, [&](int i, string &line) {
//         long id = stol(line);
//         if (spammerIds.find(id) == spammerIds.end()) {
//             User u(id, false);
//             u.loadTweets();
//             if (u.getTweets().size() >= SAMPLE_TWEET_SIZE) {
//                 nonSpammerIds.push_back(id);
//             }
//         }
//         if (nonSpammerIds.size() % 100 == 0 && nonSpammerIds.size() != 0) {
//             LOG("Sampled ", nonSpammerIds.size(), " users");
//         }
//         return true;
//     });
//     LOG_VAR(nonSpammerIds.size());

//     random_shuffle(nonSpammerIds.begin(), nonSpammerIds.end());

//     writeFile(SAMPLED_NON_SPAMMER_ID_LIST, [&](ofstream &out) {
//         for (int i = 0; i < spammerIds.size(); i++) {
//             out << nonSpammerIds[i] << endl;
//         }
//     });
// }

Dataset *user2Dataset(unordered_map<long, User> *users, int gramLen) {
    LOG("Converting user map to dataset");
    Dataset *dataset = new Dataset();

    for (auto &kv : *users) {
        if (dataset->size() % 1000 == 0) {
            LOG("Processed ", dataset->size(), " users");
        }
        Instance ins;
        ins.setClassValue(kv.second.isSpammer() ? SPAMMER_VALUE
                                                : NON_SPAMMER_VALUE);

        for (auto &t : kv.second.getTweets()) {
            vector<string> *grams = toGrams(t.getText(), gramLen);
            vector<string> *tokens = filterSpecialWords(grams);
            for (auto &g : *tokens) {
                ins[g] += 1.0;
            };
            delete tokens;
            delete grams;
        };

        dataset->addInstance(ins);
    };

    return dataset;
}

vector<double> *loadPropRate(const string &path) {
    vector<double> *prop = new vector<double>();
    readFile(path, [&](int i, string &l) {
        prop->push_back(stof(l));
        return true;
    });
    return prop;
}

Dataset *loadPropDataset(bool isSpammer) {
    Dataset *ds = new Dataset();
    vector<double> *count = isSpammer ? loadPropRate(SUSPENDED_TWEET_COUNT)
                                      : loadPropRate(NON_SUSPENDED_TWEET_COUNT);
    vector<double> *url = isSpammer ? loadPropRate(SUSPENDED_URL_COUNT)
                                    : loadPropRate(NON_SUSPENDED_URL_COUNT);
    vector<double> *mention = isSpammer
                                  ? loadPropRate(SUSPENDED_MENTION_COUNT)
                                  : loadPropRate(NON_SUSPENDED_MENTION_COUNT);
    vector<double> *hashtag = isSpammer
                                  ? loadPropRate(SUSPENDED_HASHTAG_COUNT)
                                  : loadPropRate(NON_SUSPENDED_HASHTAG_COUNT);

    int size = url->size();
    LOG_VAR(count->size());
    LOG_VAR(url->size());
    LOG_VAR(mention->size());
    LOG_VAR(hashtag->size());

    for (int i = 0; i < size; i++) {
        Instance ins;
        ins["__COUNT__"] = count->at(i);
        ins["__URL__"] = url->at(i) / count->at(i);
        ins["__MENTION__"] = mention->at(i) / count->at(i);
        ins["__HASHTAG__"] = hashtag->at(i) / count->at(i);

        ins.setClassValue(isSpammer ? SPAMMER_VALUE : NON_SPAMMER_VALUE);
        ds->addInstance(ins);
    }

    delete count;
    delete url;
    delete mention;
    delete hashtag;

    return ds;
}
