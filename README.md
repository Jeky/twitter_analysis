#Twitter Analysis Tools
This is a c++ version of my thesis toolkit.

## Building
This project is based on cmake and clang++. So when building, you need to first install clang and cmake to make sure you can compile the source code.

## Run
First download dataset [here](http://cui111.myweb.cs.uwindsor.ca/static/dataset.tar.gz) and unzip it. In the unzip folder, make two folders named **mnb** and *bnb*. Next modify the value of PATH in include/utils.h
```
static const string PATH = string("/PATH/TO/YOUR/FOLDER/");
```

And then make sure you installed cmake and make clang++ as your default compiler. Then 
```
./run.sh
```

to run the programs.

## Intermediate Outputs
After running, This program will generate several intermediate outputs in your folder.

* suspended-*.txt and non-suspended-*.txt : these files are generated by analyzeDataset() in src/analysis.cc, including the statistic of token frequency, tweet count, retweet count, mention count, url count, hashtag count, tweet length and token count per user.

* suspended.dat / non-suspended.dat : these two files are saved binary files of whole datasets.

* MNB-full-feature-cv-train-result-*.txt and BNB-full-feature-cv-train-result-*.txt : the data of trained classifiers. The formation of these files is:

```
FEATURE \t COUNT_IN_SUSPENDED \t COUNT_IN_NON_SUSPENDED \t PROB_IN_SUSPENDED \t PROB_IN_NON_SUSPENDED
```

* mnb/* : the classified data of Multinomial Naive Bayes classifier. Formation:

```no-highlight
Label = 0
Prior = ....
FEATURE_1 \t COUNT_IN_USER \t WORD_PROB
FEATURE_2 \t COUNT_IN_USER \t WORD_PROB
...
Prob = ....

Label = 1
Prior = ....
FEATURE_1 \t COUNT_IN_USER \t WORD_PROB
FEATURE_2 \t COUNT_IN_USER \t WORD_PROB
...
Prob = ....
```

The value of label: 1 means suspended and 0 means non-suspended. COUNT_IN_USER is 0 represents that this word cannot be found in training vocabulary.

* bnb/* : the classified data of Bernoulli Naive Bayes classifier. Formation:

```no-highlight
Label = 0
Total False = ....
FEATURE_1 \t WORD_PROB
FEATURE_2 \t WORD_PROB
...
Prob = ....

Label = 1
Total False = ....
FEATURE_1 \t WORD_PROB
FEATURE_2 \t WORD_PROB
...
Prob = ....
```
The value of label: 1 means suspended and 0 means non-suspended. WORD_PROB is 0 represents that this word cannot be found in training vocabulary.

* mi-features-*.txt / chi2-features-*.txt / wapmi-features-*.txt : the scores of features. Formation:
```
FEATURE \t SCORE
```

* mi-merged-*-ds.txt / chi2-merged-*-ds.txt / chi2-merged-*-ds.txt : the details of feature selection. Formation:
```
FEATURE \t N11 \t N10 \t N01 \t N00 \t SCORE
```

* mi-results-*.txt / chi2-results-*.txt / chi2-results-*.txt : the details of feature selection. Formation:
```
size = ...
CONFUSION_MATRIX_1
CONFUSION_MATRIX_2
...
CONFUSION_MATRIX_10
acc: ACCURACY
rec: RECALL
pre: PRECISION
F1:  F1_VALUE
...
```

* log.txt : this file is the log of program, which is located inside the program folder.

