//
//  fclayer.hpp
//  
//
//  Created by Esma Yildirim on 11/8/16.
//
//

#ifndef LAYER_HPP
#include "layer.hpp"
#endif

#ifndef FCLAYER_HPP
#define FCLAYER_HPP

class FCLayer: public Layer
{
public:
    FCLayer(){};
    ~FCLayer(){};
    void setNumNodes(int n){ numNodes = n;};
    int getNumNodes(){ return numNodes;};
private:
    int numNodes;
};


#endif
