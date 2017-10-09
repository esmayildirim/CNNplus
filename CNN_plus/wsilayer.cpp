//
//  WSILayer.cpp
//  
//
//  Created by Esma Yildirim on 11/9/16.
//
//

#ifndef WSILAYER_HPP
#include "wsilayer.hpp"
#endif
#include <iostream>
int WSILayer::index = 0;
using namespace cv;
WSILayer::WSILayer(char * wpath, char * lpath)
{
    wsiPath = wpath;
    labelPath = lpath;
}

void WSILayer::initCoordList(int level)
{
    //get the file list in the directory path
    vector<string> list;
    DIR * directory;
    struct dirent *dir;
    struct stat pathStat;
    stat(getWsiPath(), &pathStat);
    if(!S_ISDIR(pathStat.st_mode))
    {
        list.push_back(string(wsiPath));
    }
    else{
        directory = opendir(getWsiPath());
        while((dir = readdir(directory))!= NULL)
        {
            if(strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0 || strncmp(".", dir->d_name, 1) == 0)
                continue;
            string s(wsiPath);
            s.append("/");
            s.append(dir->d_name);
            list.push_back(s);
        }
        closedir(directory);
    }
    //get dimensions then add them to the vector based on patch size and width
    int pwidth, pheight;
    pwidth = getPatchWidth();
    pheight = getPatchHeight();
    cout <<pwidth <<" "<<pheight << endl;
       for (int i =0; i < list.size(); i++) {
        openslide_t *osr = openslide_open(list[i].c_str());
        //cout << list[i].c_str() << endl;
        int64_t width, height;
        openslide_get_level_dimensions(osr, level, &width, &height);
        //cout << width << "X" << height << endl;
        for (int j = 0; j < width; j += pwidth) {
            for (int k = 0; k < height; k += pheight) {
                WSI coordObj;
                coordObj.setX(j);
                coordObj.setY(k);
                coordObj.setPatchWidth(pwidth);
                coordObj.setPatchHeight(pheight);
                coordObj.setPath(list[i]);
                coordObj.setWidth(width);
                coordObj.setHeight(height);
                add(coordObj);
            }
        }
        openslide_close(osr);
        
    }
 
    
    
}

int WSILayer::getTumorLabel(WSI &item)
{
     char * slash = strrchr(item.getPath().c_str(), '/');
     char * under = strrchr(item.getPath().c_str(), '.');
     int i_slash = slash - item.getPath().c_str() + 1;
     int i_under = under - item.getPath().c_str() + 1;
     int len = i_under - i_slash; //including \0 character
     char * file_name = (char *)malloc(len);
     memcpy(file_name, &item.getPath().c_str()[i_slash], len - 1);
     file_name[len-1]  = '\0';
     char new_path[200];
     sprintf(new_path, "%s/%s_Mask.tif", getLabelPath(),file_name);
     printf("%s\n",new_path);
    return 0;
}

void WSILayer::uploadNextBatch(int myRank, int p, int batchSize, int level, MPI_Comm *gcomm)
{
    //for each element in the selected coord list
    //calculate which element to read based on p and rank
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
    
    //global batch object dimensions
    uint64_t gdim[2],lb[2],ub[2];
    gdim[0] = getPatchHeight() * getPatchWidth() * getMaps();
    gdim[1] = batchSize;
    dspaces_define_gdim(getID(),2,gdim);
    //create a lock name
    const char *lockName = string(getID()).append("_writelock").c_str();
    dspaces_lock_on_write(lockName, gcomm);
    for (int i = 0; i < myNoImages; i++) {
        //set up local coordinates
        lb[0] = 0;
        ub[0] = getPatchHeight() * getPatchWidth() * getMaps() - 1;
        lb[1] = ub[1] = i + myStartIndex;
        //read the image
   
        openslide_t *osr = openslide_open(coordList[myStartIndex + i + index].getPath().c_str());
        int64_t l0_width, l0_height;
        openslide_get_level_dimensions(osr, 0, &l0_width, &l0_height);
        uint64_t xcoord = coordList[myStartIndex + i + index].getX();
        uint64_t ycoord = coordList[myStartIndex + i + index].getY();
        xcoord *= l0_width/coordList[myStartIndex + i + index].getWidth();
        ycoord *= l0_height/coordList[myStartIndex + i + index].getHeight();
        uint64_t numBytes = getPatchWidth() * getPatchHeight() * 4;
        uint32_t * buffer = (uint32_t *)malloc(numBytes);
        getTumorLabel(coordList[myStartIndex + i + index]);
        openslide_read_region(osr, buffer, xcoord, ycoord, level,getPatchWidth(), getPatchHeight());
        openslide_close(osr);
     
        Mat image = Mat(getPatchHeight(), getPatchWidth(), CV_8UC3 );
        unsigned char * charBuffer = (unsigned char *)buffer;
        for( int j=0; j<getPatchHeight(); j++ ){
            for( int i=0; i<getPatchWidth(); i++ ){
                for( int iChn=0; iChn<3; iChn++ ){
                    image.at<Vec3b>(j,i)[iChn] = charBuffer[j*getPatchWidth()*4+i*4+(iChn)];
                }//END_FOR_iCh
            }//END_FOR_I
        }//END_FOR_J
        
        
        
        double  * updatedBuffer = zeroCenterNormalize(image);
        //write to dspaces
        int err = dspaces_put(getID(), 0, sizeof(double), 2, lb, ub, updatedBuffer);
        dspaces_put_sync();
        if(err!=0) printf("ERROR:write_image_batch:%d\n", err);
        while(err);
        free(updatedBuffer);
        free(buffer);
    
        //calculate the labels in the mean time
        
    }
    //set index for next batch
    index += batchSize;
    dspaces_unlock_on_write(lockName, gcomm);
}