#pragma once
#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

class kalmanEasy  //该类只能用于对匀速直线运动进行滤波，观测量是屏幕中的坐标位置，参数暂时都不知道怎么调，就是为了方便，是将openCV的kalman滤波器封装到这里
{
public:
	kalmanEasy();
	~kalmanEasy();
	Point filterOutput(Point &x);//输入是观测值，输出为滤波后的值

private:
	RNG rng;
	KalmanFilter KF;
	//1.kalman filter setup    值得注意的是，因为观测量是两个向量，所以要用的是矩阵的写法，而不是单独一个特征的写法
	static const int stateNum=4;                                      //状态值4×1向量(x,y,△x,△y) 其中△x,△y指的应该是速度，每次更新默认是一个单位
	static const int measureNum=2;                                    //测量值2×1向量(x,y)	
	int count; //由于前面的初始值设定的不太好，所以当为前几帧时，默认输出的就是真实值，当预测后其值为更新后的预测值
	Mat measurement;
};
