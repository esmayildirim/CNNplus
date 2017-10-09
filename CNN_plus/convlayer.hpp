//
//  ConvLayer.hpp
//  
//
//  Created by Esma Yildirim on 11/8/16.
//
//
#ifndef INPUTLAYER_HPP
#include "inputlayer.hpp"
#endif

#ifndef TENSOR_HPP
#include "tensor.hpp"
#endif

#ifndef ALGORITHMS_HPP
#include "Algorithms.hpp"
#endif

#ifndef CONVLAYER_HPP
#define CONVLAYER_HPP

class ConvLayer: public InputLayer
{
public:
    ConvLayer(){};
    ~ConvLayer(){};
    void setPadding(bool p){padding = p;};
    void setStride(int s){ stride = s;};
    void setFilterSize (int f){ filterSize = f;};
    bool getPadding(){ return padding;};
    int getStride(){ return stride;};
    int getFilterSize(){ return filterSize; };
    void convolution(int myRank, int p, MPI_Comm *gcomm, int batchSize);
    void calculatePatchWidthHeight();
    void coordinateRead(Tensor<double> &W , Tensor<double> &b, int mStart, int mEnd, int bStart, int bEnd, int myMaps, int myMapIndex, int batchSize, MPI_Comm *gcomm);
    Tensor<double> * computeMap(Tensor<double> &prev, Tensor<double> &W, Tensor<double> &b, int batchSize, int mIndex);
private:
    bool padding;
    int stride;
    int filterSize;
};
#endif



