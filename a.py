import matplotlib.pyplot as plt
from math import floor
import numpy as np


def ones(m):
    l = []
    for i in m:
        l.append(-1)
    return l

f = open("in").read()
g = filter(bool, f.split("\n"))
m = [i.split(" ") for i in g]

for i in range(len(m)):
    for j in range(2):
        m[i][j] = float(m[i][j])

correct_val = []
incorrect_val = []
const = 300

val_g = [0 for i in range(const)]
val_r = [0 for i in range(const)]

fig = plt.figure()
ax = fig.add_subplot(111)

ind = np.arange(const) * 3

for i in range(len(m)):
    index = int(floor((250 + m[i][0])*1)) 
    if m[i][1] == 1.0:
        correct_val.append(m[i][0])
        val_g[index] += 1
    else:
        incorrect_val.append(m[i][0])
        val_r[index] += 1

width = 1
rects1 = ax.bar(ind, val_g, width, color='g')
rects2 = ax.bar(ind + width, val_r, width, color='r')


plt.show()
print len(correct_val)*1.0/(len(correct_val) + len(incorrect_val))
