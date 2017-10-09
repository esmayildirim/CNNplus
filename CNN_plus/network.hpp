
#ifndef CONVLAYER_HPP
#include "convlayer.hpp"
#endif

#ifndef FCLAYER_HPP
#include "fclayer.hpp"
#endif

#ifndef POOLINGLAYER_HPP
#include "poolinglayer.hpp"
#endif

#ifndef SOFTMAXLAYER_HPP
#include "softmaxlayer.hpp"
#endif


#ifndef NETWORK_HPP
#define NETWORK_HPP
/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */


#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

class Network
{
public:
    Network();
    ~Network(){};
    
    void addLayer(Layer *l){ layerVector.push_back(l); count++; } ;
    void setLRate(double r){ lRate = r;};
    void setMomentum(double m){ momentum = m;};
    void setRegStrength(double s){ regStrength = s;};
    int getCount(){ return count; };
    double getLRate(){ return lRate; };
    double getMomentum(){ return momentum; };
    double getRegStrength() { return regStrength; };
    vector<Layer *> getVector(){ return layerVector;};
    void printLayerList();
private:
    
    vector<Layer *> layerVector;
    int count;
    double lRate;
    double momentum;
    double regStrength;
};
#endif

