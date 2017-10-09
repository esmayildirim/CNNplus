//
//  pooling.cpp
//  
//
//  Created by Esma Yildirim on 11/21/16.
//
//

#include <stdio.h>
#ifndef POOLINGLAYER_HPP
#include "poolinglayer.hpp"
#endif
void PoolLayer::calculatePatchWidthHeight()
{
    InputLayer * prev = static_cast<InputLayer *>(getPrevLayer());
    setPatchWidth((prev->getPatchWidth() - getPoolSize())/ getStride() + 1);
    setPatchHeight((prev->getPatchHeight() - getPoolSize())/ getStride() + 1);
}

void PoolLayer::pool(int myRank, int p, MPI_Comm *gcomm, int batchSize)
{
    //distribute the load of the layer
    int mStart, mEnd, bStart, bEnd, myMaps, myMapIndex;
    Algorithms::distributeLoad(batchSize, getMaps(), myRank, p, bStart, bEnd, mStart, mEnd,myMaps,myMapIndex);
    printf("%d: maps: %d batchsize: %d %d %d %d %d %d %d\n", myRank,getMaps(),batchSize, bStart, bEnd, mStart, mEnd,myMaps,myMapIndex);
    coordinateRead(mStart, mEnd, bStart, bEnd, myMaps, myMapIndex, batchSize, gcomm);
}

void PoolLayer::coordinateRead(int mStart, int mEnd, int bStart, int bEnd, int myMaps, int myMapIndex, int batchSize, MPI_Comm *gcomm)
{
    Tensor<double> prevT(getPrevLayer()->getID(), 2);
    //printf("ID: %s\n",getPrevLayer()->getID() );
    int * dims = new int[2];
    dims[0] = getFilterSize() * getFilterSize();
    
    if(getPrevLayer()->getType()== LINPUT || getPrevLayer()->getType()== LCONV || getPrevLayer()->getType()== LPOOL)
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
                    computeWriteMaps(prevT,batchSize,gcomm, 1,i,j, getPatchHeight()*getPatchWidth());
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
                        computeWriteMaps(prevT,batchSize,gcomm, 1,i,j, getPatchHeight()*getPatchWidth());
                        
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
                        computeWriteMaps(prevT,batchSize,gcomm, 1,i,j, getPatchHeight()*getPatchWidth());
                        
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
                        computeWriteMaps(prevT,batchSize,gcomm, 1,i,j,getPatchHeight()*getPatchWidth());

                        
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
                
                 computeWriteMaps(prevT,batchSize,gcomm, 1,i,j,getPatchHeight()*getPatchWidth());
                
            }
        }
        
    }
    else{}
    
    
    
    
}


void PoolLayer::computeWriteMaps(Tensor<double> &prev, int batchSize, MPI_Comm *gcomm, int numElems, int batchIndex, int mapIndex, int mapSize)
{
    //printf("ID:%s\n",getID());
    Tensor<double> * mapT = new Tensor<double>(getID(), 2);
    
    char * indexName = (char *)(string(getID()).append("_index").c_str());
    Tensor<int> * poolIndex = new Tensor<int>(indexName, 2);
    
    int *dims = new int[2];
    int *dimsIndex = new int[2];
    dims[0] = dimsIndex[0] = getMaps() * getPatchHeight() * getPatchWidth();
    dims[1] = dimsIndex[1] = batchSize;
    mapT->setDimSizes(dims);
    poolIndex->setDimSizes(dimsIndex);
    mapT->createData(getPatchWidth() * getPatchHeight() * 1);//create data space for one map
    poolIndex->createData(getPatchWidth() * getPatchHeight() * 1);
    int prevHeight, prevWidth;
    if(getPrevLayer()->getType()==LINPUT || getPrevLayer()->getType()==LCONV || getPrevLayer()->getType()==LPOOL)
    {
        InputLayer *temp = static_cast<InputLayer *>(getPrevLayer());
        prevHeight = temp->getPatchHeight();
        prevWidth = temp->getPatchWidth();
        
        //printf("PREV values: %d %d\n", prevHeight, prevWidth);
    }
    int pixelIndex = 0;
    for(int i = 0; i < (prevHeight - getPoolSize() + 1); i+= getStride())
    {
        for(int j = 0; j < (prevWidth -getPoolSize() + 1) ; j+= getStride())
        {
            double max = 0.0;
            //printf("first for\n");
            //no padding necessary
            for(int ii = 0; ii < getPoolSize(); ii++)
                    for(int jj =0; jj < getPoolSize(); jj++)
                    {
                        //printf("second for %d \n", (i * prevWidth + j) + (ii * getPoolSize() + jj));
                        
                        double temp = prev.getData()[(i * prevWidth + j) + (ii * getPoolSize() + jj)];
                        //printf("temp:%f\n",temp);
                        poolIndex->getData()[pixelIndex] = (i * prevWidth + j) + (ii * getPoolSize() + jj);
                        //printf("i:%d\n",poolIndex->getData()[pixelIndex]);
                        if(max < temp)
                        {
                            max = temp;
                            
                            poolIndex->getData()[pixelIndex] = (i * prevWidth + j) + (ii * getPoolSize() + jj);
                            //printf("INDEX= %llu\n", poolIndex->getData()[pixelIndex]);
                            
                        }

                        
                    }
            
            mapT->getData()[pixelIndex++] = max;
            printf("%s: %f %d\n", getID(), mapT->getData()[pixelIndex-1], poolIndex->getData()[pixelIndex-1]);
  
        }//end of J loop
    }//end of I loop
    printf("finished\n");
    
    mapT->writePartial2DData(gcomm,numElems,batchIndex,mapIndex,getPatchHeight()*getPatchWidth());
    poolIndex->writePartial2DData(gcomm,numElems,batchIndex,mapIndex,getPatchHeight()*getPatchWidth());
    
    delete poolIndex;
    delete mapT;
    
}

