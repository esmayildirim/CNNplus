//
//  mnistlayer.cpp
//
//
//  Created by Esma Yildirim on 11/14/16.
//
//
#ifndef MNISTLAYER_HPP
#include "mnistlayer.hpp"
#endif
#include <fstream>

uint64_t MNISTLayer::dataOffset = 0;
uint64_t MNISTLayer::labelOffset = 0;

void MNISTLayer::uploadNextBatch(int myRank, int p, int batchSize, MPI_Comm *gcomm, vector<int> &labels)
{
    int myNoImages = batchSize/ p;
    int remainder = batchSize % p;
    int myStartIndex = myRank * myNoImages;
    if (remainder > 0) {
        if(myRank < remainder)
        {
            myNoImages++;
            myStartIndex = myRank * myNoImages;
        }else myStartIndex = myNoImages * myRank + remainder;
    }
    //open the data and label files and seek to the defined offsets
    ifstream dataFile, labelFile;
    dataFile.open(getDataPath(),ios::binary|ios::in);
    labelFile.open(getLabelPath(), ios::binary|ios::in);
    uint64_t startDataOffset = 16 + dataOffset + myStartIndex * getPatchWidth() * getPatchHeight();
    uint64_t startLabelOffset = 8 + labelOffset + myStartIndex;
    dataFile.seekg (startDataOffset, dataFile.beg);
    labelFile.seekg(startLabelOffset, labelFile.beg);
    
    //set up dataspaces variables
    uint64_t gdim[2];
    uint64_t bufferLength = getMaps() * getPatchHeight() * getPatchWidth();
    gdim[0] = bufferLength;
    gdim[1] = batchSize;
    dspaces_define_gdim(getID(), 2, gdim);
    
    char * buffer = ( char * )malloc(bufferLength * sizeof( char));
    const char *lockName = string(getID()).append("_writelock").c_str();
    dspaces_lock_on_write(lockName,gcomm);
    for (int i = 0; i < myNoImages; i++) {
        dataFile.read (buffer, bufferLength);
        char label;
        labelFile.read(&label, 1);
        labels.push_back((int)label);
        
        Mat image = Mat( getPatchHeight(), getPatchWidth(), CV_8UC1 );
        for( int j=0; j<getPatchWidth(); j++ ){
            for( int k=0; k< getPatchWidth(); k++ ){
                image.at<unsigned char>(j,k) = buffer[j * getPatchWidth() + k]; //a change was made
                //printf("%d\t",image.at<unsigned char>(j,k));
                
            }//END_FOR_iCh
        }//END_FOR_I
                       //printf("\n");
        //imshow("",image);
       // waitKey(0);
        double *updatedData = zeroCenterNormalize(image);
        
       /* Mat gray_img(image.rows, image.cols, CV_64F);
        for (int i = 0; i < image.rows; i++)
            for (int j = 0; j < image.cols; j++)
                gray_img.at<double>(i,j) = updatedData[i * image.cols + j];
        
        Mat dst;
        normalize(gray_img, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        
        
        imshow("",dst);
        while(true)
        {
            int c;
            c = waitKey( 20 );
            if( (char)c == 27 )
            { break; }
        }

        */
        
        
        //write into dataspaces
        uint64_t lb[2], ub[2];
        ub[0] = bufferLength - 1;
        lb[0] = 0;
        lb[1] = ub[1] = myStartIndex + i;
        int err = dspaces_put(getID(), 0, sizeof(double), 2, lb, ub, updatedData);
        printf("WRITING %s %lld %lld-%lld\n", getID(), lb[0], ub[0], lb[1] );
        dspaces_put_sync();
        if(err!=0) printf("ERROR:write_image_batch:%d\n", err);
        while(err);
        free(updatedData);
        
    }
    dspaces_unlock_on_write(lockName,gcomm);
    
    free(buffer);
    //free(lockName);
    dataOffset += getPatchWidth() * getPatchHeight() * getMaps() * batchSize;
    labelOffset += batchSize;
    
    dataFile.close();
    labelFile.close();
    
}




