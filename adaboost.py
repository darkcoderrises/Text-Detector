from sklearn.tree import DecisionTreeClassifier
from PIL import Image
import os
import sys
import numpy as np
import _pickle as pickle

path1 = "images/TCLASS1/"
path2 = "images/TCLASS2/"
path3 = "images/BJCLASS1/"
path4 = "images/CHARCLASS1/"

data_x = [[[] for i in range(24)] for j in range(24)]
data_y = []

thr = 0.06
ada_thr = -2.5

direc = [(0,0), (0,1), (0,2), (1,2), (2,2), (2,1), (2,0), (1,0)]
def calcMLBP(arr):
    mean = (np.mean(arr) * 9 - arr[1,1])/8

    res = 0
    for i in direc[::-1]:
        res = (res << 1) + int(mean<arr[i])
    return res

def image_read(path):
    im = Image.open(path)
    im = im.resize((26, 26), Image.ANTIALIAS)
    data = np.asarray(im)
    res = np.zeros((24,24))

    for i in range(24):
        for j in range(24):
            res[i,j] = calcMLBP(data[i:i+3, j:j+3])
    
    return res

def parse_image(path, val):
    for i, item in enumerate(image_read(path)):
        for j, value in enumerate(item):
            data_x[i][j].append([value])

    data_y.append(val)

class DTC(DecisionTreeClassifier):
    def predict_thresh(self, x, thresh):
        return np.sign(self.predict_proba(x)[:,1] - 0.5 - thresh)

class AdaBoost:
    def __init__(self):
        self.training_set = data_x
        self.training_labels = data_y
        self.N = len(data_y)
        self.weights = np.ones((self.N, 1))/self.N        
        self.rules = []
        self.alpha = []
        self.res = np.ones(self.N) * ada_thr 

    def set_rule(self, clf, i, j):
        errors = (clf.predict_thresh(self.training_set[i][j], thr) 
                != self.training_labels)
        e = np.dot(errors, self.weights).sum()

        alpha = 0.5 * np.log((1-e)/e) 
        w = np.zeros((self.N, 1))
        
        for l in range(self.N):
            if errors[l]: w[l] = self.weights[l] * np.exp(alpha)
            else: w[l] = self.weights[l] * np.exp(-alpha)

        self.weights = w / w.sum()
        self.rules.append((clf, i, j))
        self.alpha.append(alpha)

    def evaluate(self):
        NR = len(self.rules)
        res = np.ones(self.N) * ada_thr

        for ind in range(NR):
            clf, i, j = self.rules[ind]
            alpha = self.alpha[ind]
            res += alpha * clf.predict_thresh(self.training_set[i][j], thr)

        vals = (self.training_labels == np.sign(res))
        print (sum(vals)/float(len(vals)), calc_precision(np.sign(res)),)
        print (calc_recall(np.sign(res)))

    def evaluate_one(self):
        ind = len(self.rules) - 1
        clf, i, j = self.rules[ind]
        alpha = self.alpha[ind]
        self.res += alpha * clf.predict_thresh(self.training_set[i][j], thr)
        vals = (self.training_labels == np.sign(self.res))
        
        precision = calc_precision(np.sign(self.res))
        accuracy = sum(vals)/float(len(vals))

        #print accuracy, precision
        return accuracy, precision
        
        
if __name__ == "__main__":

#    with open('adaboost_mine.pkl', 'wb') as fid:
#        for i in os.listdir(path4):
#            parse_image(path4+i, +1)
#        for i in os.listdir(path3):
#            parse_image(path3+i, +1)
#        for i in os.listdir(path2):
#            parse_image(path2+i, -1)
#        for i in os.listdir(path1):
#            parse_image(path1+i, +1)
#        
##        map(lambda i: parse_image(path4 + i, +1), os.listdir(path4))
##        map(lambda i: parse_image(path3 + i, +1), os.listdir(path3))
##        map(lambda i: parse_image(path2 + i, -1), os.listdir(path2))
##        map(lambda i: parse_image(path1 + i, +1), os.listdir(path1))
#        print ("Data read")
#    
#        for i in range(24):
#            for j in range(24):
#                pickle.dump(data_x[i][j], fid)
#        pickle.dump(data_y, fid)

    with open('adaboost_mine.pkl', 'rb') as fid:
        for i in range(24):
            for j in range(24):
                data_x[i][j] = pickle.load(fid)
        data_y = pickle.load(fid)

    data_y = np.asarray(data_y)
    classifiers = []
    classification_accuracy = []

    def calc_precision(val):
        my = np.where(val == 1)[0]
        both_same = np.where(np.logical_and(val == 1, data_y == 1))[0]
        if len(my) == 0:
            return 0
        return len(both_same)/float(len(my))

    def calc_recall(val):
        both_same = len(np.where(np.logical_and(val == 1, data_y == 1))[0])
        incorrect = len(np.where(np.logical_and(val == -1,data_y == 1))[0])

        return float(both_same)/(both_same+incorrect)

    for i in range(24):
        for j in range(24):
            clf = DTC(max_depth=500)
            clf.fit(data_x[i][j], data_y)
            predictions = clf.predict_thresh(data_x[i][j], thr)
            t = (predictions == data_y)
            val = np.sum(t)/float(len(data_y))

#            const = 101
#            true_positive = np.zeros(const)
#            false_positive = np.zeros(const)
#
#            probs = 100 * clf.predict_proba(data_x[i][j])[:,1]
#            probs = np.asarray(probs, dtype=np.int)
#            
#            true_p = np.where(np.logical_and(predictions==1, data_y==+1))
#            fals_p = np.where(np.logical_and(predictions==1, data_y==-1))
#            
#            for ind in probs[true_p]:
#                true_positive[ind] += 1
#
#            for ind in probs[fals_p]:
#                false_positive[ind] += 1
#
#            import matplotlib.pyplot as plt
#            fig = plt.figure()
#            ax = fig.add_subplot(111)
#            ind = np.arange(const)
#            
#            width = 0.2
#            rects1 = ax.bar(ind, true_positive, width, color='g')
#            reacts = ax.bar(ind + width, false_positive, width, color='r')
#            plt.show()
#
#            sys.exit(0)

            classifiers.append((clf, i, j))
            classification_accuracy.append(val)
            
    classification_accuracy = np.asarray(classification_accuracy)
    classifiers = np.asarray(classifiers)
    inds = classification_accuracy.argsort()
    sortedClassifiers = classifiers[inds]

#for thr in np.arange(0.0, 0.1, 0.01): 
#for ada_thr in np.arange(0, 2, 0.2):
#print thr, ada_thr,
    adaboost = AdaBoost()
    for i, classifier in enumerate(classifiers[::-1]):
        adaboost.set_rule(*classifier)
        #acc, prec = adaboost.evaluate_one()
                    
    adaboost.evaluate()

    with open('trained_adaboost_precision.pkl', 'wb') as fid:
        pickle.dump(adaboost.rules, fid)
        pickle.dump(adaboost.alpha, fid)
