from ultralytics import YOLO

# Load a model
model = YOLO('yolov8.yaml')  # build a new model from YAML
model = YOLO('yolov8x.pt')  # load a pretrained model (recommended for training)
model = YOLO('yolov8.yaml').load('yolov8x.pt')  # build from YAML and transfer weights

# Train the model
results = model.train(data='person.yaml', epochs=100, imgsz=640)
