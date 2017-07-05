
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "ViBe.h"
#include "minusImage.h"
#include "overlapPro.h"


using namespace cv;
using namespace std;


//�Ƿ���Լ���һ����ͨ�����ľ��룬�Ӷ�����ͨ����������ӣ������������ʵ�������Ժ��Բ��ơ�

//�ȽϷ��꣬�Ͱ�����֫�⿪�������Լ��ǿ��Եġ� �������һ���򵥳����±Ƚ��ȶ��Ķ�Ŀ�����

//7 ����ڵ�����Ҫ�˷� ���ǲ��öԴ�����ͨ���������ķ�����������ָ�   

//3 ��ԭ����overlap����л��壬��ƺ�һ���ɵ��������û�����ƿ��Գɹ�����   overlapҪ�ṩ��Ӧ����һ�������ı�ţ������ܹ��˷�һ���Ķ�ʧ����
//2 ����openCV������ɫֱ��ͼ�����ݣ�Ȼ��ͨ�����ֿ飬Ҫ�����ݰ����������㣬����һ��������ֵ��¼��������������Ҫ����ģ����磬��С�󣬷ָ�����ͨ���򣬲���Ӧ���ܻ�ȡ�������ϵ
   //Ȼ��ѡȡ����ͨ�����µ�����Χ���򣬶�ͼ�����ȥɫ�������������Ҫ����дһ������ ������ͨ����

//8 ���պ������Ӳ�ɫ����ͷ���÷�

int main()
{
	//������Ƶ
	VideoCapture capture;
	capture.open("bodyvideoMan.avi");
	//ԭʼͼ��
	Mat ori=imread("ori.png");//��ԭʼͼƬת����YCRCB��ɫ�ռ��µģ�����������ɫ����������,���շ��ֺͻҶȴ����Ч����һ���ģ��ҶȵĹ�ʽ��RGB�Ĺ�ʽ��ͬ��
	//vibe����   
	ViBeHue Vibe(ori);//�����ʼͼ��

	Mat oriGauss,oriCovert;
	vector<Mat> oriSplit;
	GaussianBlur(ori,oriGauss,Size(3,3),0,0);
	cvtColor(oriGauss,oriCovert,CV_BGR2HLS_FULL);
	split(oriCovert,oriSplit);


	overlapPro olr;
	olr.outfile.open("cameradata.csv",fstream::app);

	//minusImg����
		//minusImage minusClass(ori);

	while (1)//������
	{	

		Mat framed,grayed;
		capture>>framed;
		if (framed.empty())
		{
			break;
		}
		//minus����
//  		Mat showMinusMat;
//  		minusClass(framed,showMinusMat);

	
		//vibeHue����
		Vibe(framed,grayed);
		imshow("shu",grayed);
		olr.process(grayed,framed,oriSplit.at(0));//������ͨ�����overlap����

		waitKey(10);
	}
}