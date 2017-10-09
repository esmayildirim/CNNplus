//
//  inputlayer.cpp
//  
//
//  Created by Esma Yildirim on 11/11/16.
//
//

#ifndef INPUTLAYER_HPP
#include "inputlayer.hpp"
#endif
#include <iostream>
double * InputLayer::zeroCenterNormalize(Mat &image)
{
        int width = image.cols;
        int height = image.rows;
        int channels = image.channels();
        double * output = (double *)malloc(height * width * channels *sizeof(double));
        
        Scalar avg,sdv;
        meanStdDev(image, avg, sdv);
        sdv.val[0] = sqrt(image.cols*image.rows*sdv.val[0]*sdv.val[0]);
        Mat image_64f;
        image.convertTo(image_64f,CV_64FC3,1/sdv.val[0],-avg.val[0]/sdv.val[0]);
        
        for (int i = 0; i < image_64f.rows; i++) {
            for (int j = 0; j < image_64f.cols; j++) {
                
                for (int k = 0; k < image_64f.channels(); k++){
                    output[image_64f.cols * k * image_64f.rows + i * image_64f.cols + j] = image_64f.at<Vec3d>(i,j)[k];
                    //printf("%f\n",image_64f.at<Vec3d>(i,j)[k]);
                }
                
            }
        }
        
       
    /*
    int width = image.cols;
    int height = image.rows;
    int channels = image.channels();
    double * mean = (double *) calloc(channels, sizeof(double));
    double * std = (double *) calloc(channels, sizeof(double));
    double * output = (double *)malloc(height * width * channels *sizeof(double));
    
    
    //#pragma omp parallel
    {
        for(int k = 0; k < channels; k++)
        {
            //calculate mean for each channel
            // #pragma omp for reduction(mean[k])
            for (int i = 0 ; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    mean[k] += image.at<Vec3b>(i,j)[k];
                }
            }
            mean[k] /= width * height;
            //printf("mean:%f\n", mean[k]);
            //calculate standard deviation for each channel
            //  #pragma omp for reduction(std[k])
            for (int i = 0 ; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std[k] += pow((image.at<Vec3b>(i,j)[k] - mean[k]), 2);
                    
                }
            }
            std[k] = sqrt(std[k]/ (width * height - 1));
            //printf("std:%f\n", std[k]);
            //zero center and normalize
            //#pragma omp for
            for (int i = 0; i< height; i++) {
                for (int j = 0; j< width; j++) {
                    double temp = image.at<Vec3b>(i,j)[k] - mean[k];
                    temp /= std[k];
                    output[k * width * height + width * i + j] =  image.at<Vec3b>(i,j)[k] - mean[k];
                    output[k * width * height + width * i + j] /= std[k];
                    //printf("%f\t", output[k * width * height + width * i + j]);
                }
            }
            
            
        }//end of k loop
    }//end of pragma omp parallel
     
     */
    return output;

}