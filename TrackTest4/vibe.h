#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#define NUM_SAMPLES 20		//每个像素点的样本个数
#define MIN_MATCHES 2		//#min指数
#define RADIUS 20		//Sqthere半径
#define SUBSAMPLE_FACTOR 16	//子采样概率


class ViBe //vibe速度感觉比gmm收敛要快些 去掉了HUE 没啥用 对于输入图片尺寸
{
public:
	ViBe(Mat &src);//输入图像的构造函数 要求输入的矩阵必须是彩色图像
	~ViBe(void);
	void init(const Mat& _image);   //初始化 //输入没有人的背景照片
	Mat getMask(void) { return m_mask; };
	void operator()(Mat &image, Mat &fgmask);//更新函数
	
	Mat oriImg;//背景图片
	Mat oriImgGray;//灰度转换后图片

private:
	Mat m_samples;
	Mat m_foregroundMatchCount;
	Mat m_mask;
	void processFirstFrame(const Mat& _image);
	void testAndUpdate(const Mat& _image);  //更新
};