import sys
import numpy as np
import os
os.environ['TF_CPP_MIN_LOG_LEVEL']='2'
import tensorflow as tf

arg = np.asarray(map(float, sys.argv[1:]))
arg = np.reshape(arg, (1, 24*24))

n_hidden_1 = 72
n_hidden_2 = 8
n_input = 24*24
n_output = 1

x = tf.placeholder("float", [None, n_input])
y = tf.placeholder("float", [None, n_output])

def multilayer_perceptron(x, weights, biases):
    layer_1 = tf.add(tf.matmul(x, weights['h1']), biases['b1'])
    layer_1 = tf.nn.relu(layer_1)

    layer_2 = tf.add(tf.matmul(layer_1, weights['h2']), biases['b2'])
    layer_2 = tf.nn.relu(layer_2)

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

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    new_saver = tf.train.import_meta_graph('tensor-flow-nn.ckpt.meta')
    new_saver.restore(sess, tf.train.latest_checkpoint('./'))

    prediction = tf.sigmoid(sess.run(pred, feed_dict= {x: arg}))
    predicted_class = tf.greater(prediction, 0.5)
    res = predicted_class.eval()[0][0]
    print 1 if res else 0
