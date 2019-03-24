#ifndef UTILS_H
#define UTILS_H
#include<opencv2/opencv.hpp>
#include<vector>
#include<string>
#include<map>
#include<windows.h>
#include "faster_rcnn_test.h"
#include "dataReader.h"
#include "typeDefine.h"

using namespace cv;
using std::vector;
using std::string;

//void drawImage(Mat image, std::map<int, vector<aboxes> > classer);
void drawImage(Mat& image, vector<ResultBox> result);
void drawArea(Mat& image, AreaInfo areaInfo);
//检测指定区域的目标
std::map<int, vector<aboxes> > detectArea(Mat img, FasterRcnnDetector &detector, AreaInfo &areainfo);
//获得指定类别的所有目标,classId为目标类别索引，score为置信度
vector<ResultBox> filter_class(std::map<int, vector<aboxes> > classer, int classId, float score);
//读取指定文件夹下的所有图片文件
void readImageNames(const string dir, vector<string> &imgFileNames);
#endif