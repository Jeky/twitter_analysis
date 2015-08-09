#include "data.h"

/*
 * Tweet Member Functions
 */

const String Tweet::TWEET_PATH = PATH + String("tweets/");

Vector<Tweet> Tweet::loadTweets(long id){
    String filename = TWEET_PATH + to_string(id);
    Vector<Tweet> tweets;

    readFile(filename, false, [&tweets](int index, string &line){
        tweets.push_back(Tweet(line));
        return true;
    });

    return tweets;
}


Tweet::Tweet(){};


Tweet::Tweet(String &text){
    this->text = text;
}


Tweet::Tweet(char const *text){
    this->text = text;
}


String Tweet::getText(){
    return text;
}


ostream& operator<< (ostream &out, Tweet &t){
    out << "Tweet[" << t.getText() << "]";
    return out;
}

/*
 * User Member Functions
 */

User::User(){};


User::User(long id, bool spammer){
    this->id = id;
    this->spammer = spammer;
    this->tweets = Tweet::loadTweets(id);
}


long User::getId(){
    return id;
}


bool User::isSpammer(){
    return spammer;
}


Vector<Tweet> User::getTweets(){
    return tweets;
}


ostream& operator<< (ostream &out, User &u){
    out << "User[" << u.getId() << ", Tweets Count = " 
        << u.getTweets().size() << "]";
    return out;
}

/*
 * Data loading functions
 */

Map<long, User> loadData(String &dataPath, String &path, bool spammer){
    ifstream infile(dataPath);

    if(!infile.good()){
        infile.close();

        Map<long, User> users;
        readFile(path, [&](int index, String &line){
            long id = stol(line);
            users[id] = User(id, spammer);
            return true;
        });

        saveObject(users, dataPath);

        return users;
    }else{
        infile.close();

        return loadObject<Map<long, User>>(dataPath);
    }

    return users;
}


Map<long, User> loadSpammers(){
    return loadData(SPAMMER_DATA_PATH, 
        PATH + String("spammers.id.list"), true);
}


Map<long, User> loadNonSpammers(){
    return loadData(NONSPAMMER_DATA_PATH, 
        PATH + String("nonspammers.id.list"), false);
}

