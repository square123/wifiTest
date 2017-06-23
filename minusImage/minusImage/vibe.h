#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include "otherFuncs.h"

using namespace cv;
using namespace std;

#define NUM_SAMPLES 20		//ÿ�����ص����������
#define MIN_MATCHES 2		//#minָ��
#define RADIUS 20		//Sqthere�뾶
#define SUBSAMPLE_FACTOR 16	//�Ӳ�������


class ViBeHue //vibe�ٶȸо���gmm����Ҫ��Щ
{
public:
	ViBeHue(Mat &src);//����ͼ��Ĺ��캯�� Ҫ������ľ�������ǲ�ɫͼ��

	~ViBeHue(void);

	void init(const Mat& _image);   //��ʼ�� //����û���˵ı�����Ƭ
	void processFirstFrame(const Mat& _image);
	void testAndUpdate(const Mat& _image);  //����
	Mat getMask(void){return m_mask;};
	//���º���
	void operator()(Mat &image, Mat &fgmask);

private:
	Mat m_samples;
	Mat m_foregroundMatchCount;
	Mat m_mask;

public:
	Mat oriImg;//����ͼƬ
	Mat oriImgGauss; //��˹�˲����ͼƬ
	Mat oriImgGray;//�Ҷ�ת����ͼƬ
	Mat oriImgCovert; //ɫ��ת�����ͼƬ
	vector<Mat> orisplit;//��Ҫ�����hls����

};