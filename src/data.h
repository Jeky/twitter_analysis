#ifndef _DATA_H_
#define _DATA_H_

#include "utils.h"


static const SPAMMER_DATA_PATH = PATH + String("spammers.obj");
static const NONSPAMMER_DATA_PATH = PATH + String("nonspammers.obj");


class Tweet{
public:
    static const String TWEET_PATH;
    static Vector<Tweet> loadTweets(long id);

    Tweet();
    Tweet(String &text);
    Tweet(char const *text);
    String getText();

    friend ostream& operator<< (ostream &out, Tweet &t);

    template <typename Archive>
    void serialize(Archive &ar){
        ar(text);
    }

private:
    String text;
};


class User{
public:
    User();
    User(long id, bool spammer);
    Vector<Tweet> getTweets();
    long getId();
    bool isSpammer();

    friend ostream& operator<< (ostream &out, User &u);

    template <typename Archive>
    void serialize(Archive &ar){
        ar(id, tweets);
    }

private:
    long id;
    bool spammer;
    Vector<Tweet> tweets;
};


Map<long, User> loadSpammers();
Map<long, User> loadNonSpammers();

#endif
