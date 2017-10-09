//
//  convlayer.cpp
//
//
//  Created by Esma Yildirim on 11/16/16.
//
//

#include <stdio.h>


#ifndef CONVLAYER_HPP
#include "convlayer.hpp"
#endif

void ConvLayer::calculatePatchWidthHeight()
{
    int P = 0; //padding size
    if(getPadding())
        P = floor(getFilterSize()/2);
    InputLayer * prev = static_cast<InputLayer *>(getPrevLayer());
    setPatchWidth((prev->getPatchWidth() - getFilterSize() + 2 * P)/ getStride() + 1);
    setPatchHeight((prev->getPatchHeight() - getFilterSize() + 2 * P)/ getStride() + 1);
}

Tensor<double> * ConvLayer::computeMap(Tensor<double> &prev, Tensor<double> &W, Tensor<double> &b, int batchSize, int mIndex)
{
    Tensor<double> * mapT = new Tensor<double>(getID(), 2);
    int *dims = new int[2];
    dims[0] = getMaps() * getPatchHeight() * getPatchWidth();
    dims[1] = batchSize;
    mapT->setDimSizes(dims);
    mapT->createData(getPatchWidth() * getPatchHeight() * 1);//create data space for one map
    int prevHeight, prevWidth, prevMaps;
    if(getPrevLayer()->getType()==LINPUT || getPrevLayer()->getType()==LPOOL)
    {
        InputLayer *temp = static_cast<InputLayer *>(getPrevLayer());
        prevHeight = temp->getPatchHeight();
        prevWidth = temp->getPatchWidth();
        prevMaps = temp-> getMaps();
    }
    int pixelIndex = 0;
    for(int i = 0; i < prevHeight; i+= getStride())
    {
        
        for(int j = 0; j < prevWidth; j+= getStride())
        {
            double sum = 0.0;
            if((i >= getFilterSize()/2)  && (j>= getFilterSize()/2) && (i < prevHeight - getFilterSize()/2) && (j < prevWidth - getFilterSize()/2))
            {
                //no padding necessary
                for(int k = 0; k< prevMaps; k++)
                {
                    for(int ii = 0; ii < getFilterSize(); ii++)
                        for(int jj =0; jj < getFilterSize(); jj++)
                        {
                            sum +=prev.getData()[(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj ] * W.getData()[mIndex * prevMaps * getFilterSize()*getFilterSize() +  k * getFilterSize()*getFilterSize() + ii * getFilterSize() + jj];
                            //printf("SUM:%.10f\n", sum);
                            //printf("D%d:%.10f\n",(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj, prev.getData()[(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj ]);
                         //   printf("%.10f\n", W.getData()[mIndex * prevMaps * getFilterSize() * getFilterSize() +  k * getFilterSize()*getFilterSize() + ii * getFilterSize() + jj]);
                        }
                }
                // printf("SUM:%.10f\n", sum);
                
                mapT->getData()[pixelIndex++] = sum ;//+ b.getData()[mIndex];
                //printf("%s:%d:%.10f\n", b.getID(),mIndex,b.getData()[mIndex]  );
                //apply RELU
                if(getReLU())
                    if(mapT->getData()[pixelIndex-1] < 0)
                        mapT->getData()[pixelIndex -1] = 0;
                //printf("C%d:%.10f\n",pixelIndex-1, mapT->getData()[pixelIndex-1]);
            }
            else{
                //padding necessary
                if(padding)
                {
                    
                    for(int k = 0; k< prevMaps; k++)
                    {
                        for(int ii = 0; ii < getFilterSize(); ii++)
                            for(int jj =0; jj < getFilterSize(); jj++)
                            {
                                if(!(((i-getFilterSize()/2) + ii < 0 || (i-getFilterSize()/2) + ii >= prevHeight) || ((j-getFilterSize()/2) + jj < 0 || (j-getFilterSize()/2) + jj >= prevWidth)))
                                {
                                    
                                    sum +=prev.getData()[(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj ] * W.getData()[mIndex * prevMaps * getFilterSize() * getFilterSize() +  k * getFilterSize()*getFilterSize() + ii * getFilterSize() + jj];
                                    //printf("SUM:%.10f\n", sum);
                                   // printf("D%d:%.10f\n",(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj, prev.getData()[(k * prevHeight * prevWidth) + ((i - getFilterSize()/2) + ii) * prevWidth + (j-getFilterSize()/2) +jj ]);
                                  //  printf("%.10f\n", W.getData()[mIndex * prevMaps * getFilterSize() * getFilterSize() +  k * getFilterSize()*getFilterSize() + ii * getFilterSize() + jj]);
                                }
                            }
                    }
                     //printf("SUM:%.10f\n", sum);
                    mapT->getData()[pixelIndex++] = sum ;//+ b.getData()[mIndex];
                    //printf("%s:%d:%.10f\n", b.getID(),mIndex, b.getData()[mIndex] );
                    //apply RELU
                    if(getReLU())
                        if(mapT->getData()[pixelIndex-1] < 0)
                            mapT->getData()[pixelIndex -1] = 0;
                    //printf("C%d:%.10f\n", pixelIndex -1, mapT->getData()[pixelIndex-1]);
                    
                }
                
            }
            
            
            
        }//end of J loop
    }//end of I loop
    
    return mapT;
    
}
void ConvLayer::coordinateRead(Tensor<double> &W , Tensor<double> &b, int mStart, int mEnd, int bStart, int bEnd, int myMaps, int myMapIndex, int batchSize, MPI_Comm *gcomm)
{
    Tensor<double> prevT(getPrevLayer()->getID(), 2);
    int * dims = new int[2];
    dims[0] = getFilterSize() * getFilterSize();
    
    if(getPrevLayer()->getType()== LINPUT || getPrevLayer()->getType()== LPOOL)
    {   InputLayer * prev = static_cast<InputLayer *>(getPrevLayer());
        dims[0] = prev-> getPatchWidth() * prev-> getPatchHeight() * prev->getMaps();
        dims[1] = batchSize;
        
    }
    prevT.setDimSizes(dims);
    prevT.createData(dims[0] * 1);
    
    if((mEnd - mStart + 1)> 2)
    {
        for (int i =0 ; i < batchSize; i++) {
            prevT.readPartial2DData(gcomm, 1, i);
            for (int j = mStart; j <= mEnd; j++) {
                if(Algorithms::indicesWithinRange(myMapIndex,myMaps,j,i,batchSize))
                {
                    Tensor<double> * l = computeMap(prevT, W, b, batchSize,j-mStart);
                    l->writePartial2DData(gcomm,1,i,j,getPatchHeight()*getPatchWidth());
                    delete l;
                }
            }
            //free map
        }
    }
    else if((mEnd - mStart + 1) == 2)
    {
        if(bEnd < bStart)
        {
            for (int i = bStart; i < batchSize; i++)
            {
                prevT.readPartial2DData(gcomm, 1, i);
                for (int j = mStart; j <= mEnd; j++) {
                    if(Algorithms::indicesWithinRange(myMapIndex,myMaps,j,i,batchSize))
                    {
                        Tensor<double> * l = computeMap(prevT, W, b, batchSize,j-mStart);
                        l->writePartial2DData(gcomm,1,i,j,getPatchHeight()*getPatchWidth());
                        delete l;
                    }
                }
                //free the map buffer
            }
            for (int i = 0; i <= bEnd; i++)
            {
                prevT.readPartial2DData(gcomm, 1, i);
                for (int j = mStart; j <= mEnd; j++) {
                    if(Algorithms::indicesWithinRange(myMapIndex,myMaps,j,i,batchSize))
                    {
                        Tensor<double> * l = computeMap(prevT, W, b, batchSize,j-mStart);
                        l->writePartial2DData(gcomm,1,i,j,getPatchHeight()*getPatchWidth());
                        delete l;
                    }
                }
            }
        }
        else{
            for (int i =0 ; i < batchSize; i++) {
                prevT.readPartial2DData(gcomm, 1, i);
                for (int j = mStart; j <= mEnd; j++) {
                    if(Algorithms::indicesWithinRange(myMapIndex,myMaps,j,i,batchSize))
                    {
                        Tensor<double> * l = computeMap(prevT, W, b, batchSize,j-mStart);
                        l->writePartial2DData(gcomm,1,i,j,getPatchHeight()*getPatchWidth());
                        delete l;
                    }
                    
                }
                
                
            }
            
        }
        
        //free the weights and biases tensors
        
    }
    else if((mEnd - mStart + 1) == 1)
    {
        for (int i =bStart ; i <= bEnd; i++) {
            prevT.readPartial2DData(gcomm, 1, i);
            for (int j = mStart; j <= mEnd; j++) {
                Tensor<double> * l = computeMap(prevT, W, b, batchSize,j-mStart);
                l->writePartial2DData(gcomm,1,i,j,getPatchHeight()*getPatchWidth());
                delete l;
                
            }
        }
        
    }
    else{}
    
    
    
    
}

void ConvLayer::convolution(int myRank, int p, MPI_Comm *gcomm, int batchSize)
{
    //distribute the load of the layer
    int mStart, mEnd, bStart, bEnd, myMaps, myMapIndex;
    Algorithms::distributeLoad(batchSize, getMaps(), myRank, p, bStart, bEnd, mStart, mEnd,myMaps,myMapIndex);
    printf("%d: %d %d %d %d %d %d\n", myRank, bStart, bEnd, mStart, mEnd,myMaps,myMapIndex);
    //read weights
    Tensor<double> weights(getWeightID(),3);
    int * dims = new int[3];
    dims[0] = pow(getFilterSize(), 2);
    if(getPrevLayer()->getType()== LINPUT || getPrevLayer()->getType()== LPOOL)
    {   InputLayer * prev = static_cast<InputLayer *>(getPrevLayer());
        dims[1] = prev->getMaps();
    }
    
    dims[2] = getMaps();
    printf("CONV WEIGHTS: %d %d %d \n", dims[0], dims[1], dims[2]);
    weights.setDimSizes(dims);
    weights.createData(dims[0] * dims[1] * (mEnd - mStart+1));
    weights.readPartial3DData(gcomm, mEnd - mStart+1, mStart);
    for (int i = 0; i < dims[0] * dims[1] * (mEnd - mStart+1); i++) {
        double * datas = weights.getData();
        printf("%f\t", datas[i]);
    }
    
    //read biases
    Tensor<double> biases(getBiasID(),2);
    int * dims2 = new int[2];
    dims2[0] = getMaps();
    dims2[1] = 1;
    biases.setDimSizes(dims2);
    biases.createData(mEnd - mStart+1);
    biases.readPartial1DData(gcomm, mEnd - mStart+1, mStart);
    //for (int i = 0; i < mEnd - mStart+1 ;i++ ) {
    //    printf("%s:%.10f\n", biases.getID(), biases.getData()[i]);
    //}
    
    
    coordinateRead(weights , biases, mStart, mEnd, bStart, bEnd, myMaps, myMapIndex, batchSize, gcomm);
    
}




