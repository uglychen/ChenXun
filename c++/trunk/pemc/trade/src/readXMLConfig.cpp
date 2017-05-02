#include "readXMLConfig.h"
#include "zlog.h"

readXMLConfig::readXMLConfig()
{

}

readXMLConfig::~readXMLConfig()
{

}

TiXmlNode * readXMLConfig::selectChildNode(TiXmlNode * pNode,string nodeName,string nodeAttrName,string nodeAttrValue)  
{  
    if(pNode == NULL)  
    {  
        return NULL;  
    }  
    TiXmlNode * pSelectedNode = NULL;  
    TiXmlNode * pChildNode = NULL;  
    int t = pNode->Type();  
    switch (t)  
    {  
    case TiXmlText::TINYXML_DOCUMENT:  
    case TiXmlText::TINYXML_DECLARATION:  
    case TiXmlText::TINYXML_TEXT:  
    case TiXmlText::TINYXML_UNKNOWN:  
    case TiXmlText::TINYXML_COMMENT:  
        break;  
    case TiXmlText::TINYXML_ELEMENT:  
    if(pNode->Value() == nodeName)  
    {  
        //cout << pNode->Value() << endl;  
        if(!nodeAttrName.empty() && !nodeAttrValue.empty())  
        {  
            TiXmlElement * pElement = pNode->ToElement();  

            TiXmlAttribute * pAttr = pElement->FirstAttribute();  
            TiXmlAttribute * pNextAttr =NULL;  
            if(pAttr != NULL)  
            {    
                do  
                {                            
                    if(pAttr->Name()==nodeAttrName&&pAttr->Value()== nodeAttrValue)  
                    {  
                        //cout << pAttr->Value() << endl;  
                        return pNode;  
                    }  
                }while(pAttr = pAttr->Next());  
            }  
        }  
        else  
        {  
            return pNode;  
        }  
             
    }  
    else  
    {  
        //循环访问它的每一个元素  
        for(pChildNode=pNode->FirstChild();  
            pChildNode!=0;  
            pChildNode = pChildNode->NextSibling())  
        {  
            pSelectedNode = selectChildNode(  
                                pChildNode,  
                                nodeName,  
                                nodeAttrName,  
                                nodeAttrValue);  
            if(pSelectedNode)  
            {  
                return pSelectedNode;  
            }  
        }  
    }  
 
    default:break;  
    }  
    return NULL;  
}

TiXmlNode * readXMLConfig::SelectSingleNodeByRootEle(TiXmlElement* RootElement,string & nodeName,string nodeAttrName,string nodeAttrValue)  
{  
	//加载一个XML的文档对象。  
	 
	//  TiXmlDocument *xmlDoc = new TiXmlDocument(cXmlName);  
	//  if(!xmlDoc->LoadFile())  //是tinyXml会自动处理文档的BOM  
	//  {  
	//      return NULL;  
	//  }  
	//    
	//    
	//  if(xmlDoc == NULL)  
	//  {  
	//      return NULL;  
	//  }  
	 
	//获得根元素  
	//TiXmlElement *RootElement = xmlDoc->RootElement();  
    if(RootElement == NULL)  
    {  
        // cout << "parse error，can't get root element" << endl;
        dzlog_error("parse error，can't get root element.");  
        return NULL;  
    }  
     
    TiXmlNode * pNode  = NULL;  
    TiXmlNode * pSelectNode = NULL;  
    string msg = "";  
     
    for(pNode=RootElement->FirstChildElement();pNode;pNode=pNode->NextSiblingElement())  
    {  
         
        pSelectNode = selectChildNode(pNode,nodeName,nodeAttrName,nodeAttrValue);  
        if(pSelectNode)  
        {  
            break;  
        }  
    }  
     
    if(pSelectNode)  
    {  
        //cout << "解析成功" << endl;  
        //cout << pSelectNode->Value() << endl;  
        return pSelectNode;  
    }  
    else  
    {  
        // cout << "parse error，can't get node" << endl;  
        dzlog_error("parse error，can't get node.");  
        return NULL;  
    }  
     
}


int readXMLConfig::getNodeValueByName(string & filPath, string & nodeEleName, string & nodeEleValue)
{
	TiXmlDocument doc(filPath.c_str()); 
	bool loadOkay = doc.LoadFile();  
	if (!loadOkay) 
	{      
		// printf( "Could not load test file %s. Error='%s'. Exiting.\n", filPath.c_str(), doc.ErrorDesc() );
        dzlog_error("Could not load test file %s. Error='%s'. Exiting.", filPath.c_str(), doc.ErrorDesc() );  
		// exit( 1 );  
		return -1;
	}     


	TiXmlElement* root = doc.RootElement();
	string nodeAttrName = "";
	string nodeAttrValue = "";

	TiXmlNode * tmp = SelectSingleNodeByRootEle(root, nodeEleName, nodeAttrName, nodeAttrValue);
	if (NULL == tmp)
	{
		// printf("can't get node value.\n");
        dzlog_error("can't get node value.");  
		return -1;
	}
	nodeEleValue = tmp->ToElement()->GetText();
	// cout << nodeEleValue << endl;

	return 0;
}



