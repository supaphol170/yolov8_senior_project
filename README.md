# yolov8_senior_project
Train model for detect obj receives from sensor is human or not for control turn on/off lights
Install & Set Up Environment : 
  - Download python for install from Official website : https://www.python.org/downloads/ 
  - Download anaconda miniconda3 from this link https://www.anaconda.com/download
  - Create virtual environment(venv) using miniconda3 by this command : ```bash conda create --name env_name python=3.7 (--name is depend on name what you want) ```
  - Activate venv from this command : ```bash codna activate --name ```
  - Install yolov8 but yolov8 is from Ultralytics using this command for install : ```bash pip install ultralytics ```
  - Create SubFolder like this :
    ```bash
    --name
      |__train
         |__images
         |__labels
      |__val
         |__images
         |__labels
    ```
    Folder train is use for train model and val is use for valid as you train
  - If you want custom your own model you have to downlload pictures as you want and save it in SubFolder images in Subfolder train or download by using code python by using simple_image_download you can install by follow step this official website : https://pypi.org/project/simple-image-download/
  ![Untitled](https://github.com/supaphol170/yolov8_senior_project/assets/124768326/f03cdbf8-6ed1-4d0b-88bf-511c2bba4348)
  - Install labelImg for crop image and classified and use to train model you own dataset by using this command : ```bash pip install labelImg ```
  - Open labelImg on miniconda3 terminal by this command : ```bash labelImg ```
  - Choose file icon and scroll down until you see open dir(for you image as you download and want to use train) after you choose you have too click File icon again and scroll down until you see save dir(you choose path val/images for save image after you crop and classified)
  - When you crop and classified finished you have to create file name.yaml(name is depend on you) for use in data when you train model
  - You have to see Folder you look like this
    ```bash
    --name
      |__train
         |__images
         |__labels
      |__val
         |__images
         |__labels
      name.yaml
    ```
  - Creating file for train train.py after create, you have to see Folder you look like this
    ```bash
    --name
      |__train
         |__images
         |__labels
      |__val
         |__images
         |__labels
      name.yaml
      train.py
    ```
