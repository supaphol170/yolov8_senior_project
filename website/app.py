from flask import Flask, render_template, request, send_file, Response
from ultralytics import YOLO
import cv2
import numpy as np
import os

app=Flask(__name__)
model = YOLO("best.pt")

@app.route('/detect', methods=["POST"])
def detect():
    if request.method == "POST":
        uploaded_file = request.files["file"]
        if uploaded_file:
            # Read the uploaded image
            image = cv2.imdecode(np.frombuffer(uploaded_file.read(), np.uint8), cv2.IMREAD_COLOR)
            # Perform object detection
            results = model.predict(image, save_txt=True, save=True)
            #access attributes line 18
            for image_path in results:
                path_images = image_path.save_dir
                in_pathimages = image_path.path
                original_path = "C:\\Users\\Babe\\Downloads\\senior project\\environment\\test\\"+path_images
                final_find = os.path.join(original_path, in_pathimages)
                print(final_find)
                return render_template('show.html', images=final_find)
        #this line use for render_template('error.html') when can't process line 14-22 

@app.route('/')
def index():
    return render_template('index.html')

def video():
    return Response(detect(),mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__=="__main__":
    app.run(debug=True)
