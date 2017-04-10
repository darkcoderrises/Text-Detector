import sys
import os
import numpy as np
import cPickle as pickle
from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

bdt = None
with open('adaboost_classifier.pkl', 'rb') as fid:
    bdt = pickle.load(fid)

arg = np.asarray(sys.argv[1:])
print bdt.predict(arg)[0]
