//
//  wsi.hpp
//  
//
//  Created by Esma Yildirim on 11/10/16.
//
//

#ifndef WSI_HPP
#define WSI_HPP

#include <string>
#include <sys/types.h>
#include <inttypes.h>
using namespace std;
class WSI {
public:
    WSI(){};
    ~WSI(){};
    void setX(int64_t x){ X = x;};
    void setY(int64_t y){ Y = y;};
    void setPatchWidth(int w){ patchWidth = w;};
    void setPatchHeight(int h){ patchHeight = h;};
    void setWidth(int64_t gw){ width = gw;};
    void setHeight(int64_t gh){ height = gh;};
    void setPath(string p){path = p;}
    int64_t getX(){return X;};
    int64_t getY(){return Y;};
    int getPatchWidth(){return patchWidth;};
    int getPatchHeight(){return patchHeight;};
    int64_t getWidth(){return width;};
    int64_t getHeight(){return height;};
    string getPath(){return path;};
    
private:
    int64_t X;
    int64_t Y;
    int patchWidth;
    int patchHeight;
    int64_t width;
    int64_t height;
    string path;
};

#endif
