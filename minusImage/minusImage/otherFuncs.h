#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;


void fillHole(const Mat srcBw, Mat &dstBw);//参考网上的填充函数，很巧妙


//选择合适范围的连通区域
void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);


//用来将连通区域用凸包合并
void hullArea(Mat &src,Mat &dst);


void deNoise(Mat &src,Mat &dst);//该函数有问题 要修改
