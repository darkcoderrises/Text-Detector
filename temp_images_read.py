from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier
from multiprocessing import Pool
from PIL import Image
import numpy as np
import os
from datetime import datetime

print 

path1 = "images/CLASS1/"
path2 = "images/CLASS3/"
path3 = "images/CLASS2/"

listing1 = os.listdir(path1)
listing2 = os.listdir(path2)
listing3 = os.listdir(path3)

data_x1 = []
data_y1 = []

data_x2 = []
data_y2 = []

def val1_append(l, t):
    data_x1.append(l)
    data_y1.append(t)

def val2_append(l, t):
    data_x2.append(l)
    data_y2.append(t)

def seperate_data(path):
    im = Image.open(path)
    im = im.resize((24, 24), Image.ANTIALIAS)
    data = np.asarray(im, dtype=np.float64).flatten()
    mean = sum(data)/(24*24)
    return (data-mean)/np.var(data)

def process_correct_image(path):
    val1_append(seperate_data(path1 + path), 1)

def process_semi_image(path):
    val2_append(seperate_data(path2 + path), 2)

def process_incorrect_image(path):
    data = seperate_data(path3 + path)
    val1_append(data, 0)
    val2_append(data, 0)

def shuffle(a, b):
    p = np.random.permutation(len(b))
    return np.array(a)[p], np.array(b)[p]

map(process_correct_image, listing1)
map(process_semi_image, listing2)
map(process_incorrect_image, listing3)

data_x1, data_y1 = shuffle(data_x1, data_y1)
data_x2, data_y2 = shuffle(data_x2, data_y2)

seperate = lambda x: (x[:int(0.7*len(x))], x[int(0.7*len(x)):])
data_x1_train, data_x1_test = seperate(data_x1)
data_y1_train, data_y1_test = seperate(data_y1)
data_x2_train, data_x2_test = seperate(data_x2)
data_y2_train, data_y2_test = seperate(data_y2)

time1 = datetime.now()
print "Starting classifier 1", time1
bdt1 = AdaBoostClassifier(DecisionTreeClassifier(max_depth=3), algorithm="SAMME", n_estimators=200)
bdt1.fit(data_x1_train, data_y1_train)

print "Starting classifier 2", datetime.now(), (datetime.now()-time1).total_seconds()
bdt2 = AdaBoostClassifier(DecisionTreeClassifier(max_depth=3), algorithm="SAMME", n_estimators=200)
bdt2.fit(data_x2_train, data_y2_train)

print "Training Ended, took time:", (datetime.now()-time1).total_seconds()
test_x = np.concatenate((data_x1_test, data_x2_test))
test_y = np.concatenate((data_y1_test, data_y2_test))

res1 = bdt1.predict(test_x)
zeros = np.where(res1==0)
res2 = bdt2.predict(test_x[zeros])

print "Before", np.sum(res1 == test_y)/float(len(test_y))
res1[zeros] = res2
print "After", np.sum(res1 == test_y)/float(len(test_y))

