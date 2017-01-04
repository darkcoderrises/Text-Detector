import numpy as np
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt

f = lambda f: filter(bool, open(f).read().split('\n'))
train = lambda x: np.array([map(float, filter(bool, i.split())) for i in x])
test = lambda x: np.array(map(float, x))

train_x = f('train_x')
train_y = f('train_y')

X_train = train(train_x)
train_y = test(train_y)

pca = PCA(n_components=3)
pca.fit(X_train)
X_pca = pca.transform(X_train)

target_names = ["Text", "NonText"]
plt.figure()
for color, i, target_name in zip(['red', 'green'], [-1,1], target_names):
    plt.scatter(X_pca[train_y==i, 0], X_pca[train_y==i, 1], color=color, alpha=0.8, lw=2, label=target_name)
plt.show()

