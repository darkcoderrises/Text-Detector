import sys
import os
import numpy as np
import _pickle as pickle

import warnings
warnings.filterwarnings("ignore")
from adaboost import DTC

rules_recall = []
alpha_recall = []

rules_precision = []
alpha_precision = []

precision_thr, precision_res = 0.06, -2.4
recall_thr, recall_res = 0.05, 1.8

def load():
    global rules_precision, alpha_precision
    global rules_recall, alpha_recall
    with open('trained_adaboost_recall.pkl', 'rb') as fid:
        rules_recall = pickle.load(fid)
        alpha_recall = pickle.load(fid)

    with open('trained_adaboost_precision.pkl', 'rb') as fid:
        rules_precision = pickle.load(fid)
        alpha_precision = pickle.load(fid)

def predict(arg):
    import numpy as np
    arg = np.asarray(arg, dtype=np.float32).reshape((24,24))
    res = precision_res
    for ind, rule in enumerate(rules_precision):
        clf, i, j = rule
        val = np.asarray([int(arg[i][j])]).reshape(-1,1)
        res += alpha_precision[ind]*clf.predict_thresh(val, precision_thr)

    if  np.sign(res) == 1:
        return 1

    res = recall_res
    for ind, rule in enumerate(rules_recall):
        clf, i, j = rule
        val = np.asarray([int(arg[i][j])]).reshape(-1,1)
        res += alpha_recall[ind] * clf.predict_thresh(val, recall_thr)

    result = np.sign(res)
    
    if result == 1:
        result *= 2

    return int(result)

