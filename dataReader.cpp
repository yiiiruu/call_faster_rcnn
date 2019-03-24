#include"dataReader.h"
#include<Windows.h>

dataReader::dataReader(const string path)
{
	char tmpFileSpec[MAX_PATH + 1];
	sprintf(tmpFileSpec, "%s", path.c_str());
	file = xmlReadFile(tmpFileSpec, "UTF-8", XML_PARSE_RECOVER);
	if (file == NULL)
	{
		cout << "fail to open" << endl;
		return;
	}
}

bool dataReader::parseXMLFile(AreaInfo& areaInfo)
{
	root = xmlDocGetRootElement(file);
	if (root == NULL)
	{
		cout << "empty file" << endl;
		return false;
	}

	rootChildren = root->children;
	while (rootChildren != NULL)
	{
		//get ID
		if (!xmlStrcmp(rootChildren->name, BAD_CAST"ID"))
		{
			temp = rootChildren->children->content;
			areaInfo.ID = (char*)temp;
		}

		//get area
		if (!xmlStrcmp(rootChildren->name, BAD_CAST"area"))
		{
			Rect area;
			areaChildren = rootChildren->children;
			while (areaChildren != NULL)
			{
				if (!xmlStrcmp(areaChildren->name, BAD_CAST"xmin"))
				{
					temp = areaChildren->children->content;
					area.x = atoi((char*)temp);
				}
				if (!xmlStrcmp(areaChildren->name, BAD_CAST"ymin"))
				{
					temp = areaChildren->children->content;
					area.y = atoi((char*)temp);
				}
				if (!xmlStrcmp(areaChildren->name, BAD_CAST"xmax"))
				{
					temp = areaChildren->children->content;
					area.width = atoi((char*)temp) - area.x;
				}
				if (!xmlStrcmp(areaChildren->name, BAD_CAST"ymax"))
				{
					temp = areaChildren->children->content;
					area.height = atoi((char*)temp) - area.y ;
				}

				areaChildren = areaChildren->next;
			}
			areaInfo.areas.push_back(area);
		}

		//get thresh
		if (!xmlStrcmp(rootChildren->name, BAD_CAST"thresh"))
		{
			temp = rootChildren->children->content;
			areaInfo.thresh = atof((char*)temp);
		}

		//get timeStamp
		if (!xmlStrcmp(rootChildren->name, BAD_CAST"timeStamp"))
		{
			temp = rootChildren->children->content;
			areaInfo.timeStamp = atol((char*)temp);
		}

		//get hardExamplePath
		if (!xmlStrcmp(rootChildren->name, BAD_CAST"hardExamplePath"))
		{
			temp = rootChildren->children->content;
			areaInfo.hardExamplePath = (char*)temp;
		}

		rootChildren = rootChildren->next;
	}

	return true;
}