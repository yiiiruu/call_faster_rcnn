#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>
#include "faster_rcnn_test.h"
#include "utils.h"
#include "dataReader.h"
#include "typeDefine.h"
#include "parameters.h"
#include "trackUtils.h"

using namespace cv;
using namespace std;

//matlab model and net prototxt file .
const string rpn_prototxt_file = "../models3/proposal_test.prototxt";
const string rpn_model_file = "../models3/proposal_final";
const string fcnn_prototxt_file = "../models3/detection_test.prototxt";
const string fcnn_model_file = "../models3/detection_final";

//mean_image.bmp is convert from the model.mat  in  matlab model directory 
//you can save mean_image by command  imwrite(uint8(proposal_detection_model.image_means),"mean_image.bmp")
const string mean_image_file = "../models3/mean_image.bmp";

int main()
{
	//====================调用例子========================================
	//初始化faster-rcnn检测器
	FasterRcnnDetector detector(rpn_prototxt_file, rpn_model_file, fcnn_prototxt_file, fcnn_model_file, mean_image_file);

	//创建跟踪工具对象
	TrackUtils *trackUtils = new TrackUtils();

	//读取区域信息
	string areaID = "08010168";
	string areaInfoPath = AREA_INFO_XML_PATH_CARS + areaID + ".xml";
	dataReader reader(areaInfoPath);
	AreaInfo areaInfo;
	reader.parseXMLFile(areaInfo);

	//读取测试图片列表
	vector<string> imageFiles;
	const string imageDir = IMAGE_DIR;
	readImageNames(imageDir, imageFiles);

	//报警时间阈值
	time_t alertTime = 10;
	//图片结果保存路径
	string resultDir = RESULT_DIR;

	for (vector<string>::iterator iter = imageFiles.begin(); iter != imageFiles.end(); iter++)
	{
		//读取待检测图片
		Mat image = imread(imageDir + *iter);
		//绘制非法区域
		drawArea(image, areaInfo);
		cout << "size" << areaInfo.areas.size() << endl;

		//检测整张图片目标
		//std::map<int, vector<aboxes> > result = detector.detect(image);
		//检测指定区域目标
		std::map<int, vector<aboxes> > result = detectArea(image, detector, areaInfo);
		//过滤目标类别,得到某一类检测结果,其中6为car的类别所有，0.6为置信度，即把置信度<0.6的都过滤掉
		vector<ResultBox> result_filter = filter_class(result, 6, 0.8);
		////绘制检测结果
		drawImage(image, result_filter);
		//drawImage(image, result_filter);

		//跟踪并绘制检测结果
		bool isAlarm = false;
		time_t nowTime;
		time(&nowTime);
		trackUtils->trackAllViews(image, areaID, result_filter, nowTime, alertTime, isAlarm);

		//绘制结果图片
		Mat image_resized;
		//缩放图片尺寸为原来的0.5
		resize(image, image_resized, Size2f(image.cols * IMAGE_SCALE, image.rows * IMAGE_SCALE));
		imshow("showImage", image_resized);
		waitKey(0);

		//保存报警图片结果
		if (isAlarm)
		{
			string str;
			stringstream ss;
			ss << nowTime;
			ss >> str;
			string resultPath = resultDir + str;
			resultPath += ".jpg";
			imwrite(resultPath, image);
		}
	}
	return 0;
}