#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#define NUM_SAMPLES 20		//ÿ�����ص����������
#define MIN_MATCHES 2		//#minָ��
#define RADIUS 20		//Sqthere�뾶
#define SUBSAMPLE_FACTOR 16	//�Ӳ�������


class ViBe //vibe�ٶȸо���gmm����Ҫ��Щ ȥ����HUE ûɶ�� ��������ͼƬ�ߴ�
{
public:
	ViBe(Mat &src);//����ͼ��Ĺ��캯�� Ҫ������ľ�������ǲ�ɫͼ��
	~ViBe(void);
	void init(const Mat& _image);   //��ʼ�� //����û���˵ı�����Ƭ
	Mat getMask(void) { return m_mask; };
	void operator()(Mat &image, Mat &fgmask);//���º���
	
	Mat oriImg;//����ͼƬ
	Mat oriImgGray;//�Ҷ�ת����ͼƬ

private:
	Mat m_samples;
	Mat m_foregroundMatchCount;
	Mat m_mask;
	void processFirstFrame(const Mat& _image);
	void testAndUpdate(const Mat& _image);  //����
};