from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier
from multiprocessing import Pool
from PIL import Image
import numpy as np
import os

path1 = "images/i1/"
path2 = "images/i2/"

listing1 = os.listdir(path1)
listing2 = os.listdir(path2)

data_x = []
data_y = []

def val_append(l, t):
    data_x.append(l)
    data_y.append(t)

def process_correct_image(path):
    print "correct", path
    im = Image.open(path1 + path)
    im = im.resize((24, 24), Image.ANTIALIAS)
    data = np.asarray(im, dtype=np.float32).flatten()
    mean = sum(data)/(24*24)
    l = (data-mean)/np.var(data)

    val_append(l, 1)

def process_incorrect_image(path):
    print "incorrect", path
    im = Image.open(path2 + path)
    im = im.resize((24, 24), Image.ANTIALIAS)
    data = np.asarray(im, dtype=np.float32).flatten()
    mean = sum(data)/(24*24)
    l = (data-mean)/np.var(data)

    val_append(l, 0)

map(process_correct_image, listing1)
map(process_incorrect_image, listing2)

inds = int(0.7*len(data_y))

p = np.random.permutation(len(data_y))
data_x = np.array(data_x)[p]
data_y = np.array(data_y)[p]

train_x = data_x[:inds]
train_y = data_y[:inds]

test_x = data_x[inds:]
test_y = data_y[inds:]

bdt = AdaBoostClassifier(DecisionTreeClassifier(max_depth=3), algorithm="SAMME", n_estimators=200)
bdt.fit(train_x, train_y)
y = bdt.predict(test_x)

y = np.array(y)
test_y = np.array(test_y)

count = 0
for i in range(len(y)):
    print y[i], test_y[i]
    if y[i] == test_y[i]:
        count += 1

print count/float(len(y))
