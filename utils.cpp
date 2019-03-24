#include"utils.h"

#define MAX_PATH 260
//lmh add
cv::Scalar colortable[25] = { cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 125),
cv::Scalar(0, 125, 125), cv::Scalar(125, 125, 125), cv::Scalar(125, 0, 0), cv::Scalar(125, 125, 0), cv::Scalar(0, 125, 0), cv::Scalar(125, 0, 125),
cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 255), cv::Scalar(255, 255, 255), cv::Scalar(255, 0, 0), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0),
cv::Scalar(255, 0, 255), cv::Scalar(0, 255, 100), cv::Scalar(0, 0, 100),
cv::Scalar(255, 0, 100), cv::Scalar(255, 255, 100), cv::Scalar(100, 100, 100),
cv::Scalar(255, 125, 65), cv::Scalar(255, 65, 125), cv::Scalar(255, 255, 100), cv::Scalar(100, 100, 100),
cv::Scalar(255, 125, 65) };
string classname[25] = { "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow",
"diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor", "tricycle","truck","trolley","Manhole","Shelter" };

void drawImage(Mat& image, vector<ResultBox> result)
{
	for (vector<ResultBox>::iterator it = result.begin(); it != result.end(); it++)
	{
		ResultBox box = *it;
		rectangle(image, box, Scalar(0, 0, 255), 2);
		char strTemp[100];
		sprintf(strTemp, "%s : %.3f", box.className, box.score);
		putText(image, strTemp, Point(max(min((int)box.x + 20, image.cols - 1), 20), max(min((int)box.y + 20, image.rows - 1), 20)), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0)/*colortable[i]*/, 1, 2);
	}
}

//void drawImage(Mat image, std::map<int, vector<aboxes> > classer)
//{
//	char strTemp[100];
//
//	for (int i = 0; i < 22; i++)
//	{
//		if (!classer[i].empty())
//		{
//			for (auto ite = classer[i].begin(); ite != classer[i].end(); ite++)
//			{
//				aboxes test = *ite;
//				rectangle(image, Rect(test.x1, test.y1, test.x2 - test.x1 + 1, test.y2 - test.y1 + 1), colortable[i], 1 + test.score * 2, 4, 0);
//				sprintf_s(strTemp, "%s:%.3f", classname[i], test.score);
//				putText(image, strTemp, Point(max(min((int)test.x1 + 10, image.cols - 1), 10), max(min((int)test.y1 + 10, image.rows - 1), 10)), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0)/*colortable[i]*/, 1, 2);
//			}
//		}
//	}
//}

void drawArea(Mat& image, AreaInfo areaInfo)
{
	//draw area 
	vector<Rect>::iterator it_area = areaInfo.areas.begin();
	for (int i = 0; i < areaInfo.areas.size(); i++)
	{
		rectangle(image, (*it_area), Scalar(255, 0, 0), 2);
		it_area++;
	}
}

std::map<int, vector<aboxes> > detectArea(Mat img, FasterRcnnDetector &detector, AreaInfo &areainfo)
{
	//待检测的区域
	vector<Rect> areas = areainfo.areas;
	//最终检测结果
	std::map<int, vector<aboxes> > result;
	vector<aboxes> vec;
	for (int i = 0; i < 25; i++)
	{
		result.insert(std::pair<int, vector<aboxes> >(i, vec));
	}

	//遍历每一个检测区域
	for (auto iter_area = areas.begin(); iter_area != areas.end(); iter_area++)
	{
		Rect area = *iter_area;
		Mat image_roi(img, area);
		std::map<int, vector<aboxes> >  result_temp = detector.detect(image_roi);

		//遍历每一个类别
		for (int i = 0; i < 25; i++)
		{
			//遍历一个类别的所有aboxs
			for (auto iter_abox = result_temp[i].begin(); iter_abox != result_temp[i].end(); iter_abox++)
			{
				aboxes box_roi = *iter_abox;
				aboxes box;
				box.x1 = box_roi.x1 + area.x;
				box.y1 = box_roi.y1 + area.y;
				box.x2 = box_roi.x2 + area.x;
				box.y2 = box_roi.y2 + area.y;
				box.score = box_roi.score;
				result[i].push_back(box);
			}
		}
	}
	return result;
}

//获得指定类别的所有目标
vector<ResultBox> filter_class(std::map<int, vector<aboxes> > classer, int classId, float score)
{
	vector<ResultBox> result;
	if (!classer[classId].empty())
	{
		for (auto iter = classer[classId].begin(); iter != classer[classId].end(); iter++)
		{
			if (iter->score >= score)
			{
				aboxes abox = *iter;
				ResultBox resbox(Rect(abox.x1, abox.y1, abox.x2 - abox.x1, abox.y2 - abox.y1));
				resbox.score = abox.score;
				resbox.className = classname[classId];
				result.push_back(resbox);
			}
		}
	}
	return result;
}

void readImageNames(const string dir, vector<string> &imgFileNames)
{
	char tmpDirSpec[MAX_PATH + 1];
	sprintf(tmpDirSpec, "%s*", dir.c_str());

	WIN32_FIND_DATAA f;
	HANDLE h = FindFirstFileA(tmpDirSpec, &f); // read .
	if (h != INVALID_HANDLE_VALUE)
	{
		FindNextFileA(h, &f);	//read ..
		while (FindNextFileA(h, &f))
			imgFileNames.push_back(f.cFileName);
	}
	FindClose(h);
}