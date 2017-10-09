//
//  tensor.cpp
//  
//
//  Created by Esma Yildirim on 11/14/16.
//
//
#ifndef MPIABSTRACTION_HPP
#include "mpiabstraction.hpp"
#endif

#ifndef TENSOR_HPP
#include "tensor.hpp"
#endif

#include <iostream>
#include <mpi.h>
#include <random>
#include <chrono>

unsigned seed = chrono::system_clock::now().time_since_epoch().count();//
//unsigned seed = 456;
default_random_engine generator (seed);

normal_distribution<double> distribution(0.0, 1.0);//zero mean, unit standard deviation



template <class T>
Tensor<T>::Tensor(char * id, int d){
    ID = id;
    dimSizes = new int[d];
}
template <class T>
Tensor<T>::~Tensor(){
    if(dimSizes != nullptr)
        delete [] dimSizes;
    if (data != nullptr)
        delete [] data;
}
template <class T>
void Tensor<T>::createData(int size)
{
    data = new T[size];
}

template <class T>
T* Tensor<T>::getData()
{
    return data;
}

template <class T>
void Tensor<T>::initData(int myRank, int p, MPI_Comm * gcomm)
{
    //randomly assign numbers to data then broadcast it then write in parallel to dspaces so that each piece goes to a different server
    if(myRank == 0){
        int k = 0;
        for (int i = 0; i < dims; i++) {
            for (int j = 0 ; j < dimSizes[i]; j++) {
                data[k++] = 0.01 * distribution(generator);
            }
        }
    
    }
    
    //broadcast the data to all
    MPI_Bcast(data,dimSizes[0]*dimSizes[1]*dimSizes[2], convertType(getAbstractionDataType<T>()), 0, *gcomm);
    
  }
template <class T>
void Tensor<T>::initDataFixed(int myRank, int p, MPI_Comm * gcomm, T number)
{
    //randomly assign numbers to data then broadcast it then write in parallel to dspaces so that each piece goes to a different server
    if(myRank == 0){
        int k = 0;
        for (int i = 0; i < dims; i++) {
            for (int j = 0 ; j < dimSizes[i]; j++) {
                data[k++] = number;
            }
        }
        
    }
    
    //broadcast the data to all
    MPI_Bcast(data,dimSizes[0]*dimSizes[1]*dimSizes[2], convertType(getAbstractionDataType<T>()), 0, *gcomm);
    
}




template <class T>
int Tensor<T>::write3DData(int myRank,int p, MPI_Comm * gcomm)
{
    //decide who will write based on the last dimension
    int myNoParams = dimSizes[getDims()-1]/ p;
    int remainder = dimSizes[getDims()-1] % p;
    int myStartIndex = myRank * myNoParams;
    if (remainder > 0) {
        if(myRank < remainder)
        {
            myNoParams++;
            myStartIndex = myRank * myNoParams;
        }else myStartIndex = myNoParams * myRank + remainder;
    }
    const char *lockName = string(getID()).append("_writelock").c_str();
    dspaces_lock_on_write(lockName, gcomm);
    uint64_t gdim[3];
    gdim[0] = dimSizes[0];
    gdim[1] = dimSizes[1];
    gdim[2] = dimSizes[2]; // a square R|G|B image is stored one dimensional
    int ndim = 3; // originally a 4D tensor, height and width are combined into 1D
    dspaces_define_gdim(getID(), getDims(), gdim);
    uint64_t lb[3]; uint64_t ub[3];
    
    // printf("%d:filter bounds: %lld-%lld\n",my_rank, my_filter_lower_bound, filter_number_per_proc);
    if(myNoParams > 0){
        for(int i = 0; i < myNoParams ; i ++)
        {
            
            for(int j = 0 ; j < dimSizes[1] ; j ++)
            {
                lb[0] = 0;
                ub[0] = dimSizes[0] - 1;
                lb[1] = ub[1] = j;
                lb[2] = ub[2] = i + myStartIndex;
                int err = dspaces_put(getID(), 0, sizeof(T), ndim, lb, ub, data[i * dimSizes[1] * dimSizes[0] + j * dimSizes[0]]);
                dspaces_put_sync();
                //printf("write %s %d %d %d-%d\n",getID(), lb[2], lb[1], lb[0], ub[0]);
                if(err!=0) printf("ERROR:write_conv_weights:%d\n", err);
                while(err);
                
                //    for (int k =0; k < filter_size * filter_size; k++) {
                //   printf("%d writes:%f at \t%d %d %d\n", my_rank, filter_weights[i][j][k], lb[2], lb[1], k);
                //    }
                
            }
            
        }
    }
    dspaces_unlock_on_write(lockName, gcomm);

    

    return 0;
}


