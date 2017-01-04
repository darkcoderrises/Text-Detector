import numpy as np
from PIL import Image, ImageDraw
from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier

f = lambda f: filter(bool, open(f).read().split('\n'))

result_x = f('Result/result_x')
result_y = f("Result/result_y")
boundary = f('Result/boundary')

train_x = f('train_x')
train_y = f('train_y')

train = lambda x: np.array([filter(bool, i.split()) for i in x])
test = lambda x: np.array(x)

X_train = train(train_x)
Y_train = test(train_y)

X_test = train(result_x)
Y_test = test(result_y)

bdt = AdaBoostClassifier(DecisionTreeClassifier(max_depth=10), algorithm="SAMME.R", n_estimators=2000, learning_rate=0.95)
bdt.fit(X_train, Y_train)
Y = bdt.predict(X_test)

print float(sum(Y==Y_test))/(np.shape(Y_test)[0])

im = Image.open("database/100.jpg")
dr = ImageDraw.Draw(im)

#for i in range(len(boundary)):
#	m = boundary[i]
#	k = Y[i]
#
#	o = ''
#	if k==1:
#		o = 'green'
#	else:
#		o = 'red'
#
#	dr.rectangle(tuple(map(int, m.split())), outline=o)
#im.show()
