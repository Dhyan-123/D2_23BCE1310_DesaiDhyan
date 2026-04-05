import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.applications import EfficientNetB0
from tensorflow.keras import layers, models
import numpy as np

# ========================
# CONFIG
# ========================
IMG_SIZE = (224, 224)
BATCH_SIZE = 32
EPOCHS = 15

train_dir = "dataset/train"
val_dir = "dataset/val"
test_dir = "dataset/test"

# ========================
# DATA GENERATORS
# ========================
train_datagen = ImageDataGenerator(
    rescale=1./255,
    rotation_range=25,
    zoom_range=0.3,
    horizontal_flip=True,
    shear_range=0.2
)

val_test_datagen = ImageDataGenerator(rescale=1./255)

train_data = train_datagen.flow_from_directory(
    train_dir,
    target_size=IMG_SIZE,
    batch_size=BATCH_SIZE,
    class_mode='categorical'
)

val_data = val_test_datagen.flow_from_directory(
    val_dir,
    target_size=IMG_SIZE,
    batch_size=BATCH_SIZE,
    class_mode='categorical'
)

test_data = val_test_datagen.flow_from_directory(
    test_dir,
    target_size=IMG_SIZE,
    batch_size=BATCH_SIZE,
    class_mode='categorical',
    shuffle=False
)

num_classes = train_data.num_classes
print("Classes:", train_data.class_indices)

# ========================
# MODEL (TRANSFER LEARNING)
# ========================
base_model = EfficientNetB0(
    input_shape=(224, 224, 3),
    include_top=False,
    weights='imagenet'
)

base_model.trainable = False  # freeze base

model = models.Sequential([
    base_model,
    layers.GlobalAveragePooling2D(),
    layers.BatchNormalization(),
    layers.Dense(256, activation='relu'),
    layers.Dropout(0.4),
    layers.Dense(num_classes, activation='softmax')  # multi-class
])

# ========================
# COMPILE
# ========================
model.compile(
    optimizer='adam',
    loss='categorical_crossentropy',
    metrics=['accuracy']
)

model.summary()

# ========================
# TRAIN
# ========================
history = model.fit(
    train_data,
    validation_data=val_data,
    epochs=EPOCHS
)

# ========================
# OPTIONAL: FINE-TUNING
# ========================
base_model.trainable = True

for layer in base_model.layers[:-20]:  # freeze most layers
    layer.trainable = False

model.compile(
    optimizer=tf.keras.optimizers.Adam(1e-5),
    loss='categorical_crossentropy',
    metrics=['accuracy']
)

history_fine = model.fit(
    train_data,
    validation_data=val_data,
    epochs=5
)

# ========================
# EVALUATE
# ========================
test_loss, test_acc = model.evaluate(test_data)
print(f"Test Accuracy: {test_acc:.4f}")

# ========================
# SAVE MODEL
# ========================
model.save("plant_disease_model.h5")
print("Model saved successfully.")