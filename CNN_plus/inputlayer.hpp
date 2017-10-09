#ifndef LAYER_HPP
#include "layer.hpp"
#endif

#ifndef OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/plot.hpp>
#endif
#ifndef INPUTLAYER_HPP
#define INPUTLAYER_HPP
/**
 *  @file
 *  Class "InputLayer"  .
 *  @version 1.0
 */

#include <string>
#include <stdexcept>



class InputLayer: public Layer
{
public:
    InputLayer(){};
    ~InputLayer(){};
    void setMaps(int m){ maps = m;};
    int getMaps(){ return maps;};
    void setPatchWidth(int w){ patchWidth = w;};
    void setPatchHeight(int h){ patchHeight = h;};
    int getPatchWidth(){ return patchWidth;};
    int getPatchHeight(){ return patchHeight;};
    double * zeroCenterNormalize(Mat &image);
private:
    int maps;
    int patchWidth;
    int patchHeight;
    
};
#endif
