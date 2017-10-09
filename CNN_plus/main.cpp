//
//  main.cpp
//  
//
//  Created by Esma Yildirim on 11/9/16.
//
//
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#ifndef XML_PARSER_HPP
#include "parser.hpp"
#endif

//#ifndef OPENSLIDE_H
//#include "openslide.h"
//#endif

//#ifndef MNISTLAYER_HPP
//#include "mnistlayer.hpp"
//#endif

//#ifndef TENSOR_HPP
//#include "tensor.hpp"
//#endif

#ifndef POOLINGLAYER_HPP
#include "poolinglayer.hpp"
#endif

#include "mpi.h"

using namespace xercesc;
using namespace std;

int main(int argc, char **argv)
{
    
    int  myRank; /* rank of process */
    int  p;       /* number of processes */
    MPI_Status status ;   /* return status for receive */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm gcomm = MPI_COMM_WORLD;
    dspaces_init(p, 1,  &gcomm, NULL);
    /*network parameters */
    int batchSize = 1;
    int pWidth = 28;
    int pHeight = 28;
    //int level = 4;
    int channels = 1;
    int learningCycles = 1;
    int trainSetSize = 60000;
    /********************/
    string configFile="./network.xml"; // defines the layers of the network.
    GetConfig appConfig;
    Network convNetwork;
    appConfig.readConfigFile(configFile, convNetwork);
    vector<Layer *> list = convNetwork.getVector();
    printf("SIZE: %d\n", list.size());
    for (int i = 0; i < list.size(); i++) {
        if(i == 0) {
             list[i]->setNextLayer(list[i+1]);
            list[i]->setPrevLayer(nullptr);
           
            //printf("%s %s %s\n", list[i]->getID(), "NULL", list[i]->getNextLayer()->getID());
        }
        else if(i == list.size()-1){
            list[i]->setNextLayer(nullptr);
            list[i]->setPrevLayer(list[i-1]);
            
            //printf("%s %s %s\n", list[i]->getID(), list[i-1]->getPrevLayer()-> getID(), "NULL");
        }
        else {
            list[i]->setNextLayer(list[i+1]);
            list[i]->setPrevLayer(list[i-1]);
            
            //printf("%s %s %s\n", list[i]->getID(), list[i]->getPrevLayer()->getID(), list[i]->getNextLayer()->getID());
        }
        //printf("Layer:%s\n", list[i]->getID());
    }
   // convNetwork.printLayerList();
    
    //assume first layer is input
    MNISTLayer * layer;
    vector<int> batchLabels;
    if(list[0]->getType() == LINPUT)
    {
        layer = static_cast<MNISTLayer *>(list[0]);
        layer->setDataPath("/Users/eyildirim/Documents/MNIST/mnist_train");
        layer->setLabelPath("/Users/eyildirim/Documents/MNIST/mnist_train_labels");
        layer->setPatchWidth(pWidth);
        layer->setPatchHeight(pHeight);
        layer->setMaps(channels);
        cout << "MAPS:"<<layer -> getMaps() << endl;
        //layer->initCoordList(level);
        
    }
    else
    {
        //throw an exception in a try catch block
    
    }
    MPI_Barrier(MPI_COMM_WORLD);
    int numIterations = trainSetSize/batchSize;
    cout << numIterations << endl;
    for(int iter = 0 ; iter < 5; iter++)
    {
        //cout << iter << endl;
        //upload the next batch into dataspaces
        layer ->uploadNextBatch(myRank, p, batchSize, &gcomm, batchLabels);
        //start layer calculations and weight initializations
        for (int j = 1; j < 2; j++) {
             if(list[j]->getType() == LCONV)
             {   
                 ConvLayer * layer = static_cast<ConvLayer *>(list[j]);
                 if(iter == 0)
                 {
                     //create the weights and biases
                    
                     Tensor<double> weights(layer->getWeightID(),3);
                     int * dims = new int[3];
                     dims[0] = pow(layer->getFilterSize(), 2);
                     if(layer->getPrevLayer()->getType()== LINPUT || layer->getPrevLayer()->getType()== LPOOL)
                     {   InputLayer * prev = static_cast<InputLayer *>(layer->getPrevLayer());
                         dims[1] = prev->getMaps();
                         printf("ID: %s DIMS1: %d\n", prev->getID(), dims[1]);
                     }
                     dims[2] = layer->getMaps();
                     weights.setDimSizes(dims);
                     weights.createData(dims[0] * dims[1] * dims[2]);
                     weights.initData(myRank, p , &gcomm, 1);
                     weights.write3DData(myRank, p , &gcomm);
                    
                     Tensor<double> biases(layer->getBiasID(),2);
                     int * dims2 = new int[2];
                     dims2[0] = layer->getMaps();
                     dims2[1] = 1;
                     biases.setDimSizes(dims2);
                     biases.createData(dims2[0] * dims2[1]);
                     biases.initData(myRank, p , &gcomm, 0);
                     biases.write1DData(myRank, p , &gcomm);
                 }
                 
                 layer->calculatePatchWidthHeight();
                 layer->convolution(myRank, p, &gcomm, batchSize);
                 
             
             }
            else if(list[j]->getType() == LPOOL)
            {
                PoolLayer * layer = static_cast<PoolLayer *>(list[j]);
                layer->calculatePatchWidthHeight();
                layer->pool(myRank, p, &gcomm, batchSize);

            }
            else if(list[j]->getType() == LFC)
            {}
            else if(list[j]->getType() == LSOFTMAX)
            {}
            else
            {}
            
        }
        //backpropagate with another for loop
    
    }
  
    dspaces_finalize();
    MPI_Finalize();
    return 0;
}
