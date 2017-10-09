//
//  mnistlayer.h
//  
//
//  Created by Esma Yildirim on 11/14/16.
//
//

#ifndef INPUTLAYER_HPP
#include "inputlayer.hpp"
#endif

#ifndef MNISTLAYER_HPP
#define MNISTLAYER_HPP

#include <iostream>
#include <vector>
#include <inttypes.h>
#include <mpi.h>
#include "dataspaces.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
class MNISTLayer: public InputLayer
{

public:
    MNISTLayer(){};
    ~MNISTLayer(){};
    static uint64_t dataOffset;
    static uint64_t labelOffset;
    char * getLabelPath(){return labelPath;};
    void setLabelPath( char * p){labelPath = p;};
    char * getDataPath(){return dataPath;};
    void setDataPath( char * p){dataPath = p;};
    void uploadNextBatch(int myRank, int p, int batchSize, MPI_Comm *gcomm, vector<int> &labels);

private:
    char * dataPath;
    char * labelPath;
    
};


#endif /* defined(MNISTLAYER_HPP) */
