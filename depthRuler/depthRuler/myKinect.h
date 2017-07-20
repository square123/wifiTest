/**************************************************
 * \file myKinect.h
 * \date 2017/06/27 13:15
 * \author ���Ǹ���
 * Contact: kfhao123@163.com
 * \note �����ԭ����myKinect�಻�Ǻ����ƣ����Լ��¼ӵĳɷ�
 * ����infrared�ͼ�ⲿ�ֵĳ���
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
public://�洢һЩ�������ʹ�õ���
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
	Mat bodyIndexMat;//��ʾΪ��ɫͼ���bodyIndex
	Mat bodyIndexMatInt;//��ʾΪ��ֵ��bodyIndex
	Mat InfraredMat16;
	Mat InfraredMat8;
	Mat CoordinateMapperMat;//ƥ���ľ���
	Mat CoordinateMapperMat_TM;//��ģ��ĺ�����������ʾ���
	Mat TwoMat;//��ֵͼ�����
	Mat TwoMatTemp;
	Mat depthDeTemp;//���ͼ��ƽ��ȥ��ľ���
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT Initbody();
	HRESULT InitBodyIndex();
	HRESULT InitInfrared();
	void colorProcess();
	void depthProcess();//�÷����ڶ�ȡ16λʱ���ڴ���������,���õ������帳ֵ
	void depthProcess2();//�÷���û�����⣬���õ��Ƿֱ�ֵ
	void bodyIndexProcess();
	void InfraredProcess(float fGamma);
	void kincetSave(Mat a,string savePath,int opt);//������ģʽѡ�񣬱����ɫ�����
	void kinectSaveAll(string savePath);//ͬʱ�����ɫ�����
	float detPeopleDepth();//ͨ���ؽڵ����ж������Զ��������Ч�����ȶ������ǹؽڵ�̫�٣����ɺ����ж�
	int detPeopleDepth2();//ͨ����Ⱥ�bodyIndex���õ����

	int areaMatForSpecNum(Mat &src,int num);//���ؾ������ض�ֵ�����
	int areaMatOri(Mat &src);//���ؾ�������,Ҫ���������ݸ�ʽΪUC1
	void MyGammaCorrection(Mat& src, Mat& dst, float fGamma);
	int avgMat(Mat &src,int num);//��ƽ���������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�,��ʵ�õĸ����ԭ���Ǹ����������
	int sumMat( Mat &src);//��ͣ������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�

	Kinect();
	~Kinect();

private://�洢һЩʵ�ֵĹ�����
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
