#include "gtest/gtest.h"

#include "data.h"
#include "ml/ml.h"
#include "ml/text.h"
#include "utils.h"

TEST(DataTest, user2Dataset) {
    // construct user list
    unordered_map<long, User> *users = new unordered_map<long, User>();
    for (int i = 0; i < 10; i++) {
        (*users)[i] = User(i, true);
        (*users)[i].getTweets().push_back(Tweet("aaaaaa"));
    }
    for (int i = 10; i < 20; i++) {
        (*users)[i] = User(i, false);
        (*users)[i].getTweets().push_back(Tweet("bbbbbb"));
    }

    // convert to dataset
    Dataset *ds = user2Dataset(users, 1);

    EXPECT_EQ(ds->size(), 20);

    Instance instance = ds->at(0);
    if (instance.hasAttribute("aaaaaa")) {
        EXPECT_EQ(instance["aaaaaa"], 1);
        EXPECT_FALSE(instance.hasAttribute("bbbbbb"));
    }
    if (instance.hasAttribute("bbbbbb")) {
        EXPECT_EQ(instance["bbbbbb"], 1);
        EXPECT_FALSE(instance.hasAttribute("aaaaaa"));
    }

    delete users;
    delete ds;
}

TEST(DataTest, Tweet_isRetweet) {
    vector<Tweet> trueVectors = {
        Tweet("RT @xxxx:this is a retweet."),
        Tweet("RT without @ can be still seen as a retweet"),
        Tweet("rt test lower case version")};

    vector<Tweet> falseVectors = {Tweet("this isn\'t a retweet."), Tweet("")};

    for (auto &t : trueVectors) {
        EXPECT_TRUE(t.isRetweet());
    }

    for (auto &t : falseVectors) {
        EXPECT_FALSE(t.isRetweet());
    }
}

TEST(DataTest, Tweet_containsUrl) {
    Tweet t1("contains url http://some.where/blah/blah");
    Tweet t2("contains url http://some.where");
    Tweet t3("contains url https://some.where/blah/blah");
    Tweet t4("contains url https://some.where");
    Tweet t5("doesn't contain url");
    Tweet t6("doesn't contain url. but contains a fake url some.where.fake");
    Tweet t7("");

    EXPECT_TRUE(t1.containsUrl());
    EXPECT_TRUE(t2.containsUrl());
    EXPECT_TRUE(t3.containsUrl());
    EXPECT_TRUE(t4.containsUrl());
    EXPECT_FALSE(t5.containsUrl());
    EXPECT_FALSE(t6.containsUrl());
    EXPECT_FALSE(t7.containsUrl());
}

TEST(UtilsTest, Counter) {
    vector<string> words = {"a", "b", "c", "a", "a", "c"};
    Counter<string> c;
    c.count(words);

    vector<pair<string, int>> *result = c.getTop();

    EXPECT_EQ(result->size(), 3);
    EXPECT_EQ((*result)[0].first, "a");
    EXPECT_EQ((*result)[0].second, 3);
    EXPECT_EQ((*result)[1].first, "c");
    EXPECT_EQ((*result)[1].second, 2);
    EXPECT_EQ((*result)[2].first, "b");
    EXPECT_EQ((*result)[2].second, 1);
}

TEST(UtilsTest, SetOperation) {
    unordered_set<int> *s1 = new unordered_set<int>();
    s1->insert(1);
    s1->insert(2);
    s1->insert(3);

    unordered_set<int> *s2 = new unordered_set<int>();
    s2->insert(2);
    s2->insert(3);
    s2->insert(4);

    unordered_set<int> *s1Copy = setIntersection(s1, s1);
    unordered_set<int> *intersect = setIntersection(s1, s2);
    unordered_set<int> *s1CopyCopy = setUnion(s1, s1);
    unordered_set<int> *unionSet = setUnion(s1, s2);

    EXPECT_EQ(s1Copy->size(), 3);
    EXPECT_EQ(intersect->size(), 2);
    EXPECT_EQ(s1CopyCopy->size(), 3);
    EXPECT_EQ(unionSet->size(), 4);

    delete s1;
    delete s2;
    delete s1Copy;
    delete intersect;
    delete s1CopyCopy;
    delete unionSet;
}

TEST(TextTest, splitWords) {
    string s = "This is a test. http://aaa.bbb.com/cc will be tokenized as one "
               "word. !!!! and ??? will be removed. @username #hashtag are "
               "also a token.";
    vector<string> *words = splitWords(s);
    vector<string> expectedWords = {
        "This",      "is",       "a",         "test", "http://aaa.bbb.com/cc",
        "will",      "be",       "tokenized", "as",   "one",
        "word",      "and",      "will",      "be",   "removed",
        "@username", "#hashtag", "are",       "also", "a",
        "token"};

    EXPECT_EQ(expectedWords.size(), words->size());

    for (int i = 0; i < words->size(); i++) {
        EXPECT_EQ(expectedWords[i], words->at(i));
    }

    delete words;
}

TEST(TextTest, filterStopWords) {
    auto *tokens =
        toGrams("This is a test and the stops words will be removed. URL "
                "http://aaa.bbb.ccc won't be removed..... @username!!!");
    auto *stops = loadStops();

    auto *result = filterStopWords(tokens, stops);
    vector<string> expectedWords = {"test",  "stop",  "word",
                                    "remov", "url",   "http://aaa.bbb.ccc",
                                    "won",   "remov", "@username"};

    for (int i = 0; i < result->size(); i++) {
        EXPECT_EQ(expectedWords[i], result->at(i));
    }

    delete tokens;
    delete stops;
    delete result;
}
