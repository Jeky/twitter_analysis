#include "gtest/gtest.h"

#include "data.h"
#include "ml/ml.h"
#include "utils.h"

TEST(DataTest, User2DatasetTest) {
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

TEST(UtilsTest, Counter) {
    vector<string> words = {"a", "b", "c", "a", "a", "c"};
    Counter<string> c;
    c.count(words);

    vector<pair<string, int>> *result = c.getTop();

    LOG_VAR(*result);

    EXPECT_EQ(result->size(), 3);
    EXPECT_EQ((*result)[0].first, "a");
    EXPECT_EQ((*result)[0].second, 3);
    EXPECT_EQ((*result)[1].first, "c");
    EXPECT_EQ((*result)[1].second, 2);
    EXPECT_EQ((*result)[2].first, "b");
    EXPECT_EQ((*result)[2].second, 1);
}