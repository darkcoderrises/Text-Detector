import numpy as np
from PIL import Image, ImageDraw
from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import GradientBoostingClassifier

f = lambda f: filter(bool, open(f).read().split('\n'))

result_x = f('Result_total/result_x')
result_y = f("Result_total/result_y")

train_x = f('train_x')
train_y = f('train_y')

train = lambda x: np.array([filter(bool, i.split()) for i in x])
test = lambda x: np.array(x)

X_train = train(train_x)
Y_train = test(train_y)

X_test = train(result_x)
Y_test = test(result_y)

bdt = GradientBoostingClassifier(max_depth=3, n_estimators=2000)
bdt.fit(X_train, Y_train)

print bdt.score(X_test, Y_test)

# Y = bdt.predict(X_test)
# print float(sum(Y==Y_test))/(np.shape(Y_test)[0])

#im = Image.open("database/100.jpg")
#dr = ImageDraw.Draw(im)

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
