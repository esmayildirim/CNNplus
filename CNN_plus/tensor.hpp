//
//  tensor.hpp
//  
//
//  Created by Esma Yildirim on 11/14/16.
//
//

#ifndef MPIABSTRACTION_HPP
#include "mpiabstraction.hpp"
#endif

#ifndef TENSOR_HPP
#define TENSOR_HPP




#include <stdio.h>
#include <unistd.h>
#include "dataspaces.h"
#include <mpi.h>
#include <random>
#include <chrono>

template <class T>
class Tensor{
public:
    Tensor(){};
    Tensor(char * id, int d);
    ~Tensor();
    void setID(char * id){ID = id;};
    void setDims(int d){dims = d;};
    void setDimSizes(int * ds){dimSizes = ds;};
    char * getID(){return ID;};
    int getDims(){return dims;};
    int * getDimSizes(){return dimSizes;};
    void createData(int size);
    T* getData();
    void initData(int myRank, int p, MPI_Comm * gcomm, int mode = 0);
    int write3DData(int myRank,int p, MPI_Comm * gcomm);
    int write1DData(int myRank,int p, MPI_Comm * gcomm);
    void readPartial3DData(MPI_Comm * gcomm, int noElements, int startIndex);
    void readPartial1DData(MPI_Comm * gcomm, int noElements, int startIndex);
    void readPartial2DData(MPI_Comm * gcomm, int noElements, int startIndex);
    void writePartial2DData(MPI_Comm * gcomm, int noElements, int batchIndex, int mapIndex, int mapSize);
private:
    char * ID;
    int dims;
    int * dimSizes;
    T * data;
};




template <class T>
Tensor<T>::Tensor(char * id, int d){
    ID = id;
    //dimSizes = new int[d];
    dims = d;
}
template <class T>
Tensor<T>::~Tensor(){
    if(dimSizes != nullptr)
        delete [] dimSizes;
    if (data != nullptr)
        delete [] data;
    printf("Destroyed data and dimensions for %s\n", getID());
}
template <class T>
void Tensor<T>::createData(int size)
{
    data = new T[size];
    printf("Created %d space for %s\n", size, getID());
}

template <class T>
T* Tensor<T>::getData()
{
    return data;
}

template <class T>
void Tensor<T>::initData(int myRank, int p, MPI_Comm * gcomm, int mode)
{
    static unsigned seed = chrono::system_clock::now().time_since_epoch().count();//
    //unsigned seed = 456;
    static default_random_engine generator (seed);
    
    static normal_distribution<double> distribution(0.0, 1.0);//zero mean, unit standard deviation

    //randomly assign numbers to data then broadcast it then write in parallel to dspaces so that each piece goes to a different server
    int numElems = 1;
    for (int i = 0; i < dims; i++) {
        numElems *= dimSizes[i];
    }
    if(myRank == 0){

        if(mode == 0){
            for (int j = 0 ; j < numElems; j++) {
                data[j] = 0.0;
            }
        }else {
        for (int j = 0 ; j < numElems; j++) {
            data[j] = 0.01 * distribution(generator);
        }
        }
    }
    
    //broadcast the data to all
  
    MPI_Bcast(data,numElems, ConvertType(getAbstractionDataType<T>()), 0, *gcomm);
    
}

template <class T>
int Tensor<T>::write3DData(int myRank,int p, MPI_Comm * gcomm)
{
    printf("%d %d\n", getDims()-1, dimSizes[getDims()-1]);
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
    
     printf("%d:filter bounds: %lld-%lld\n",myRank, myStartIndex, myNoParams);
    if(myNoParams > 0){
        for(int i = 0; i < myNoParams ; i ++)
        {
            
            for(int j = 0 ; j < dimSizes[1] ; j ++)
            {
                lb[0] = 0;
                ub[0] = dimSizes[0] - 1;
                lb[1] = ub[1] = j;
                lb[2] = ub[2] = i + myStartIndex;
                int err = dspaces_put(getID(), 0, sizeof(T), ndim, lb, ub, &data[i * dimSizes[1] * dimSizes[0] + j * dimSizes[0]]);
                dspaces_put_sync();
           
                //printf("%d:write %s %d %d %d-%d\n",myRank, getID(), lb[0], ub[0], lb[1], lb[2]);
             //   for (int x = 0; x < dimSizes[0]; x++) {
             //       printf("%f\t",data[i * dimSizes[1] * dimSizes[0] + j * dimSizes[0]+x] );
             //   }
                
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

template <class T>
int Tensor<T>::write1DData(int myRank,int p, MPI_Comm * gcomm)
{
    //decide who will write based on the last dimension
    int myNoParams = dimSizes[0]/ p;
    int remainder = dimSizes[0] % p;
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
    int ndim = 2; // originally a 4D tensor, height and width are combined into 1D
    dspaces_define_gdim(getID(), ndim, gdim);
    uint64_t lb[2]; uint64_t ub[2];
    
    // printf("%d:filter bounds: %lld-%lld\n",my_rank, my_filter_lower_bound, filter_number_per_proc);
    if(myNoParams > 0){
                lb[0] = myStartIndex;
                ub[0] = myStartIndex + myNoParams -1 ;
                lb[1] = ub[1] = 0;
                
                int err = dspaces_put(getID(), 0, sizeof(T), ndim, lb, ub, &data[myStartIndex]);
                dspaces_put_sync();
        
                if(err!=0) printf("ERROR:write_conv_weights:%d\n", err);
                while(err);
                
                //    for (int k =0; k < filter_size * filter_size; k++) {
                //   printf("%d writes:%f at \t%d %d %d\n", my_rank, filter_weights[i][j][k], lb[2], lb[1], k);
                //    }
    }
    dspaces_unlock_on_write(lockName, gcomm);
    
    
    
    return 0;
}

template <class T>
void Tensor<T>::readPartial3DData(MPI_Comm * gcomm, int noElements, int startIndex)
{
    uint64_t gdim[3];
    gdim[0] = dimSizes[0];
    gdim[1] = dimSizes[1];
    gdim[2] = dimSizes[2]; // a square R|G|B image is stored one dimensional
    int ndim = getDims(); // originally a 4D tensor
    const char *lockName = string(getID()).append("_readlock").c_str();
    //printf("ndim:%d\n", ndim);
    dspaces_lock_on_read(lockName, gcomm);
    dspaces_define_gdim(getID(), ndim, gdim);
    uint64_t lb[3]; uint64_t ub[3];
    
    if(noElements > 0 ){
        
        for(int i = 0; i < noElements ; i ++)
        {
            
            for(int j = 0 ; j < dimSizes[1] ; j ++)
            {
                
                lb[0] = 0;
                ub[0] = dimSizes[0] -1;
                lb[1] = ub[1] = j;
                lb[2] = ub[2] = i + startIndex ;
                int err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, &data[i * dimSizes[0] * dimSizes[1]+ j * dimSizes[0]]);
                //printf("READ: %s %lld %lld %lld %lld\n", getID(),lb[0],ub[0], lb[1], lb[2]);
                while(err!=0) {
                    sleep(1);
                    printf("ERROR:read_conv_weights:%d\n", err);
                    err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, &data[i * dimSizes[0] * dimSizes[1]+ j * dimSizes[0]]);
                }
                
            }
            
        }
    }
    dspaces_unlock_on_read(lockName, gcomm);

}

template <class T>
void Tensor<T>::readPartial2DData(MPI_Comm * gcomm, int noElements, int startIndex)
{
    uint64_t gdim[2];
    gdim[0] = dimSizes[0];
    gdim[1] = dimSizes[1];
    int ndim = getDims();
    const char *lockName = string(getID()).append("_readlock").c_str();
    //printf("ndim:%d\n", ndim);
    dspaces_lock_on_read(lockName, gcomm);
    dspaces_define_gdim(getID(), ndim, gdim);
    uint64_t lb[2]; uint64_t ub[2];
    
    if(noElements > 0 ){
        for (int i = 0; i < noElements; i++) {
            
            lb[0] = 0;
            ub[0] = dimSizes[0]-1;
            lb[1] = ub[1] = i+ startIndex;
            int err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, &data[i * dimSizes[0]]);
            printf("READ: %s lb0:%lld ub0:%lld lb1:%lld\n", getID(),lb[0],ub[0], lb[1]);
            while(err!=0) {
                sleep(1);
                printf("ERROR:read_conv_weights:%d\n", err);
                err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, &data[i * dimSizes[0]]);
                
            }

            
        }
    }
    dspaces_unlock_on_read(lockName, gcomm);
}

template <class T>
void Tensor<T>::writePartial2DData(MPI_Comm * gcomm, int noBatchElems, int batchIndex, int mapIndex, int mapSize)
{
    uint64_t gdim[2];
    gdim[0] = dimSizes[0];
    gdim[1] = dimSizes[1];
    int ndim = getDims();
    const char *lockName = string(getID()).append("_writelock").c_str();
    //printf("ndim:%d\n", ndim);
    dspaces_lock_on_write(lockName, gcomm);
    dspaces_define_gdim(getID(), ndim, gdim);
    uint64_t lb[2]; uint64_t ub[2];
    
    if(noBatchElems > 0 ){
        for (int i = 0; i < noBatchElems; i++) {
            
            lb[0] = mapIndex * mapSize;
            ub[0] = lb[0] + mapSize -1;
            lb[1] = ub[1] = i+ batchIndex;
            int err = dspaces_put(getID(), 0, sizeof(T), ndim, lb, ub, data);
            printf("WRITTEN: %s lb0:%lld ub0:%lld lb1:%lld\n", getID(),lb[0],ub[0], lb[1]);
            if(err!=0) printf("ERROR:write partial 2D data :%d\n", err);
            while(err);
            
            
        }
    }
    dspaces_unlock_on_write(lockName, gcomm);
}



template <class T>
void Tensor<T>::readPartial1DData(MPI_Comm * gcomm, int noElements, int startIndex)
{
    uint64_t gdim[2];
    gdim[0] = dimSizes[0];
    gdim[1] = dimSizes[1];
    int ndim = getDims(); // originally a 4D tensor
    const char *lockName = string(getID()).append("_readlock").c_str();
    printf("ndim:%d\n", ndim);
    dspaces_lock_on_read(lockName, gcomm);
    dspaces_define_gdim(getID(), ndim, gdim);
    uint64_t lb[2]; uint64_t ub[2];
    
    if(noElements > 0 ){
                lb[0] = startIndex;
                ub[0] = startIndex + noElements -1;
                lb[1] = ub[1] = 0;
                int err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, data);
                //printf("READ: %s %lld %lld %lld\n", getID(),lb[0],ub[0], lb[1]);
                while(err!=0) {
                    sleep(1);
                    printf("ERROR:read_conv_weights:%d\n", err);
                    err = dspaces_get(getID(), 0, sizeof(T), ndim, lb, ub, data);
                
                }
    }
    dspaces_unlock_on_read(lockName, gcomm);
    
}
#endif /* defined(____tensor__) */
