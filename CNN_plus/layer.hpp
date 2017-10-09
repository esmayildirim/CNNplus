#ifndef OPENCV
#define OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/plot.hpp>
#endif


#ifndef LAYER_HPP
#define LAYER_HPP
/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */

#include <string>
#include <stdexcept>

// Error codes

enum Types {
    LINPUT,
    LCONV,
    LPOOL,
    LFC,
    LSOFTMAX,
    LUNDEFINED
};
using namespace cv;
class Layer
{
public:
    Layer(){};
    ~Layer(){};
    char *getID() { return id; };
    void setID(char * I){ id = I; };
    Types getType() { return type; };
    void setType(Types t){type = t;};
    Layer * getNextLayer(){return nextLayer;};
    Layer * getPrevLayer(){ return prevLayer;};
    void setNextLayer(Layer * l){ nextLayer = l;};
    void setPrevLayer(Layer * l){ prevLayer = l;};
    void setPredID(char * pid){ predID = pid;};
    char * getPredID(){return predID;};
    void setWeightID(char * wid){weightID = wid;};
    char * getWeightID(){return weightID;};
    void setBiasID(char * bid){biasID = bid;};
    char * getBiasID(){return biasID;};
    void setReLU(bool b) {reLU = true;};
    bool getReLU(){return reLU;};
private:
    
    Layer * prevLayer;
    Layer * nextLayer;
    char* id;
    char* predID;
    char* weightID;
    char* biasID;
    Types type;
    bool reLU;
    
    // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.
    
};
#endif

