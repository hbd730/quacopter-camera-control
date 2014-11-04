# README #

### What is this repository for? ###

This repository is for camera detection application, developed under MacOS.
The latest OpenCV 3.0 alpha is used as the image processing library. Since it is a corssplatform library, it is very easy to setup in different OS. 

### How do I get set up? ###

All the setup information can be found in http://opencv.org/


In Mac:

* Download openCV from http://opencv.org/downloads.html

* Extract and go to the directory where OpenCV is.

```
#!bash

cd /PATH/TO/OPENCV
```

* Make a separate directory for building.


```
#!bash

mkdir build
cd build
cmake -G "Unix Makefiles" ..
```



* Build OpenCV


```
#!bash

make -j8
sudo make install
```


* OpenCV will be installed under /usr/local/lib and usr/local/include

* Pull the detection repository and run the xcode project.

**NOTE:** In Xcode we we need to provide the opencv library you are going to use and header path, or we can build openCV framework instead of the library, and just include that framework in the project.
different OS might have different setup regarding library reference. The framework build approach can be found in https://github.com/kyogoku42/opencv2-framework-for-mac