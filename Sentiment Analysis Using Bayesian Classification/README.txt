Sentiment Analysis using Naive Bayes Classification in Python.
Using a large collection of positive and negative movie reviews,  
the Naive Bayes classifier model predicts the sentiment of the corresponding movie review.
Unigram features are used in the bayes.py implementation, and the feature space is extended further   
using Bigrams in the bayesbest.py file.   

Results:
---------
Original Bayes implementation achieved F1 measure of 0.932 given testing set.
Optimized Bayes implementation using Bigrams achieved F1 measure of 0.942 given testing set.

How to Run:
-----------   
Simply Run: "python bayes.py" or "python bayesbest.py" and the program will start training the classifier using the all moview reviews found in the training folder. Training is only performed once and will save the output to a file, shown as neg_words, pos_words, neg_words_best, and pos_words_best, which are the saved extracted unigram/bigram words found along with the number of occurences. Hash Table data structure is used for this storage.

NOTE: unzip the training and testing folders for the program to be able to access the training and testing sets.

