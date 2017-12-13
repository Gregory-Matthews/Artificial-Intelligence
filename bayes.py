# CS 510: Homework 2
# Naive Bayesian Classification with Unigram features
# By Greg Matthews
# 11/21/17

import math, os, pickle, re

pos_words_filename = "pos_words"
neg_words_filename = "neg_words"


class Bayes_Classifier:
    def __init__(self, trainDirectory="movies_reviews/"):
        '''This method initializes and trains the Naive Bayes Sentiment Classifier.  If a 
      cache of a trained classifier has been stored, it loads this cache.  Otherwise, 
      the system will proceed through training.  After running this method, the classifier 
      is ready to classify input text.'''
        self.trainDirectory = trainDirectory
        # Holds positive review words
        self.pos_words = {}
        # Holds negative review words
        self.neg_words = {}

        # Check if trained dictionaries already exist
        if os.path.isfile(pos_words_filename) and os.path.isfile(neg_words_filename):
            self.pos_words = self.load(pos_words_filename)
            self.neg_words = self.load(neg_words_filename)

        # If not, train
        else:
            self.train()

    def train(self):
        '''Trains the Naive Bayes Sentiment Classifier.'''
        iFileList = []
        for fFileObj in os.walk(self.trainDirectory):
            iFileList = fFileObj[2]
            break

        # Tokenize every files string and add them to appropriate dictionary
        for f in iFileList:
            text = self.loadFile(self.trainDirectory + f)
            tokens = self.tokenize(text)

            # File is a negative review
            if f[7] == '1':
                words = self.neg_words

            # File is a positive review
            elif f[7] == '5':
                words = self.pos_words

            # Create dictionary
            for word in tokens:
                if words.has_key(word):
                    words[word] += 1
                else:
                    words[word] = 1

        # Save to file using pickle
        self.save(self.neg_words, neg_words_filename)
        self.save(self.pos_words, pos_words_filename)

    def classify(self, sText):
        '''Given a target string sText, this function returns the most likely document
      class to which the target string belongs. This function should return one of three
      strings: "positive", "negative" or "neutral".
      '''
        # Initialize variables
        neg_reviews = 0
        pos_reviews = 0
        documents = []

        # Find num of pos & neg reviews
        for fFileObj in os.walk(self.trainDirectory + "/"):
            documents = fFileObj[2]
            break
        for document in documents:
            # File is a negative review
            if document[7] == '1':
                neg_reviews += 1

            # File is a positive review
            elif document[7] == '5':
                pos_reviews += 1

        # Probability of being a pos/neg review
        p_pos = float(pos_reviews) / (neg_reviews + pos_reviews)
        p_neg = float(neg_reviews) / (neg_reviews + pos_reviews)

        # Probability of feature f given pos/neg review
        p_f_pos = 0.0
        p_f_neg = 0.0

        # Find sum of all frequency of features
        freq_total_pos = 0.0
        freq_total_neg = 0.0
        for key, val in self.pos_words.iteritems():
            freq_total_pos += val
        for key, val in self.neg_words.iteritems():
            freq_total_neg += val

        # Get all features from the text file
        features = self.tokenize(sText)

        # Calculating product of conditional probability
        for feature in features:
            if self.pos_words.has_key(feature):
                freq_feature_pos = self.pos_words[feature]
            # Add 1 smoothing
            else:
                freq_feature_pos = 1

            if self.neg_words.has_key(feature):
                freq_feature_neg = self.neg_words[feature]
            # Add 1 smoothing
            else:
                freq_feature_neg = 1

            # Calculate conditional probabilities
            p_f_pos += math.log10(float(freq_feature_pos) / freq_total_pos)
            p_f_neg += math.log10(float(freq_feature_neg) / freq_total_neg)

        # Add up total logs of conditional probability and add prior probability
        p_pos_f = math.log10(p_pos) + p_f_pos
        p_neg_f = math.log10(p_neg) + p_f_neg

        #print p_pos_f, p_neg_f
        #print len(self.pos_words)

        # variable alpha used for intensity of classifying neutral
        alpha = 0.0025

        if p_pos_f < p_neg_f * (1 - alpha) and p_pos_f > p_neg_f * (1 + alpha):
            return "neutral"

        elif p_pos_f > p_neg_f:
            return "positive"
        elif p_pos_f < p_neg_f:
            return "negative"

    def loadFile(self, sFilename):
        '''Given a file name, return the contents of the file as a string.'''

        f = open(sFilename, "r")
        sTxt = f.read()
        f.close()
        return sTxt

    def save(self, dObj, sFilename):
        '''Given an object and a file name, write the object to the file using pickle.'''

        f = open(sFilename, "w")
        p = pickle.Pickler(f)
        p.dump(dObj)
        f.close()

    def load(self, sFilename):
        '''Given a file name, load and return the object stored in the file.'''

        f = open(sFilename, "r")
        u = pickle.Unpickler(f)
        dObj = u.load()
        f.close()
        return dObj

    def tokenize(self, sText):
        '''Given a string of text sText, returns a list of the individual tokens that 
      occur in that string (in order).'''

        lTokens = []
        sToken = ""
        for c in sText:
            if re.match("[a-zA-Z0-9]", str(c)) != None or c == "\'" or c == "_" or c == '-':
                sToken += c
            else:
                if sToken != "":
                    lTokens.append(sToken)
                    sToken = ""
                if c.strip() != "":
                    lTokens.append(str(c.strip()))

        if sToken != "":
            lTokens.append(sToken)

        return lTokens
