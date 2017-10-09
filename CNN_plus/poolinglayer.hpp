//
//  poolinglayer.hpp
//  
//
//  Created by Esma Yildirim on 11/8/16.
//
//
#ifndef CONVLAYER_HPP
#include "convlayer.hpp"
#endif

#ifndef POOLINGLAYER_HPP
#define POOLINGLAYER_HPP
enum Ops{
    MAX,
    AVG
};
class PoolLayer: public ConvLayer
{
public:
    PoolLayer(){};
    ~PoolLayer(){};
    void setOps(Ops o){op = o;};
    void setPoolSize(int p){ poolSize = p;};
    Ops getOps(){return op;};
    int getPoolSize(){return poolSize;};
    void pool(int myRank, int p, MPI_Comm *gcomm, int batchSize);
    void coordinateRead(int mStart, int mEnd, int bStart, int bEnd, int myMaps, int myMapIndex, int batchSize, MPI_Comm *gcomm);
    void computeWriteMaps(Tensor<double> &prev, int batchSize, MPI_Comm *gcomm, int numElems, int batchIndex, int mapIndex, int mapSize);
    void calculatePatchWidthHeight();
private:
    Ops op;
    int poolSize;
    
};
#endif
