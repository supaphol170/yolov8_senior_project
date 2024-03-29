from flask import Flask, render_template, Response, jsonify
from flask_socketio import SocketIO
from flask_cors import CORS  # Import the CORS module
from ultralytics import YOLO
import numpy as np
import math
import cv2

app = Flask(__name__)

model = YOLO("C:/Users/Babe/Downloads/senior project/environment/test/runs/detect/train18/weights/best.pt")
color = "black"
count_person = "0"
class_name = ""
classNames = ["read", "sleep", "not read"]

def class_detected_to_color(alls):
    if int(alls) == 0:
        return "black"
    elif int(alls) >= 1:
        return "yellow"
#This function use for check class_name_human have a value or not.
def know_detect(alls):
    if (alls):
        return classNames[alls]
    else:
        return "No action"

def video_detection():
    URL = "http://192.168.78.109"
    AWB = True
    #video_capture = 0
    #Create a Webcam Object
    #cap=cv2.VideoCapture(video_capture)
    # Face recognition and opencv setup
    cap = cv2.VideoCapture(URL + ":81/stream") #use for receive input frame from esp32-cam via ip
    #video_capture = 0
    #cap = cv2.VideoCapture(video_capture)
    while True:
        _, image_input = cap.read()
        results = model(image_input, stream=True)
        global class_name_human
        class_name_human = ""
        for r in results:
            boxes = r.boxes
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                cv2.rectangle(image_input, (x1, y1), (x2, y2), (255, 0, 255), 3)
                conf = math.ceil((box.conf[0] * 100)) / 100
                clses = int(box.cls[0])
                class_name_human = clses
                label = f'{class_name}{conf}'
                textsize = cv2.getTextSize(label, 0, fontScale=1, thickness=2)[0]
                c2 = x1 + textsize[0], y1 - textsize[1] - 3
                cv2.rectangle(image_input, (x1, y1), c2, [255, 0, 255], -1, cv2.LINE_AA)
                cv2.putText(image_input, label, (x1, y1 - 2), 0, 1, [255, 255, 255], thickness=1, lineType=cv2.LINE_AA)
            yield image_input, boxes.shape[0]

@app.route('/api/get_color', methods=['GET'])
def api_get_color():
    global color
    global class_name_human
    return jsonify({'color': color, 'class_name_human': class_name_human})

@app.route('/')
def index():
    global color
    return render_template('index.html', color=color)

def gen():
    yolo_output = video_detection() 
    for detection_, count in yolo_output:#show how detect and show like numpy.ndarray
        _,buffer=cv2.imencode('.jpg',detection_)
        #for change color on page when detect
        global color
        #color = class_detected_to_color(count)
        #for count how many detected are there
        global class_name_human
        class_name_human = know_detect(int(count))
        #print(count_person)
        # Send the video frame to the client
        frame=buffer.tobytes()
        yield (b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame +b'\r\n')
        
@app.route('/video_feed', methods=['GET']) #return frame as predicted and send back to anyone want this to show on web.
def video_feed():
    return Response(gen(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='127.0.0.1', debug=True, port=5001)
