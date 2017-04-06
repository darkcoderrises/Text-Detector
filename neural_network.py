import numpy as np
import os
from PIL import Image
os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

"""
Training data, taken from images/CLASS1/ and images/CLASS2/
"""

path1 = "images/CLASS1/"
path2 = "images/CLASS2/"

data_x = []
data_y = []

def image_to_arr(path):
    im = Image.open(path)
    im = im.resize((24, 24), Image.ANTIALIAS)
    data = np.asarray(im, dtype=np.float64).flatten()
    mean = sum(data)/(24*24)
    return (data-mean)/np.var(data)

def val_append(path, val):
    data_x.append(image_to_arr(path))
    data_y.append(val)

map(lambda name: val_append(name, 1), [path1 + i for i in os.listdir(path1)])
map(lambda name: val_append(name, 0), [path2 + i for i in os.listdir(path2)])

def unison_shuffled_copies(a, b):
    p = np.random.permutation(len(a))
    return np.asarray(a)[p], np.asarray(b)[p]

data_x, data_y = unison_shuffled_copies(data_x, data_y)
length = int(0.7*len(data_y))
trX, trY = data_x[:length], data_y[:length]
teX, teY = data_x[length:], data_y[length:]

teX = np.reshape(teX, (len(teY), 24*24))
teY = np.reshape(teY, (len(teY), 1))

print "Input Over"

"""
Tensorflow code
"""
import tensorflow as tf

learning_rate = 0.0001
training_epoch = 100
batch_size = 10

n_hidden_1 = 72
n_hidden_2 = 8
n_input = 24*24
n_output = 1

x = tf.placeholder("float", [None, n_input])
y = tf.placeholder("float", [None, n_output])

def multilayer_perceptron(x, weights, biases):
    # Hidden layer with RELU activation
    layer_1 = tf.add(tf.matmul(x, weights['h1']), biases['b1'])
    layer_1 = tf.nn.relu(layer_1)

    # Hidden layer with RELU activation
    layer_2 = tf.add(tf.matmul(layer_1, weights['h2']), biases['b2'])
    layer_2 = tf.nn.relu(layer_2)

    # Output layer with linear activation
    out_layer = tf.matmul(layer_2, weights['out']) + biases['out']
    return out_layer

weights = {
    'h1': tf.Variable(tf.random_normal([n_input, n_hidden_1])),
    'h2': tf.Variable(tf.random_normal([n_hidden_1, n_hidden_2])),
    'out': tf.Variable(tf.random_normal([n_hidden_2, n_output]))
}

biases = {
    'b1': tf.Variable(tf.random_normal([n_hidden_1])),
    'b2': tf.Variable(tf.random_normal([n_hidden_2])),
    'out': tf.Variable(tf.random_normal([n_output]))
}

pred = multilayer_perceptron(x, weights, biases)

# Define loss and optimizer
cost = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(logits=pred, labels=y))
optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(cost)

# Initializing the variables
init = tf.global_variables_initializer()

with tf.Session() as sess:
    sess.run(init)

    for epoch in range(training_epoch):
        for start, end in zip(range(0, len(trX), batch_size), 
                range(batch_size, len(trX)+1, batch_size)):

            batch_x = trX[start:end]
            batch_y = trY[start:end]

            batch_x = np.reshape(batch_x, (len(batch_y), 24*24))
            batch_y = np.reshape(batch_y, (len(batch_y), 1))

            _, c = sess.run([optimizer, cost], feed_dict= {x: batch_x, y: batch_y})
        
        print "Epoch Value :", epoch

        print "Training Error :",
        prediction = tf.sigmoid(sess.run(pred, feed_dict= {x: teX}))
        predicted_class = tf.greater(prediction, 0.5)
        correct = tf.equal(predicted_class, teY)
        accuracy = tf.reduce_mean(tf.cast(correct, 'float'))
        print accuracy.eval()

    saver = tf.train.Saver()
    print saver.save(sess, "tensor-flow-nn.ckpt")
        
#print "Testing Error :",
#print np.mean(teY == sess.run(tf.argmax(pred, 1), feed_dict= {x: teX}))
