from ultralytics import YOLO

# Load a pretrained YOLOv8n model
model = YOLO("best.pt")

# Single stream with batch-size 1 inference
#source = '1.jpeg'  # images
#source = 'video.mp4'  # video
source = '0'  # video

# images
#results = model(source, save=True)  # generator of Results objects
# video
#results = model(source, show=True)  # generator of Results objects
#web cam
results = model(source, show=True)  # generator of Results objects