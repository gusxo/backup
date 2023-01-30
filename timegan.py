import tensorflow as tf
import tensorflow.keras as keras

def get_model(input_shape, output_units, rnn_units, layer_cnt):
    inputs = keras.layers.Input(input_shape)
    x = inputs
    for i in range(layer_cnt):
        x = keras.layers.GRU(rnn_units, return_sequences=True)(x)
    outputs = keras.layers.Dense(output_units, activation="sigmoid")(x)
    return keras.Model(inputs, outputs)

def init_timegan(input_shape, units=24, layers=3):
    pass


