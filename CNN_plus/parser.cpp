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

using namespace xercesc;
using namespace std;
/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

GetConfig::GetConfig()
{
    try
    {
        XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
    }
    catch( XMLException& e )
    {
        char* message = XMLString::transcode( e.getMessage() );
        cerr << "XML toolkit initialization error: " << message << endl;
        XMLString::release( &message );
        // throw exception here to return ERROR_XERCES_INIT
    }
    
    // Tags and attributes used in XML file.
    // Can't call transcode till after Xerces Initialize()
    TAG_network  = XMLString::transcode("network");
    TAG_layer = XMLString::transcode("layer");
    TAG_type  =  XMLString::transcode("type");
    TAG_maps = XMLString::transcode("maps");
    TAG_stride = XMLString::transcode("stride");
    TAG_filter_size = XMLString::transcode("filter_size");
    TAG_pool_size = XMLString::transcode("pool_size");
    TAG_padding = XMLString::transcode("padding");
    TAG_pred_id = XMLString::transcode("pred_id");
    TAG_weight_id = XMLString::transcode("weight_id");
    TAG_bias_id = XMLString::transcode("bias_id");
    TAG_op = XMLString::transcode("op");
    TAG_nodes = XMLString::transcode("nodes");
    ATTR_id = XMLString::transcode("id");
    ATTR_subtype = XMLString::transcode("subtype");
     ATTR_act_func = XMLString::transcode("act_func");
    m_ConfigFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

GetConfig::~GetConfig()
{
    // Free memory
    
    delete m_ConfigFileParser;
    // if(m_OptionA)   XMLString::release( &m_OptionA );
    // if(m_OptionB)   XMLString::release( &m_OptionB );
    
    try
    {
        XMLString::release( &TAG_network );
        XMLString::release( &TAG_layer );
        XMLString::release( &TAG_type );
        XMLString::release( &TAG_maps );

        XMLString::release( &TAG_stride );
        XMLString::release( &TAG_filter_size );
        XMLString::release( &TAG_pool_size );
        XMLString::release( &TAG_padding );
        XMLString::release( &TAG_pred_id );
        XMLString::release( &TAG_weight_id );
        XMLString::release( &TAG_bias_id );
        XMLString::release( &TAG_op );
        XMLString::release( &TAG_nodes );
        XMLString::release( &ATTR_id );
        XMLString::release( &ATTR_subtype );
        XMLString::release( &ATTR_act_func );
        
    }
    catch( ... )
    {
        cerr << "Unknown exception encountered in TagNamesdtor" << endl;
    }
    
    // Terminate Xerces
    
    try
    {
        XMLPlatformUtils::Terminate();  // Terminate after release of memory
    }
    catch( xercesc::XMLException& e )
    {
        char* message = xercesc::XMLString::transcode( e.getMessage() );
        
        cerr << "XML ttolkit teardown error: " << message << endl;
        XMLString::release( &message );
    }
}

/**
 *  This function:
 *  - Tests the access and availability of the XML configuration file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param in configFile The text string name of the HLA configuration file.
 */

void GetConfig::readConfigFile(string& configFile, Network &net)
throw( std::runtime_error )
{
    // Test to see if the file is ok.
    
    struct stat fileStatus;
    
    errno = 0;
    if(stat(configFile.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
    {
        if( errno == ENOENT )      // errno declared by include file errno.h
            throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
        else if( errno == ENOTDIR )
            throw ( std::runtime_error("A component of the path is not a directory."));
        else if( errno == ELOOP )
            throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
        else if( errno == EACCES )
            throw ( std::runtime_error("Permission denied."));
        else if( errno == ENAMETOOLONG )
            throw ( std::runtime_error("File can not be read\n"));
    }
    
    // Configure DOM parser.
    
    m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
    m_ConfigFileParser->setDoNamespaces( false );
    m_ConfigFileParser->setDoSchema( false );
    m_ConfigFileParser->setLoadExternalDTD( false );
    
    try
    {
        m_ConfigFileParser->parse( configFile.c_str() );
        
        // no need to free this pointer - owned by the parent parser object
        DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();
        
        // Get the top-level element: NAme is "root". No attributes for "root"
        
        DOMElement* elementRoot = xmlDoc->getDocumentElement();
        if( !elementRoot ) throw(std::runtime_error( "empty XML document" ));
        
        // Parse XML file for tags of interest: "network"
        // Look one level nested within "root". (child of root)
        
        DOMNodeList*      children = elementRoot->getChildNodes();
        const  XMLSize_t nodeCount = children->getLength();
        //cout << nodeCount << endl;
        // For all nodes, children of "root" in the XML tree.
        
        for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
        {
            DOMNode* currentNode = children->item(xx);
            if( currentNode->getNodeType() &&  // true is not NULL
               currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
            {
                // Found node which is an Element. Re-cast node as element
                DOMElement* currentElement
                = dynamic_cast< xercesc::DOMElement* >( currentNode );
                if( XMLString::equals(currentElement->getTagName(), TAG_layer))
                {
                    //Found a layer
                    Layer * polyLayer;
                    
                    const XMLCh* xmlch_id = currentElement->getAttribute(ATTR_id);
                    // cout << "before setting id"<< endl;
                    char * temp_id = XMLString::transcode(xmlch_id);
                    //temp.setID( XMLString::transcode(xmlch_id) );
                    //cout << temp_id << endl;
                    const XMLCh* xmlch_act =currentElement->getAttribute(ATTR_act_func);
                    char * temp_func = XMLString::transcode(xmlch_act);
                    DOMNodeList* currentElementChildren = currentElement -> getChildNodes();
                    const XMLSize_t count = currentElementChildren -> getLength();
                   // cout << count<<endl;
                    for (XMLSize_t yy =0; yy < count; ++yy)
                    {   //cout<<"hello"<<endl;
                        DOMNode * currentNodeChild = currentElementChildren -> item(yy);
                        
                        if( currentNodeChild->getNodeType() &&  // true is not NULL
                           currentNodeChild->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
                        {
                            //cout<<"hello2"<<endl;
                            DOMElement *currentElementChild = dynamic_cast< xercesc::DOMElement* >( currentNodeChild );
                            if (XMLString::equals(currentElementChild -> getTagName(),TAG_type)) {
                                const XMLCh* xmlch_type = currentElementChild -> getTextContent();
                                char * type = XMLString::transcode(xmlch_type);
                                //cout<<"hello"<<endl;
                                if(strcmp(type, "input")==0){
                                    const XMLCh* xmlch_subtype = currentElementChild->getAttribute(ATTR_subtype);
                                    char * subtype = XMLString::transcode(xmlch_subtype);
                                    cout << subtype << endl;
                                    if(strcmp(subtype, "mnist")==0)
                                    {
                                        polyLayer = new MNISTLayer;
                                        cout<< "creating mnist object"<<endl;
                                    
                                    }
                                    else polyLayer = new InputLayer;
                                    polyLayer->setType(LINPUT);
                                    if(strcmp(temp_func, "true")==0)
                                        polyLayer->setReLU(true);
                                    else polyLayer->setReLU(false);
                                }
                                else if(strcmp(type, "conv")==0) {
                                    polyLayer = new ConvLayer;
                                    polyLayer->setType(LCONV);
                                    if(strcmp(temp_func, "true")==0)
                                        polyLayer->setReLU(true);
                                    else polyLayer->setReLU(false);
                                }
                                else if(strcmp(type, "pool")==0){
                                    polyLayer = new PoolLayer;
                                    polyLayer->setType(LPOOL);
                                    if(strcmp(temp_func, "true")==0)
                                        polyLayer->setReLU(true);
                                    else polyLayer->setReLU(false);
                                }
                                else if(strcmp(type, "fc")==0){
                                    polyLayer = new FCLayer;
                                    polyLayer->setType(LFC);
                                    if(strcmp(temp_func, "true")==0)
                                        polyLayer->setReLU(true);
                                    else polyLayer->setReLU(false);
                                }
                                else if(strcmp(type, "softmax")==0) {
                                    polyLayer = new SoftmaxLayer;
                                    polyLayer->setType(LSOFTMAX);
                                    if(strcmp(temp_func, "true")==0)
                                        polyLayer->setReLU(true);
                                    else polyLayer->setReLU(false);
                                }
                                else{
                                    polyLayer = new FCLayer();
                                    polyLayer->setType(LUNDEFINED);
                                }
                                
                                //cout << "\t"<<polyLayer->getType() << endl;
                                free(type);
                                polyLayer->setID(temp_id);
                                
                            }
                           
                             else if (XMLString::equals(currentElementChild -> getTagName(),TAG_maps)) {
                             const XMLCh* xmlch_maps = currentElementChild -> getTextContent();
                             char * maps = XMLString::transcode(xmlch_maps);
                                 
                                 ConvLayer * cl = static_cast<ConvLayer *> (polyLayer);
                                 cl->setMaps(atoi(maps));
                                 //cout << "\t"<< cl->getMaps() << endl;
                                 free(maps);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(),TAG_stride)) {
                                 const XMLCh* xmlch_stride = currentElementChild -> getTextContent();
                                 char * stride = XMLString::transcode(xmlch_stride);
                                 ConvLayer * cl = static_cast<ConvLayer *>(polyLayer);
                                 cl->setStride(atoi(stride));
                                 //cout << "\t"<< cl->getStride() << endl;
                                 free(stride);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_filter_size)) {
                                 const XMLCh* xmlch_size = currentElementChild -> getTextContent();
                                 char * size = XMLString::transcode(xmlch_size);
                                 ConvLayer * cl = (ConvLayer *)polyLayer;
                                 cl->setFilterSize(atoi(size));
                                 //cout << "\t"<< cl->getFilterSize() << endl;
                                 free(size);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_pool_size)) {
                                 const XMLCh* xmlch_size = currentElementChild -> getTextContent();
                                 char * size = XMLString::transcode(xmlch_size);
                                 PoolLayer *pl = (PoolLayer *) polyLayer;
                                 pl->setPoolSize(atoi(size));
                                 //cout << "\t"<< pl->getPoolSize() << endl;
                                 free(size);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_padding)) {
                                 const XMLCh* xmlch_padding = currentElementChild -> getTextContent();
                                 char * padding = XMLString::transcode(xmlch_padding);
                                 ConvLayer * cl = (ConvLayer *)polyLayer;
                                 if(strcmp(padding, "true")==0) cl->setPadding(true);
                                 else cl->setPadding(false);
                                 //cout << "\t"<< cl->getPadding() << endl;
                                 free(padding);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_pred_id)) {
                                 const XMLCh* xmlch_pred_id = currentElementChild -> getTextContent();
                                 char * pred_id = XMLString::transcode(xmlch_pred_id);
                                 polyLayer->setPredID(pred_id);
                                 //cout << "\t"<< polyLayer->getPredID() << endl;
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_weight_id)) {
                                 const XMLCh* xmlch_weight_id = currentElementChild -> getTextContent();
                                 char * weight_id = XMLString::transcode(xmlch_weight_id);
                                 polyLayer->setWeightID(weight_id);
                                 //cout << "\t"<< polyLayer->getWeightID() << endl;
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_bias_id)) {
                                 const XMLCh* xmlch_bias_id = currentElementChild -> getTextContent();
                                 char * bias_id = XMLString::transcode(xmlch_bias_id);
                                 polyLayer->setBiasID(bias_id);
                                 //cout << "\t"<< polyLayer->getBiasID() << endl;
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(),TAG_op)) {
                                 const XMLCh* xmlch_op = currentElementChild -> getTextContent();
                                 char * op = XMLString::transcode(xmlch_op);
                                 PoolLayer * pl = (PoolLayer *)polyLayer;
                                 if(strcmp(op, "max")==0) pl->setOps(MAX);
                                 else if(strcmp(op, "avg")==0) pl->setOps(AVG);
                                 //cout << "\t"<< pl->getOps() << endl;
                                 free(op);
                             }
                             else if (XMLString::equals(currentElementChild -> getTagName(), TAG_nodes)) {
                                 const XMLCh* xmlch_nodes = currentElementChild -> getTextContent();
                                 char * nodes = XMLString::transcode(xmlch_nodes);
                                 FCLayer * fl = (FCLayer *)polyLayer;
                                 fl->setNumNodes(atoi(nodes));
                                 //cout << "\t"<< fl->getNumNodes() << endl;
                                 free(nodes);
                             }
                            
                            
                        }
                        
                    }
                    net.addLayer(polyLayer);
                    // break;  // Data found. No need to look at other elements in tree.
                }
            }
        }//end of for xx
        
    }//END OF TRY
    catch( xercesc::XMLException& e )
    {
        char* message = xercesc::XMLString::transcode( e.getMessage() );
        ostringstream errBuf;
        errBuf << "Error parsing file: " << message << flush;
        XMLString::release( &message );
    }
    
}

//#ifdef MAIN_TEST
/* This main is provided for unit test of the class. */


//#endif

