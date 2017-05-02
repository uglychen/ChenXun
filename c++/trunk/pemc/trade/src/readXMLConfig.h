#ifndef _READXMLCONFIG_H_
#define _READXMLCONFIG_H_
#include "tinyxml.h"
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class readXMLConfig
{
public:
	readXMLConfig();
	~readXMLConfig();
	int getNodeValueByName(string & filPath, string & nodeEleName, string & nodeEleValue);
	
private:

	/**
	  根据父节点循环遍历查找子节点
	  参数说明
	    noteName 节点名
	    noteAttrName 属性名
	    noteAttrValue 属性值
	*/  
	TiXmlNode * selectChildNode(TiXmlNode * pNode, string nodeName, string nodeAttrName, string nodeAttrValue);
	TiXmlNode * SelectSingleNodeByRootEle(TiXmlElement* RootElement, string & nodeName, string nodeAttrName, string nodeAttrValue);
	
};

#endif // !_READXMLCONFIG_H_
