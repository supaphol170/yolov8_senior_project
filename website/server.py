from flask import Flask, render_template, Response, jsonify
import cv2
from ultralytics import YOLO
import numpy as np
import math
from flask_socketio import send, emit


app = Flask(__name__)
model = YOLO("best.pt")
# Create a variable to store the current color
color = "black"
#Create a variable to store how many people as detected
count_person = "0"

def class_detected_to_color(alls):
    if int(alls) == 0:
        return "black"
    elif int(alls) >=1 :
        return "red"
        
def video_detection():
    #URL = "http://192.168.137.17/:81/stream"
    #AWB = True
    #Create a Webcam Object
    #cap=cv2.VideoCapture(video_capture)
    # Face recognition and opencv setup
    #cap = cv2.VideoCapture(URL)
    URL = "http://192.168.137.17"
    AWB = True
    video_capture = 0
    #Create a Webcam Object
    #cap=cv2.VideoCapture(video_capture)
    # Face recognition and opencv setup
    cap = cv2.VideoCapture(URL + ":81/stream")
    classNames = ["person"]
    while True:
        _, image_input = cap.read()
        results=model(image_input,stream=True)
        for r in results:
            boxes=r.boxes
            for box in boxes:
                x1,y1,x2,y2=box.xyxy[0] #use to see value in xyxy when u detect
                x1,y1,x2,y2=int(x1), int(y1), int(x2), int(y2)
                cv2.rectangle(image_input, (x1,y1), (x2,y2), (255,0,255),3) #create box when detect somehing 
                conf=math.ceil((box.conf[0]*100))/100 # for know confidence as predict from model and change from several decimal places to two.
                clses=int(box.cls[0])
                class_name=classNames[clses]
                label=f'{class_name}{conf}' #concatinate between class as Detected and Confidence
                textsize = cv2.getTextSize(label, 0, fontScale=1, thickness=2)[0]#create word from classNames when detected 
                c2 = x1 + textsize[0], y1 - textsize[1] - 3
                cv2.rectangle(image_input, (x1,y1), c2, [255,0,255], -1, cv2.LINE_AA)  # filled
                cv2.putText(image_input, label, (x1,y1-2),0, 1,[255,255,255], thickness=1,lineType=cv2.LINE_AA)
            yield image_input, boxes.shape[0] # Yield like function return in python annd image_input is used for stream frames and boxes is use for kno what kind of detect
cv2.destroyAllWindows()

@app.route('/get_color')
def get_color(): # Using for change color when detect is human in int.html
    global color #setting value global for across different routes
    global count_person
    return jsonify({'color': color, 'count_person': count_person}) #Converting python file to .json file to htmml can read

@app.route('/')
def index():
    global color
    return render_template('index.html', color=color)

#like main or call other function 
def gen():
    yolo_output = video_detection()
    for detection_, count in yolo_output:#show how detect and show like numpy.ndarray
        _,buffer=cv2.imencode('.jpg',detection_)
        #for change color on page when detect
        global color
        color = class_detected_to_color(count)
        #for count how many detected are there
        global count_person
        count_person = "Person : " + str(count)
        print(count_person)
        # Send the video frame to the client
        frame=buffer.tobytes()
        yield (b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame +b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='127.0.0.1', debug=True,port="5000")
