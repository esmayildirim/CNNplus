//
//  Algorithms.cpp
//  
//
//  Created by Esma Yildirim on 11/16/16.
//
//
#ifndef ALGORITHMS_HPP
#include "Algorithms.hpp"
#endif




void Algorithms::distributeLoad(int batchSize, int maps, int myRank, int p, int &bStart, int &bEnd, int &mStart, int &mEnd, int &myMaps,int &myMapIndex)
{
    int totalMaps = maps * batchSize;
    int mapsProc = totalMaps / p;
    int remainder = totalMaps % p;
    
    if(remainder > 0 && myRank < remainder )
        myMaps = mapsProc + 1;
    else myMaps = mapsProc;
    
    
    if(remainder > 0){
        if(myRank < remainder)
            myMapIndex = myMaps * myRank;
        else myMapIndex = myMaps * myRank + remainder;
    }
    else myMapIndex = myMaps * myRank;
    
    getMapIndices(myMapIndex, myMaps, batchSize, maps, mStart, mEnd, bStart, bEnd);
}

void Algorithms::getMapIndices(int myMapIndex,int myMaps, int batchSize,int maps, int & mStart, int & mEnd, int & bStart, int & bEnd)
{
    mStart = getMapIndex(batchSize, maps, myMapIndex );
    mEnd = getMapIndex(batchSize,  maps, myMapIndex+ myMaps - 1);
    bStart = getBatchIndex(batchSize, maps, myMapIndex);
    bEnd = getBatchIndex(batchSize, maps, myMapIndex + myMaps - 1);
}

int Algorithms::getBatchIndex(int batchSize, int maps, int globalMapIndex)
{
    int mapIndex = getMapIndex(batchSize, maps, globalMapIndex);
    return globalMapIndex - (mapIndex * batchSize);
}
int Algorithms::getMapIndex(int batchSize, int maps, int globalMapIndex)
{
    return (globalMapIndex / (batchSize));
}

bool Algorithms::indicesWithinRange(int globalMapIndex, int myTotalMaps, int mapIndex, int batchIndex, int batchSize)
{
    int temp = mapIndex * batchSize + batchIndex;
    if (temp >=globalMapIndex && temp < globalMapIndex + myTotalMaps)
        return true;
    else return false;
}
