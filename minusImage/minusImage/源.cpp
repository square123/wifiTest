
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "ViBe.h"

using namespace cv;
using namespace std;

//ͼ����� ��ת���ɻҶ�ȥ��� //�ǵ�ȡ����ֵ   Ӧ�ò��ԣ�ֱ�Ӽ�����֣�Ӧ���ǽ���ͼ����ɫ��ͬ��ɸѡ����
//Ӧ��Ҫ��һ���˲��Ĺ��� ͼ��Ҳ�ڲ��ϵĲ���
//���Խ�ͼ��ֳ�RGB��ͨ���ֱ�����о��½����
//Ӧ���и���ֵ�Ĵ���

//��������Ч������֪�������䱳��������Ҫ���������Ч��Ҫ�á�

//6�µ׽�����д�꣬Ͷһƪ���ĵ�����
//��Hɫ�������ȥ����ȥ����Ӱ�����ڵ�˼·�ǲ�����ͨ��������޳�һ��������Ȼ�������е���ȡH���������Ƿ��ܷ��ü��ķ����������Ӱ�ļ���
//��ͨ�����ֱ����һ���������������ͣ��Ƿ���Լ���һ����ͨ�����ľ��룬�Ӷ�����ͨ����������ӣ������������ʵ�������Ժ��Բ��ơ�
//��overlap������ӻ�����ƣ����³����

int main()
{
	//������Ƶ
	VideoCapture capture;
	capture.open("bodyvideoJH.avi");
	//ԭʼͼ��
	Mat ori=imread("ori.png",0);//��ԭʼͼƬת����YCRCB��ɫ�ռ��µģ�����������ɫ����������,���շ��ֺͻҶȴ����Ч����һ���ģ��ҶȵĹ�ʽ��RGB�Ĺ�ʽ��ͬ��

	Mat ori1=imread("ori.png");//��ԭʼͼƬת����YCRCB��ɫ�ռ��µģ�����������ɫ����������,���շ��ֺͻҶȴ����Ч����һ���ģ��ҶȵĹ�ʽ��RGB�Ĺ�ʽ��ͬ��
 	Mat oriT,oriTed;//�����ɫ�ľ���͸�˹��ľ���
	GaussianBlur(ori1,oriT,Size(5,5),0,0);
	cvtColor(oriT,oriTed,CV_BGR2HLS_FULL);
	vector<Mat> oriTedSplit;
	split(oriTed,oriTedSplit);//��ȡɫ��������ȥ����ӰЧ��
	

 	Mat oried;
	GaussianBlur(ori,oried,Size(5,5),0,0);

	//cv::BackgroundSubtractorGMG bgSubtractor; 

	//vibe����   ����vibe�㷨�������ɫ���г̶Ƚϸߣ����ڴ�����и���ʱ��ͼ�����ֽϴ����ʧ��
	Mat  mask;
	ViBe_BGS Vibe_Bgs;
	int count = 0;

	while (1)//������
	{	
		//ɫ�������ٴβ��� ����һ��
		imshow("hsl",oriTedSplit.at(0));
		Mat framedTed,framedTeded;//��ȡ�任��ľ����Լ���˹��ľ���
		vector<Mat> framedTedSplit;//�����Ҫ�洢�ľ���

		////��ȡ�ѱ�������ݲ���
		Mat framed;
		Mat diframe;
		Mat gray;
		Mat grayed;
		//Mat matCrCb;
		Mat dograyed;
		capture>>framed;
		if (framed.empty())
		{
			break;
		}
		//cvtColor(framed,matCrCb,CV_BGR2YCrCb);
		//imshow("mat",matCrCb);

		//ɫ������
		GaussianBlur(framed,framedTed,Size(5,5),0,0);
		cvtColor(framedTed,framedTeded,CV_BGR2HLS_FULL);
		split(framedTeded,framedTedSplit);
		imshow("11",framedTedSplit.at(0));
		Mat res;
		absdiff(framedTedSplit.at(0),oriTedSplit.at(0),res);
		imshow("rrr",res);


		cvtColor(framed,gray,CV_BGR2GRAY);
		GaussianBlur(gray,grayed,Size(5,5),0,0);
/*		vector<Mat> chan1,chan2;*/
// 		split(matCrCb,chan1);
// 		split(oried,chan2);
// 		Mat mer1,mer2;
// 		Mat mer10=chan1.at(1),mer11=chan1.at(2);
// 		Mat mer20=chan2.at(1),mer21=chan2.at(2);
// 		addWeighted(mer10,0.5,mer11,0.5,0.0,mer1);
// 		addWeighted(mer20,0.5,mer21,0.5,0.0,mer2);
		absdiff(oried,grayed,diframe);
		imshow("��",diframe);
		threshold(diframe,dograyed,30,255,THRESH_BINARY);
		imshow("����Ƶ",dograyed);
		
		//openCV subtractor
// 		Mat imask;
// 		bgSubtractor(framed,imask);
// 		imshow("gmm2",imask);

		////vibe����
		count++;
		Mat masked;
		if (count == 1)
		{
			Vibe_Bgs.init(oried);
			Vibe_Bgs.processFirstFrame(oried);
			cout<<"Init complete!"<<endl;
			
		}
		else
		{
			Vibe_Bgs.testAndUpdate(grayed);
			mask = Vibe_Bgs.getMask();

			//vibe ��ʾ����
// 			fillHole(mask,masked);
// 			erode(masked,masked,element);
// 			dilate(masked,masked,element);
// 			dilate(masked,masked,element);
// 			dilate(masked,masked,element);
			imshow("mask", mask);
			cout<<count<<endl;
		}

		waitKey(100);
	}
}