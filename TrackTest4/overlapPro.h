#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <set>
#include<fstream>
#include <time.h>
#include "vibe.h"
#include <math.h>

#define count_num 20 //������ͨ���������Ŀ
#define max_num 20 //ͬһʱ����ͨ����������Ŀ
using namespace cv;



class overlapPro //�������ڽ���ͨ����ͨ��overlap����ɱ��
{
private:
	struct bwMix //������ͨ����Ľṹ��
	{
		Rect bwRect;//��������
		int markLabel;//���
		Mat bwEdge;//��Ե��Ϣ
		//Point center;//ͼ������� ��ʱ����Ҫ�������ģ���Ϊ���ǵ�����������ͬ�ֿ���ɢ�����ԾͲ����������ˣ������ϲ����ٿ�������
		time_t bwTime;//̽���ռ�����ʱ��
	};
	vector<bwMix> picBwMix;
	struct bwMixRawData
	{
		char Timestamp[14];
		Point center;
	}bwMixTimeRaw[60][count_num][max_num];//ʱ�� ��������ͨ���� ��ͬһʱ���ܲɼ����������
	int bwMixIndex[60][count_num];//�洢��������
	time_t processIndex;
public:
	overlapPro();
	~overlapPro();
	void fillHole(const Mat srcBw, Mat &dstBw);//�ο����ϵ���亯����������
	void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);//ѡ����ʷ�Χ����ͨ����
	void hullArea(Mat &src,Mat &dst);//��������ͨ������͹���ϲ�
	void getBwMix(Mat &src,vector<bwMix> &dst);
	void bwMixToColorMat(vector<bwMix> &src,Mat &dst);
	float rectOverlap(const Rect & preBox,const Rect & nowBox);
	void renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd);
	int charTimeGetSecond(char ttt[14]);//��õõ����ݵĺ���λ
	void bwMixToOrbit(vector<bwMix> &src);
	void process(Mat &src);//��GMM��ģ�ķ�����������������Ҫ��ɵ�ֻ��overlap����
	void timeRawProcess();//������Ϣѹ����1��
	Mat selectedMat;
	Mat hullMat;
	Mat showColor;
	vector<bwMix> prePicBwMix;//����������ڸ���
	//ofstream outfile;//���ڴ洢�켣���ݵ�
};