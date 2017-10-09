/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */



#include <string>
#include <stdexcept>
//#include "layer.hpp"

#ifndef NETWORK_HPP
#include "network.hpp"
#endif

Network::Network()
{
    count = 0;
    lRate = 1e-4;
    momentum = 1e-3;
    regStrength = 1e-3;
}

void Network::printLayerList()
{
    for (int i = 0; i < layerVector.size(); i++){
        cout << "ID: "<<layerVector[i]->getID()<< endl;
        if(i != 0)
            cout << "Predecessor: " << layerVector[i]->getPredID() << endl;
        if(layerVector[i]->getType() == LINPUT)
        {
            InputLayer * il = (InputLayer *)layerVector[i];
            cout << "type: INPUT"<<endl;
            cout << "maps: "<< il->getMaps() << endl;
        }
        else if(layerVector[i]->getType() == LCONV)
        {
            cout << "type: CONV"<<endl;
            ConvLayer *cl = (ConvLayer *)layerVector[i];
            cout << "maps: "<< cl->getMaps() << endl;
            cout << "stride: "<< cl->getStride() << endl;
            cout << "filter_size: " << cl->getFilterSize() << endl;
            cout << "weight-id: " << cl->getWeightID() << endl;
            cout << "bias-id: " << cl->getBiasID() << endl;
        }
        else if(layerVector[i]->getType() == LPOOL)
        {
            cout << "type: POOL"<<endl;
            PoolLayer *pl = (PoolLayer *)layerVector[i];
            //find the map number for pool layer
            //Layer * temp = pl -> getPrevLayer();
            //while(temp->getType() != LCONV)
            //    temp = temp -> getPrevLayer();
            //pl -> setMaps(static_cast<PoolLayer *>(temp) -> getMaps());
            cout << "maps: "<< pl->getMaps() << endl;
            cout << "stride: "<< pl->getStride() << endl;
            cout << "pool_size: " << pl->getPoolSize() << endl;
            if(pl->getOps() == MAX) cout << "Op: Max" << endl;
            else cout << "Op: Avg"<<endl;
        }
        else if(layerVector[i]->getType() == LFC)
        {
            cout << "type: FC"<<endl;
            FCLayer *fl = (FCLayer *)layerVector[i];
            cout << "nodes: "<< fl->getNumNodes() << endl;
            cout << "weight-id: " << fl->getWeightID() << endl;
            cout << "bias-id: " << fl->getBiasID() << endl;
            
        }
        else if(layerVector[i]->getType() == LSOFTMAX)
        {
            cout << "type: Softmax"<<endl;
            SoftmaxLayer *sl = (SoftmaxLayer *)layerVector[i];
            cout << "nodes: "<< sl->getNumNodes() << endl;
        }

        
    }


}


//int main()
//{
//
//    return 0;
//}