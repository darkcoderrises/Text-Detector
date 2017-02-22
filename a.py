import matplotlib.pyplot as plt

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

for i in range(len(m)):
    if m[i][1] == 1.0:
        correct_val.append(m[i][0])
    else:
        incorrect_val.append(m[i][0])

plt.plot(correct_val, ones(correct_val), 'go')
plt.plot(incorrect_val, ones(incorrect_val), 'ro')

plt.show()
print len(correct_val)*1.0/(len(correct_val) + len(incorrect_val))
