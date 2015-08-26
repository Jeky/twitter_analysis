#ifndef _DATA_H_
#define _DATA_H_

#include "utils.h"
#include "ml/ml.h"
#include "ml/text.h"

static const string SPAMMER_DATA_PATH = PATH + string("spammers.obj");
static const string NONSPAMMER_DATA_PATH = PATH + string("nonspammers.obj");

static const double SPAMMER_VALUE = 1.0;
static const double NON_SPAMMER_VALUE = 0.0;


class Tweet {
public:
    static const string TWEET_PATH;

    Tweet() { }

    Tweet(string const &text) { this->text = text; }

    Tweet(char const *text) { this->text = text; }

    string getText() const { return text; }

    friend ostream &operator<<(ostream &out, Tweet &t);

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(text);
    }

private:
    string text;
};


class User {
public:
    User() { };

    User(long id, bool spammer) {
        this->id = id;
        this->spammer = spammer;
    }

    long getId() const {
        return id;
    }

    void setId(long id) {
        User::id = id;
    }

    bool isSpammer() const {
        return spammer;
    }

    void setSpammer(bool spammer) {
        User::spammer = spammer;
    }

    vector<Tweet> &getTweets() {
        return tweets;
    }

    void loadTweets();

    friend ostream &operator<<(ostream &out, User &u);

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(id, tweets);
    }

private:
    long id;
    bool spammer;
    vector<Tweet> tweets;
};


unordered_map<long, User> *loadSpammers();

unordered_map<long, User> *loadNonSpammers();

Dataset *user2Dataset(unordered_map<long, User> *users, int gramLen);

#endif
