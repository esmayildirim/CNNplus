#ifndef INPUTLAYER_HPP
#include "inputlayer.hpp"
#endif

#ifndef MNISTLAYER_HPP
#include "mnistlayer.hpp"
#endif 

//#ifndef CONVLAYER_HPP
//#include "convlayer.hpp"
//#endif

#ifndef FCLAYER_HPP
#include "fclayer.hpp"
#endif

#ifndef POOLINGLAYER_HPP
#include "poolinglayer.hpp"
#endif

#ifndef SOFTMAXLAYER_HPP
#include "softmaxlayer.hpp"
#endif

#ifndef NETWORK_HPP
#include "network.hpp"
#endif 

#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP
/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>
//#include "network.hpp"
//#include "layer.hpp"
// Error codes

enum {
   ERROR_ARGS = 1, 
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

class GetConfig
{
public:
    GetConfig();
    ~GetConfig();
    void readConfigFile(std::string&, Network &net) throw(std::runtime_error);
 
    char *getID() { return id; };

private:
    xercesc::XercesDOMParser *m_ConfigFileParser;
    char* id;

   // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

    XMLCh* TAG_network;
    XMLCh* TAG_layer;
    XMLCh* TAG_type;
    XMLCh* TAG_maps;
    XMLCh* TAG_stride;
    XMLCh* TAG_filter_size;
    XMLCh* TAG_pool_size;
    XMLCh* TAG_padding;
    XMLCh* TAG_pred_id;
    XMLCh* TAG_weight_id;
    XMLCh* TAG_bias_id;
    XMLCh* TAG_op;
    XMLCh* TAG_nodes;
    XMLCh* ATTR_id;
    XMLCh* ATTR_subtype;
    XMLCh* ATTR_act_func;
   
};
#endif

