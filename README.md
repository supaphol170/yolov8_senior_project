# yolov8_senior_project
Train model for detect obj receives from sensor is human or not for control turn on/off lights
Install & Set Up Environment : 
  - Download python for install from Official website : https://www.python.org/downloads/ 
  - Download anaconda miniconda3 from this link https://www.anaconda.com/download
  - Create virtual environment(venv) using miniconda3 by this command : ``` conda create -n --name env_name python=3.7 (--name is depend on name what you want) ```
  - Activate venv from this command : ``` conda activate --name ```
  - Install yolov8 but yolov8 is from Ultralytics so you have to using this command for install : ``` pip install ultralytics ```
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
  - If you want custom your own model you have to download pictures as you want and save it in SubFolder images in Subfolder train or download by using code python by using simple_image_download you can install by follow step this official website : https://pypi.org/project/simple-image-download/
  ![Untitled](https://github.com/supaphol170/yolov8_senior_project/assets/124768326/f03cdbf8-6ed1-4d0b-88bf-511c2bba4348)
  - If you use libraries ``` simple_image_download ``` for download pictures as you want from your own keywords when downloads finished, It'll show look like this on your terminal
    ![terminal](https://github.com/supaphol170/yolov8_senior_project/assets/124768326/56b8f568-4432-4513-9f20-311f65b96af0)
  - Install labelImg for crop image and classified and use to train model you own dataset by using this command : ``` pip install labelImg ```
  - Open labelImg on miniconda3 terminal by this command : ``` labelImg ```
  - Choose file icon and scroll down until you see open dir(for you image as you download and want to use train) after you choose you have to click File icon again and scroll down until you see save dir(you choose path val/images for save image after you crop and classified)
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
