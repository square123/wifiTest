#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;


void fillHole(const Mat srcBw, Mat &dstBw);//�ο����ϵ���亯����������


//ѡ����ʷ�Χ����ͨ����
void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);


//��������ͨ������͹���ϲ�
void hullArea(Mat &src,Mat &dst);


void deNoise(Mat &src,Mat &dst);//�ú��������� Ҫ�޸�
