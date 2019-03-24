#ifndef TRACK_UTILS
#define TRACK_UTILS

#include<opencv2\opencv.hpp>
#include"typeDefine.h"
using namespace cv;
using namespace std;

typedef struct TrkObject
{
	string id;//ID = ��λ����+Ψһ����
	string class_name;//�������
	float score;//���Ŷ�
	Rect bbox;//���һ֡�����bbox
	vector<Rect> bbox_history;//��չ����������Ŀ�����ʷ��¼����ǰû���õ�
	time_t start_time;//��ʼ����ʱ��
	time_t continue_time;//��������ʱ��
	int lost_time;//��ʧʱ��,����ʧʱ��>=3��������Ŀ��
	bool is_already_alarm;//�Ƿ��ѱ���
	bool is_update;//��ǰ֡�Ƿ����Ŀ��
};
typedef struct ViewInfo
{
	string view_name;//��λ����
	vector<TrkObject> trk_object_list;//�õ�λ���ٶ����б�
	long long trk_id_counter;//�õ�λ����ID������
};

class TrackUtils
{
public:
	TrackUtils();
	//�������е�λ��Ŀ��
	void trackAllViews(Mat &image, string viewName, vector<ResultBox> &boxs, time_t now_time,
		time_t alert_time, bool &is_alarm);
	//���ٶ�Ӧ��λ��Ŀ��
	void trackBoxs(Mat &image, vector<ResultBox> &boxs,
		time_t alert_time, time_t now_time, ViewInfo &viewInfo, bool &is_alarm);
	//ƥ������box��λ��
	float bbOverlap(const Rect& box1, const Rect& box2);
	bool compareBox(Rect bbox,Rect oldBbox);
	void printViewInfos();

private:
	TrkObject m_trkObject;
	vector<ViewInfo>  m_viewInfos;
};

#endif