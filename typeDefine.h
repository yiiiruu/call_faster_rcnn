#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H
#include<opencv2/opencv.hpp>
using namespace cv;

struct ResultBox :public Rect
{
	ResultBox(){}
	ResultBox(Rect r) :Rect(r){}
public:
	float score;
	string className;
};

struct AreaInfo
{
	string ID;
	vector<Rect> areas;
	float thresh;
	time_t timeStamp;
	string hardExamplePath;
};


#endif