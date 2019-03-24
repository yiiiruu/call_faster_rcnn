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
	//====================��������========================================
	//��ʼ��faster-rcnn�����
	FasterRcnnDetector detector(rpn_prototxt_file, rpn_model_file, fcnn_prototxt_file, fcnn_model_file, mean_image_file);

	//�������ٹ��߶���
	TrackUtils *trackUtils = new TrackUtils();

	//��ȡ������Ϣ
	string areaID = "08010168";
	string areaInfoPath = AREA_INFO_XML_PATH_CARS + areaID + ".xml";
	dataReader reader(areaInfoPath);
	AreaInfo areaInfo;
	reader.parseXMLFile(areaInfo);

	//��ȡ����ͼƬ�б�
	vector<string> imageFiles;
	const string imageDir = IMAGE_DIR;
	readImageNames(imageDir, imageFiles);

	//����ʱ����ֵ
	time_t alertTime = 10;
	//ͼƬ�������·��
	string resultDir = RESULT_DIR;

	for (vector<string>::iterator iter = imageFiles.begin(); iter != imageFiles.end(); iter++)
	{
		//��ȡ�����ͼƬ
		Mat image = imread(imageDir + *iter);
		//���ƷǷ�����
		drawArea(image, areaInfo);
		cout << "size" << areaInfo.areas.size() << endl;

		//�������ͼƬĿ��
		//std::map<int, vector<aboxes> > result = detector.detect(image);
		//���ָ������Ŀ��
		std::map<int, vector<aboxes> > result = detectArea(image, detector, areaInfo);
		//����Ŀ�����,�õ�ĳһ������,����6Ϊcar��������У�0.6Ϊ���Ŷȣ��������Ŷ�<0.6�Ķ����˵�
		vector<ResultBox> result_filter = filter_class(result, 6, 0.8);
		////���Ƽ����
		drawImage(image, result_filter);
		//drawImage(image, result_filter);

		//���ٲ����Ƽ����
		bool isAlarm = false;
		time_t nowTime;
		time(&nowTime);
		trackUtils->trackAllViews(image, areaID, result_filter, nowTime, alertTime, isAlarm);

		//���ƽ��ͼƬ
		Mat image_resized;
		//����ͼƬ�ߴ�Ϊԭ����0.5
		resize(image, image_resized, Size2f(image.cols * IMAGE_SCALE, image.rows * IMAGE_SCALE));
		imshow("showImage", image_resized);
		waitKey(0);

		//���汨��ͼƬ���
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