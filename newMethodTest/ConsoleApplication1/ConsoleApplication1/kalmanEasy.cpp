#include "kalmanEasy.h"

kalmanEasy::kalmanEasy()
{
	KF.init(stateNum, measureNum, 0);	
	KF.transitionMatrix = *(Mat_<float>(4, 4) <<1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1);  //转移矩阵A 运动状态方程，做跟踪就是这个
	setIdentity(KF.measurementMatrix);                                             //测量矩阵H
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));                            //系统噪声方差矩阵Q
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));                        //测量噪声方差矩阵R
	setIdentity(KF.errorCovPost, Scalar::all(1));                                  //后验错误估计协方差矩阵P
	rng.fill(KF.statePost,RNG::UNIFORM,0,0);   //初始状态值x(0)
	measurement = Mat::zeros(measureNum, 1, CV_32F);                           //初始测量值x'(0)，因为后面要更新这个值，所以必须先定义 类似中间变量
	count=0;//统计次数
}

kalmanEasy::~kalmanEasy()
{
}

Point kalmanEasy::filterOutput(Point &src) //输入是需要被滤波的，输出时要滤波的
{
	if (count>3)//前几帧输出的是非滤波后的跟踪
	{
		KF.predict();//需要有这个步骤 不然无法更新
		measurement.at<float>(0) = (float)src.x;
		measurement.at<float>(1) = (float)src.y;	
		Mat res=KF.correct(measurement);//此时更新后
		return Point(res.at<float>(0),res.at<float>(1));
	}else
	{
		count++;
		return src;
	}
}