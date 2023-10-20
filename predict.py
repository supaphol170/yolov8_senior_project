from ultralytics import YOLO
import cv2

# Load a pretrained YOLOv8n model
model = YOLO("best.pt")

# Single stream with batch-size 1 inference
#source = '1.jpeg'  # images
#source = 'video.mp4'  # video
#source = '0'  # video
cap = cv2.VideoCapture(0)

# images
#results = model(source, save=True)  # generator of Results objects
# video
#results = model(source, show=True)  # generator of Results objects
#web cam
#results = model.predict(source, show=True)  # generator of Results objects
while cap.isOpened():
    success, frames = cap.read()
    if success:
        results = model.predict(frames, save_txt=True) #use for run this model on this frames
        virtualize = results[0].plot() # virtualize output from results as plot the detection on this frames
        # View results
        person = False
        for r in results:
            if (str(r.boxes.xywh)) == "tensor([], size=(0, 4))":
                person == True
                with open('output.txt', 'w') as flies:
                    print(person, file=flies) # lines26-30 for js website to change color
        cv2.imshow("Detect Person", virtualize) # Display virtualize on the frames
        #break this loop for close frame
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
cap.release()
cv2.destroyAllWindows()
        
