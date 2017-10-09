//
//  WSILayer.hpp
//  
//
//  Created by Esma Yildirim on 11/9/16.
//
//
#ifndef WSI_HPP
#include "wsi.hpp"
#endif

#ifndef INPUTLAYER_HPP
#include "inputlayer.hpp"
#endif

#ifndef OPENSLIDE_H
#define OPENSLIDE_H
#include "openslide.h"
#endif

#ifndef WSILAYER_HPP
#define WSILAYER_HPP
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mpi.h>
#include "dataspaces.h"
#include <opencv2/opencv.hpp>

class WSILayer: public InputLayer
{
    
public:
    WSILayer(){};
    WSILayer(char * wpath, char * lpath);
    ~WSILayer(){};
    static int index;
    void add(WSI obj){ coordList.push_back(obj); };
    vector<WSI > getCoordList(){ return coordList; };
    void initCoordList(int level);
    void uploadNextBatch(int myRank, int p, int batchSize, int level, MPI_Comm *gcomm);
    char * getWsiPath(){return wsiPath;};
    void setWsiPath(char * p){wsiPath = p;}
    char * getLabelPath(){return labelPath;};
    void setLabelPath( char * p){labelPath = p;}
    int getTumorLabel(WSI &item);
private:
    char * wsiPath;
    char * labelPath;
    vector<WSI > coordList;
    vector<int> labelList;
};

//

#endif
