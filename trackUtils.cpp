#include"trackUtils.h"

TrackUtils::TrackUtils()
{
	m_viewInfos.clear();
}

void TrackUtils::trackAllViews(Mat &image, string viewName, vector<ResultBox> &boxs, time_t now_time,
	time_t alert_time, bool &is_alarm)
{
	vector<ViewInfo>::iterator iter_view;
	static int count = 0;
#ifndef NDEBUG
	cout << "count ------------------" << count++ << endl;
	cout << "alert_time: " << alert_time << endl;
#endif
	//ƥ���Ӧ�ĵ�λ
	for (iter_view = m_viewInfos.begin(); iter_view != m_viewInfos.end(); iter_view++)
	{
		if (iter_view->view_name == viewName)
		{
			trackBoxs(image, boxs, alert_time, now_time, *iter_view, is_alarm);
			return;
		}
	}
	//���õ�λû�ж�Ӧ������������β����µĵ�λ����
	ViewInfo viewInfo;
	viewInfo.view_name = viewName;
	viewInfo.trk_object_list.clear();
	viewInfo.trk_id_counter = 0;
	vector<ResultBox>::iterator iter_bbox;
#ifndef NDEBUG
	cout << "boxs.size" << boxs.size() << endl;
#endif
	for (iter_bbox = boxs.begin(); iter_bbox != boxs.end(); iter_bbox++)
	{
		TrkObject trkObject;
		char _id[10];
		sprintf(_id, "%d", viewInfo.trk_id_counter);
		string id = _id;
		trkObject.id = viewInfo.view_name + ":" + id;
		trkObject.class_name = iter_bbox->className;
		trkObject.score = iter_bbox->score;
		trkObject.bbox = *iter_bbox;
		trkObject.start_time = now_time;
		trkObject.continue_time = 0;
		trkObject.lost_time = 0;
		trkObject.is_already_alarm = false;
		trkObject.is_update = true;
		viewInfo.trk_object_list.push_back(trkObject);
		viewInfo.trk_id_counter++;
	}
	m_viewInfos.push_back(viewInfo);
	//��ʾ�¼ӽ������Ŀ��
	vector<TrkObject>::iterator iter_trk;
	for (iter_trk = viewInfo.trk_object_list.begin(); iter_trk != viewInfo.trk_object_list.end(); iter_trk++)
	{
		rectangle(image, iter_trk->bbox, Scalar(0, 255, 255), 2);
	}
}

void TrackUtils::trackBoxs(Mat &image, vector<ResultBox> &boxs, time_t alert_time, time_t now_time, ViewInfo &viewInfo, bool &is_alarm)
{
	//��ʼ�����ٶ���ĸ��±�־λ
	vector<TrkObject>::iterator iter_trk;
	for (iter_trk = viewInfo.trk_object_list.begin(); iter_trk != viewInfo.trk_object_list.end(); iter_trk++)
	{
		iter_trk->is_update = false;
	}

	vector<ResultBox>::iterator iter_box;
	//������ǰ֡�����
	for (iter_box = boxs.begin(); iter_box != boxs.end(); iter_box++)
	{
		bool is_exist = false;
		vector<TrkObject>::iterator iter_trk;
		//�������ٶ����б����ҵ�ƥ�������¸��ٲ���
		for (iter_trk = viewInfo.trk_object_list.begin(); iter_trk != viewInfo.trk_object_list.end(); iter_trk++)
		{
			if (bbOverlap(*iter_box, (*iter_trk).bbox) > 0.5 && iter_trk->class_name == iter_box->className)
			{
				is_exist = true;
				//�������еı�����Ŀ��
				iter_trk->score = iter_box->score;
				iter_trk->bbox = *iter_box;
				iter_trk->lost_time = 0;
				iter_trk->continue_time = now_time - iter_trk->start_time;
				iter_trk->is_update = true;
			}
		}

		//���Ҳ���ƥ������½����ٶ���
		if (!is_exist)
		{
			TrkObject new_trk_object;
			char _id[10];
			sprintf(_id, "%d", viewInfo.trk_id_counter);
			string id = _id;
			new_trk_object.id = viewInfo.view_name + ":" + id;
			new_trk_object.class_name = iter_box->className;
			new_trk_object.score = iter_box->score;
			new_trk_object.bbox = *iter_box;
			new_trk_object.start_time = now_time;
			new_trk_object.continue_time = 0;
			new_trk_object.lost_time = 0;
			new_trk_object.is_already_alarm = false;
			//new_trk_object.is_first_alarm = false;
			new_trk_object.is_update = true;
			viewInfo.trk_object_list.push_back(new_trk_object);
			viewInfo.trk_id_counter++;
		}
	}

#ifndef NDEBUG
	printViewInfos();
#endif

	
	for (iter_trk = viewInfo.trk_object_list.begin(); iter_trk != viewInfo.trk_object_list.end();)
	{
		//������Ŀ��֮ǰ��δ�й����������ҳ���ʱ�䳬������ʱ�䣬�򱨾�
		if (!iter_trk->is_already_alarm && iter_trk->continue_time >= alert_time)
		{
			iter_trk->is_already_alarm = true;
			rectangle(image, iter_trk->bbox, Scalar(0, 0, 255), 7);
			char strTemp[100];
			sprintf(strTemp, "%s : %.3f", iter_trk->class_name, iter_trk->score);
			putText(image, strTemp, Point(max(min((int)iter_trk->bbox.x + 20, image.cols - 1), 20), max(min((int)iter_trk->bbox.y + 20, image.rows - 1), 20)), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0)/*colortable[i]*/, 1, 2);
			is_alarm = true;
		}
		else
		{
			rectangle(image, iter_trk->bbox, Scalar(0, 255, 255), 2);
			char strTemp[100];
			sprintf(strTemp, "%s : %.3f", iter_trk->class_name, iter_trk->score);
			putText(image, strTemp, Point(max(min((int)iter_trk->bbox.x + 20, image.cols - 1), 20), max(min((int)iter_trk->bbox.y + 20, image.rows - 1), 20)), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 0)/*colortable[i]*/, 1, 2);
		}

		////����Ŀ�껭�������Ŀ�껭�ƿ�
		//if (iter_trk->is_first_alarm)
		//{
		//	iter_trk->is_first_alarm = false;
		//	iter_trk->is_already_alarm = true;
		//	rectangle(image, iter_trk->bbox, Scalar(0, 0, 255), 1);
		//	is_alarm = true;
		//}
		//else
		//{
		//	rectangle(image, iter_trk->bbox, Scalar(0, 255, 255), 1);
		//}

		//�����и��±�־λΪfalse�ĸ��ٶ���lost_time��1
		if (!iter_trk->is_update)
			iter_trk->lost_time++;

		//����ʧʱ��>=2,�����ٸ��ٶ���
		if (iter_trk->lost_time >= 2)
		{
			iter_trk = viewInfo.trk_object_list.erase(iter_trk);
			continue;
		}
		iter_trk++;
	}

}

float TrackUtils::bbOverlap(const Rect& box1, const Rect& box2)
{
	if (box1.x > box2.x + box2.width) { return 0.0; }
	if (box1.y > box2.y + box2.height) { return 0.0; }
	if (box1.x + box1.width < box2.x) { return 0.0; }
	if (box1.y + box1.height < box2.y) { return 0.0; }

	float colInt = min(box1.x + box1.width, box2.x + box2.width) - max(box1.x, box2.x);
	float rowInt = min(box1.y + box1.height, box2.y + box2.height) - max(box1.y, box2.y);

	float intersection = colInt * rowInt;
	float area1 = box1.width*box1.height;
	float area2 = box2.width*box2.height;
	return intersection / (area1 + area2 - intersection);
}

bool TrackUtils::compareBox(Rect bbox, Rect oldBbox)
{
	float bbox_center_x = bbox.x + bbox.width / 2;
	float bbox_center_y = bbox.y + bbox.height / 2;
	float oldBbox_center_x = oldBbox.x + oldBbox.width / 2;
	float oldBbox_center_y = oldBbox.y + oldBbox.height / 2;

	if (abs(bbox_center_x - oldBbox_center_x) < 20 && abs(bbox_center_y - oldBbox_center_y) < 20
		&& abs(bbox.width - oldBbox.width) < 30 && abs(bbox.height - oldBbox.height) < 30)
		return true;
	else
		return false;
}

void TrackUtils::printViewInfos()
{
	vector<ViewInfo>::iterator iter;
	for (iter = m_viewInfos.begin(); iter != m_viewInfos.end(); iter++)
	{
		cout << "view_name :" << iter->view_name << endl;
		vector<TrkObject>::iterator iter_trk;
		cout << "trk_object_list_size:" << iter->trk_object_list.size() << endl;
		for (iter_trk = iter->trk_object_list.begin(); iter_trk != iter->trk_object_list.end(); iter_trk++)
		{
			Rect bbox;//���һ֡�����bbox
			vector<Rect> bbox_history;//��չ����������Ŀ�����ʷ��¼����ǰû���õ�
			time_t start_time;//��ʼ����ʱ��
			time_t continue_time;//��������ʱ��
			int lost_time;//��ʧʱ��,����ʧʱ��>=3��������Ŀ��
			bool is_already_alarm;//�Ƿ��ѱ���
			bool is_first_alarm;//�Ƿ��һ�α���
			bool is_update;//��ǰ֡�Ƿ����Ŀ��
			cout << "ID:" << iter_trk->id << endl;
			cout << "bbox:" << iter_trk->bbox.x << " " << iter_trk->bbox.y << " " << iter_trk->bbox.width << " " << iter_trk->bbox.height << endl;
			cout << "start_time:" << iter_trk->start_time << endl;
			cout << "continue_time:" << iter_trk->continue_time << endl;
			cout << "lost_time:" << iter_trk->lost_time << endl;
			cout << "is_already_alarm:" << iter_trk->is_already_alarm << endl;
			//cout << "is_first_alarm:" << iter_trk->is_first_alarm << endl;
			cout << "is_update:" << iter_trk->is_update << endl;
			cout << "--------------------------------------------------" << endl;
		}
		cout << "==============================================================" << endl;
	}
	
}