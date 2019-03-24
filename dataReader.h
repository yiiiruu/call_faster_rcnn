#ifndef DATA_READER_H
#define DATA_READER_H
#include"typeDefine.h"
#include<opencv2/opencv.hpp>
#include<libxml/parser.h>
#include<libxml/tree.h>
#include<libxml/encoding.h>
#include<libxml/xmlwriter.h>

using namespace std;
using namespace cv;

class dataReader
{
public:
	dataReader(const string path);
	bool parseXMLFile(AreaInfo& areaInfo);
private:
	xmlDocPtr file;
	xmlNodePtr root;
	xmlNodePtr rootChildren;
	xmlNodePtr areaChildren;
	xmlChar* temp;
};

#endif