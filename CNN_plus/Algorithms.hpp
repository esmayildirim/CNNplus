//
//  Algorithms.h
//  
//
//  Created by Esma Yildirim on 11/16/16.
//
//

#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <stdio.h>
class Algorithms{
public:
    static void distributeLoad(int batchSize, int maps, int myRank, int p, int &bStart, int &bEnd, int &mStart, int &mEnd, int &myMaps,int &myMapIndex);
    static bool indicesWithinRange(int globalMapIndex, int myTotalMaps, int mapIndex, int batchIndex, int batchSize);
private:
    static void getMapIndices(int myMapsIndex,int myMaps, int batchSize,int maps, int & mStart, int & mEnd, int & bStart, int & bEnd);
    static int getBatchIndex(int batchSize, int maps, int globalMapIndex);
    static int getMapIndex(int batchSize, int maps, int globalMapIndex);
};
#endif /* defined(____Algorithms__) */
