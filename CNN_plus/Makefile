

OPENCV_INC=-I/usr/local/Cellar/opencv-3.1/include
OPENCV_LIB=-L/usr/local/Cellar/opencv-3.1/lib -lopencv_calib3d  -lopencv_core -lopencv_features2d -lopencv_flann  -lopencv_highgui -lopencv_imgproc  -lopencv_ml  -lopencv_objdetect  -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab -lopencv_plot

OPENSLIDE_LIB=-L/usr/local/lib -lopenslide
OPENSLIDE_INC=-I/usr/local/include/openslide
GLIB_INC=-I/usr/local/Cellar/glib/2.44.1/include/glib-2.0 -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include -I/usr/local/opt/gettext/include

DATASPACES_LIB=-L/Users/eyildirim/Documents/ds_install/lib -ldspaces -ldscommon -ldart -lm
DATASPACES_INC=-I/Users/eyildirim/Documents/ds_install/include

XERCES_INC=-I/Users/eyildirim/Documents/xerces/include
XERCES_LIB=-L/Users/eyildirim/Documents/xerces/lib

CC=mpicxx

all: executable 

executable:main.o network.o parser.o inputlayer.o mnistlayer.o algorithms.o convlayer.o poolinglayer.o
	$(CC) -o $@ $^ $(XERCES_LIB)  $(OPENCV_LIB) $(DATASPACES_LIB) -lxerces-c-3.1

parser.o:parser.cpp
	$(CC) -c parser.cpp $(XERCES_INC) $(OPENCV_INC) $(DATASPACES_INC)

network.o:network.cpp
	$(CC) -c network.cpp  $(DATASPACES_INC) $(OPENCV_INC)

convlayer.o:convlayer.cpp
	$(CC) -c convlayer.cpp  $(DATASPACES_INC) $(OPENCV_INC)

poolinglayer.o:poolinglayer.cpp
	$(CC) -c poolinglayer.cpp  $(DATASPACES_INC) $(OPENCV_INC)

inputlayer.o:inputlayer.cpp
	$(CC) -c inputlayer.cpp  $(DATASPACES_INC) $(OPENCV_INC)

mnistlayer.o:mnistlayer.cpp
	$(CC) -c mnistlayer.cpp $(DATASPACES_INC) $(OPENCV_INC)

algorithms.o:algorithms.cpp
	$(CC) -c algorithms.cpp


main.o:main.cpp
	$(CC) -c main.cpp $(XERCES_INC)  $(OPENCV_INC) $(DATASPACES_INC)



.PHONY:clean

clean:
	rm -f *.o


