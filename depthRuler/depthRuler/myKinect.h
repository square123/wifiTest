/**************************************************
 * \file myKinect.h
 * \date 2017/06/27 13:15
 * \author 正那个方
 * Contact: kfhao123@163.com
 * \note 该类和原来的myKinect类不是很相似，有自己新加的成分
 * 比如infrared和检测部分的程序
**************************************************/
#ifndef MYKINECT_H
#define MYKINECT_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <kinect.h>
#include<fstream>
#include<math.h>

#include <time.h>

#define FilePath "H://testbody//"
#define saveDepth 1
#define saveColor 2
#define PI 3.1415926

//Infrared use
#define InfraredSourceValueMaximum static_cast<float>(USHRT_MAX)
#define InfraredOutputValueMinimum 0.01f 
#define InfraredOutputValueMaximum 1.0f
#define InfraredSceneValueAverage 0.08f
#define InfraredSceneStandardDeviations 3.0f
///////
using namespace cv;
using namespace std;

template<class Interface> 
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if ( pInterfaceToRelease != NULL )
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

class Kinect
{
public://存储一些程序可以使用的量
	static const int DepthWidth = 512;
	static const int DepthHeight = 424;
	static const int ColorWidth = 1920;
	static const int ColorHeight = 1080;
	char saveTmpStrDep[10000];
	int saveTmpIntDep;
	char saveTmpStrClr[10000];
	int saveTmpIntClr;
	char saveTmpStrBoI[10000];
	int saveTmpIntBoI;
	
	Mat depthMat8;
	Mat depthMat16;
	Mat colorMat;
	Mat colorHalfSizeMat;
	Mat bodyIndexMat;//显示为彩色图像的bodyIndex
	Mat bodyIndexMatInt;//显示为数值的bodyIndex
	Mat InfraredMat16;
	Mat InfraredMat8;
	Mat CoordinateMapperMat;//匹配后的矩阵
	Mat CoordinateMapperMat_TM;//掩模后的函数，用于显示结果
	Mat TwoMat;//二值图像矩阵
	Mat TwoMatTemp;
	Mat depthDeTemp;//深度图像平均去噪的矩阵
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT Initbody();
	HRESULT InitBodyIndex();
	HRESULT InitInfrared();
	void colorProcess();
	void depthProcess();//该方法在读取16位时，内存会出现问题,采用的是整体赋值
	void depthProcess2();//该方法没有问题，采用的是分别赋值
	void bodyIndexProcess();
	void InfraredProcess(float fGamma);
	void kincetSave(Mat a,string savePath,int opt);//有两种模式选择，保存彩色或深度
	void kinectSaveAll(string savePath);//同时保存彩色和深度
	float detPeopleDepth();//通过关节点来判断人物的远近，发现效果不稳定，考虑关节点太少，依旧很难判读
	int detPeopleDepth2();//通过深度和bodyIndex来得到深度

	int areaMatForSpecNum(Mat &src,int num);//返回矩阵中特定值的面积
	int areaMatOri(Mat &src);//返回矩阵的面积,要求输入数据格式为UC1
	void MyGammaCorrection(Mat& src, Mat& dst, float fGamma);
	int avgMat(Mat &src,int num);//求平均，必须是单通道的 注意这里数据格式是int型的,不实用的浮点的原因是浮点运算较慢
	int sumMat( Mat &src);//求和，必须是单通道的 注意这里数据格式是int型的

	Kinect();
	~Kinect();

private://存储一些实现的过程量
	IKinectSensor* pSensor ;

	IColorFrameSource* pColorSource;
	IColorFrameReader* pColorReader;
	IFrameDescription* pColorDescription;
	IColorFrame* pColorFrame;

	IDepthFrameSource* pDepthSource;
	IDepthFrameReader* pDepthReader;
	IFrameDescription* pDepthDescription;
	IDepthFrame* pDepthFrame;

	IBodyIndexFrameSource* pBodyIndexSource;
	IBodyIndexFrameReader* pBodyIndexReader;
	IBodyIndexFrame* pBodyIndexFrame;

	IInfraredFrameSource* pInfraredSource;
	IInfraredFrameReader* pInfraredReader;
	IInfraredFrame* pInfraredFrame;

	IBodyFrameSource* pBodySource;
	IBodyFrameReader* pBodyReader;
	IBodyFrame* pBodyFrame;
	ICoordinateMapper* pCoordinateMapper;
};
#endif
