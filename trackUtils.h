#ifndef TRACK_UTILS
#define TRACK_UTILS

#include<opencv2\opencv.hpp>
#include"typeDefine.h"
using namespace cv;
using namespace std;

typedef struct TrkObject
{
	string id;//ID = 点位名字+唯一数字
	string class_name;//类别名字
	float score;//置信度
	Rect bbox;//最近一帧检测结果bbox
	vector<Rect> bbox_history;//扩展，保存所有目标的历史记录，当前没有用到
	time_t start_time;//开始跟踪时间
	time_t continue_time;//持续跟踪时间
	int lost_time;//丢失时间,若丢失时间>=3，则销毁目标
	bool is_already_alarm;//是否已报警
	bool is_update;//当前帧是否跟新目标
};
typedef struct ViewInfo
{
	string view_name;//点位名字
	vector<TrkObject> trk_object_list;//该点位跟踪对象列表
	long long trk_id_counter;//该点位跟踪ID计数器
};

class TrackUtils
{
public:
	TrackUtils();
	//跟踪所有点位的目标
	void trackAllViews(Mat &image, string viewName, vector<ResultBox> &boxs, time_t now_time,
		time_t alert_time, bool &is_alarm);
	//跟踪对应点位的目标
	void trackBoxs(Mat &image, vector<ResultBox> &boxs,
		time_t alert_time, time_t now_time, ViewInfo &viewInfo, bool &is_alarm);
	//匹配两个box的位置
	float bbOverlap(const Rect& box1, const Rect& box2);
	bool compareBox(Rect bbox,Rect oldBbox);
	void printViewInfos();

private:
	TrkObject m_trkObject;
	vector<ViewInfo>  m_viewInfos;
};

#endif