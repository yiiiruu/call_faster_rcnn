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
//���ָ�������Ŀ��
std::map<int, vector<aboxes> > detectArea(Mat img, FasterRcnnDetector &detector, AreaInfo &areainfo);
//���ָ����������Ŀ��,classIdΪĿ�����������scoreΪ���Ŷ�
vector<ResultBox> filter_class(std::map<int, vector<aboxes> > classer, int classId, float score);
//��ȡָ���ļ����µ�����ͼƬ�ļ�
void readImageNames(const string dir, vector<string> &imgFileNames);
#endif