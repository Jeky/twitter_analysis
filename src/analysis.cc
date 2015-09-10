#include "data.h"
#include "ml/text.h"
#include "ml/classifier.h"
#include "ml/evaluator.h"
#include "ml/features.h"

#include "utils.h"
#include <iterator>

void convertToDS() {
	auto *spammers = loadSpammers();
	auto *nonSpammers = loadSampledNonSpammers();

	LOG("User loaded. Convert users to Dataset...");

	auto *spammerDS = user2Dataset(spammers);
	delete spammers;

	auto *nonSpammerDS = user2Dataset(nonSpammers);
	delete nonSpammers;

	LOG("Save datasets...");
	saveObject(spammerDS, SPAMMER_DS);

	delete spammerDS;

	saveObject(nonSpammerDS, NON_SPAMMER_DS);
	delete nonSpammerDS;
}

Counter<string> *countTokens(unordered_map<long, User> *users,
		const string &path) {
	ifstream infile(path);

	if (!infile.good()) {
		LOG("Cannot Find ", path, ". Counting Tokens...");
		infile.close();

		Counter < string > *tokenCounter = new Counter<string>();
		int i = 0;
		for (auto &kv : *users) {
			if (i % 1000 == 0) {
				LOG("Process ", i, " users");
			}
			for (auto &t : kv.second.getTweets()) {
				vector < string > *words = toGrams(t.getText());
				tokenCounter->count(words);
				delete words;
			}
			i++;
		}

		saveObject(tokenCounter, path);
		return tokenCounter;
	} else {
		LOG("Loading Data from ", path);
		infile.close();

		return loadObject < Counter < string >> (path);
	}
}

double collectAUCP(unordered_map<long, User> *users) {
	double aucp = 0.0;
	for (auto &kv : *users) {
		int cut = 0;
		for (auto &t : kv.second.getTweets()) {
			if (t.containsUrl()) {
				cut++;
			}
		}
		aucp += 1.0 * cut / kv.second.getTweets().size();
	}

	aucp /= users->size();

	return aucp;
}

double collectARP(unordered_map<long, User> *users) {
	double arp = 0.0;
	for (auto &kv : *users) {
		int r = 0;
		for (auto &t : kv.second.getTweets()) {
			if (t.isRetweet()) {
				r++;
			}
		}
		arp += 1.0 * r / kv.second.getTweets().size();
	}

	arp /= users->size();

	return arp;
}

void countByLen(const Counter<string> *counter, const string &path) {
	LOG("Counting Tokens by Length into ", path);
	Counter<int> lenCounter;
	counter->eachEntry([&](const string &key, const int &value) {
		lenCounter.count(key.size(), value);
	});

	auto *top = lenCounter.getTop();
	writeFile(path, [&](ofstream &out) {
		for (auto &i : *top) {
			out << i.first << "\t" << i.second << endl;
		}
	});
}

int countTweets(const unordered_map<long, User> *users) {
	int count = 0;
	for (auto & kv : *users) {
		count += kv.second.getTweets().size();
	}
	return count;
}

void printDatasetStatistic() {
	auto *spammers = loadSpammers();
	auto *nonSpammers = loadSampledNonSpammers();

	LOG_VAR(spammers->size());
	LOG_VAR(nonSpammers->size());
	LOG_VAR(countTweets(spammers));
	LOG_VAR(countTweets(nonSpammers));

	// average url contain percentage definition:
	// aucp = user.containUrlTweets.size() / user.getTweets().size()
	LOG("Counting Average URL Contains Percentage");
	LOG_VAR(collectAUCP(spammers));
	LOG_VAR(collectAUCP(nonSpammers));

	// average retweet percentage definition:
	// arp = user.retweets.size() / user.getTweets().size()
	LOG("Counting Average Retweet Percentage");
	LOG_VAR(collectARP(spammers));
	LOG_VAR(collectARP(nonSpammers));

	LOG("Counting Tokens in Tweets from Spammers");
	auto *spammerTokenCounter = countTokens(spammers, SPAMMER_TOKEN_COUNTER);

	LOG("Counting Tokens in Tweets from NonSpammers");
	auto *nonSpammerTokenCounter = countTokens(nonSpammers,
			NON_SPAMMER_TOKEN_COUNTER);

	LOG("Counting All Tokens in Tweets");
	auto *allTokenCounter = countTokens(spammers, SPAMMER_TOKEN_COUNTER);
	allTokenCounter->addCounter(*nonSpammerTokenCounter);

	delete spammers;
	delete nonSpammers;

	countByLen(spammerTokenCounter, PATH + "spammer-len-dist.txt");
	countByLen(nonSpammerTokenCounter, PATH + "non-spammer-len-dist.txt");
	countByLen(allTokenCounter, PATH + "all-len-dist.txt");

	auto *spammerTokens = spammerTokenCounter->getKeySet();
	auto *nonSpammerTokens = nonSpammerTokenCounter->getKeySet();
	auto *sharedTokens = setIntersection(spammerTokens, nonSpammerTokens);
	auto *allTokens = setUnion(spammerTokens, nonSpammerTokens);

	LOG("Before removing stops");
	LOG_VAR(spammerTokens->size());
	LOG_VAR(nonSpammerTokens->size());
	LOG_VAR(sharedTokens->size());
	LOG_VAR(allTokens->size());

	delete spammerTokens;
	delete nonSpammerTokens;
	delete sharedTokens;
	delete allTokens;

	spammerTokenCounter->saveFrequency(SPAMMER_TOKEN_FREQ);
	nonSpammerTokenCounter->saveFrequency(NON_SPAMMER_TOKEN_FREQ);
	allTokenCounter->saveFrequency(ALL_TOKEN_FREQ);

	LOG("After removing stops");
	auto *stops = loadStops();
	for (auto &k : *stops) {
		spammerTokenCounter->deleteKey(k);
		nonSpammerTokenCounter->deleteKey(k);
		allTokenCounter->deleteKey(k);
	}

	countByLen(spammerTokenCounter, PATH + "spammer-no-stops-len-dist.txt");
	countByLen(nonSpammerTokenCounter,
			PATH + "non-spammer-no-stops-len-dist.txt");
	countByLen(allTokenCounter, PATH + "all-no-stops-len-dist.txt");

	spammerTokens = spammerTokenCounter->getKeySet();
	nonSpammerTokens = nonSpammerTokenCounter->getKeySet();
	sharedTokens = setIntersection(spammerTokens, nonSpammerTokens);
	allTokens = setUnion(spammerTokens, nonSpammerTokens);

	LOG_VAR(spammerTokens->size());
	LOG_VAR(nonSpammerTokens->size());
	LOG_VAR(sharedTokens->size());
	LOG_VAR(allTokens->size());

	delete stops;

	delete spammerTokenCounter;
	delete nonSpammerTokenCounter;
	delete allTokenCounter;
}

void testClassification() {
	auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
	auto *nonSpammerDS = Dataset::loadDataset(NON_SPAMMER_DS,
			NON_SPAMMER_VALUE);

	Classifier *cls = new NaiveBayes();
	Evaluator eval;

	eval.crossValidate(10, cls, spammerDS, nonSpammerDS);
	for (auto &item : eval.getConfusionMatrixVector()) {
		LOG(item);
	}
	LOG_VAR(eval.getAccuracy());
	LOG_VAR(eval.getRecall());
	LOG_VAR(eval.getPrecision());
	LOG_VAR(eval.getF1());

	delete cls;
	delete spammerDS;
	delete nonSpammerDS;
}

void testFeatureSelection() {
	auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
	auto *nonSpammerDS = Dataset::loadDataset(NON_SPAMMER_DS,
			NON_SPAMMER_VALUE);

	auto *all = spammerDS;
	all->addDataset(*nonSpammerDS);
	delete nonSpammerDS;

	auto *selector = new BiClassMutualInformation();
	selector->train(all);

	vector<pair<string, double>> *result = selector->getTopFeatureList();
	writeFile(PATH + "feature.txt", [&](ofstream &out) {
		for (auto &r : *result) {
			out << r.first << "\t" << r.second << endl;
		}
	});

	delete selector;
	delete all;
}

int main(int argc, char const *argv[]) {
	printDatasetStatistic();
	return 0;
}
