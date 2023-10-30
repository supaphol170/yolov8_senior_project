from flask import Flask, render_template, request, Response, send_from_directory
from ultralytics import YOLO
import cv2
import numpy as np
import os
from werkzeug.utils import secure_filename

app=Flask(__name__)
app.config['UPLOAD_FOLDER'] = os.path.join('static', 'runs')
model = YOLO("best.pt") # Model as train own dataset for predict
ALLOWED_EXTENSIONS = {'mp4', 'pdf', 'png', 'jpg', 'jpeg', 'gif'} #for protect that user insert input on website 

@app.route('/detect', methods=["POST"])
def detect():
    if request.method == "POST":
        uploaded_file = request.files["file"]
        if uploaded_file:
            # Read the uploaded image
            image = cv2.imdecode(np.frombuffer(uploaded_file.read(), np.uint8), cv2.IMREAD_COLOR)
            # Perform object detection
            results = model.predict(image, save_txt=True, save=True, project="C:\\Users\\Babe\\Downloads\\senior project\\environment\\test\\static\\runs")
            #access attributes line 18
            for image_path in results:
                path_images = image_path.save_dir.split("\\")[9] # For split path by \\ to list and go to index 9 for use word "predict1-.."
                in_pathimages = image_path.path #for query name image as save in this case is name "image0.jpg"
                keep_path = app.config['UPLOAD_FOLDER']+"\\"+path_images #for combine variable path_images with \ and app.config['UPLOAD_FOLDER'] to full path image save
                final_find = os.path.join(keep_path, in_pathimages) #for open keep_path to open image on web
                return render_template('show.html', images=final_find) # show find html name show with image
        #this line use for render_template('error.html') when can't process line 14-22 

@app.route('/')
def index():
    return render_template('index.html')

def video():
    return Response(detect(),mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__=="__main__":
    app.run(port=5500, debug=True)
