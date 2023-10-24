from ultralytics import YOLO
import cv2 #Using for show output on fram as create
import imghdr #Using for indentify type of input it's images or not
import magic #Using for indentify type of input it's video or not
import cgi

#This file use to detect input
def execute(types, file):
    #type of input
    #Single stream with batch-size 1 inference
    source = ''
    if types == 'image':
        source = file  # images
    elif types == 'video':
        source = file  # video
    else:
        source = '0'  # video

    # Load a pretrained YOLOv8n model
    model = YOLO("best.pt")
    cap = cv2.VideoCapture(source)
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

#check type of input
def check_type_input(file):
     # for detect image
    file_type_image = imghdr.what(file)
    # lines 8-9 for detect video
    mime = magic.Magic()
    file_type_video = mime.from_file(file)
    #check image or video or others
    if file_type_image:
        return 'image'
    elif 'video' in file_type_video:
        return 'video'
    else:
        return 'stream'   

#main function
def main(file):
    form = cgi.FieldStorage()
    if "file" in form:
        fileitem = form["file"]
        if fileitem.file:
            types = check_type_input(fileitem)
            execute(types, file)
            print("File uploaded and processed successfully!")
        else:
            print("No file Uploaded.")
    else:
        print("No file Uploaded.")
