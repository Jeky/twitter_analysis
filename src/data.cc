#include "data.h"


Vector<Tweet> Tweet::loadTweets(long id){
    String filename = TWEET_PATH + to_string(id);
    Vector<Tweet> tweets;

    readFile(filename, [tweets](int index, string &line){
        tweets.push_back(Tweet(line));
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
    out << "Tweet[" << t.getText() << "]" << endl;
    return out;
}


User::User(){};


User::User(long id){
    this->id = id;
    this->tweets = Tweet::loadTweets(id);
}


long User::getId(){
    return id;
}


Vector<Tweet> User::getTweets(){
    return tweets;
}


ostream& operator<< (ostream &out, User &u){
    out << "User[" << u.getId() << ", Tweets Count = " 
        << u.getTweets().size() << "]" << endl;
    return out;
}