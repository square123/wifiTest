#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include "kalmanEasy.h" //��Ҫʹ��kalman�˲�����ΪҪ���ǵ��ǳ�������˲�������ɾ���˲���
#define screenFilePath "E://projectTest//"
using namespace cv;
using namespace std;

string screeenShot(Mat &src, time_t t)//��ʱ���������ļ���
{
	string savePathTemp=screenFilePath;
	stringstream sstr2;
	sstr2<<t;
	string str2;
	sstr2>>str2;
	savePathTemp=savePathTemp+str2+ ".png";
	imwrite(savePathTemp,src);
	return savePathTemp;
}

//Ϊ�˼�� ֮������귵������
//�������䲿��
// void onMouse(int event, int x, int y, int flags, void* param)  
// {  
// 	Mat *im = reinterpret_cast<Mat*>(param);  
// 	switch (event)  
// 	{  
// 	case CV_EVENT_LBUTTONDOWN:     //������������Ӧ����������ͻҶ�  
// 		std::cout<<x<<","<<y<<" "<<static_cast<int>(im->at<short>(cv::Point(x,y)))<<std::endl;  
// 		break;            
// 	}  
// }  
// 
// int areaMatOri(Mat &src)//���ؾ�������,Ҫ���������ݸ�ʽΪUC1
// {
// 	int areaSum=0;
// 	for(int i=0;i<src.rows;i++)
// 	{
// 		uchar* data= src.ptr<uchar>(i);
// 		for(int j=0;j<src.cols;j++){
// 			uchar a=data[j];
// 			areaSum=areaSum+int(a);
// 		}
// 	}
// 	return areaSum=areaSum/255;
// }
// 
// int sumMat( Mat &src)//��ͣ������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�
// {
// 	Mat srcTemp;
// 	src.convertTo(srcTemp,CV_32SC1);//ע��int�Ͷ�Ӧ����CV_32SC1 ����ڿ�����������һ������ת���Ĺ���
// 	int matSumNum=0;
// 	for(int i=0;i<srcTemp.rows;i++)
// 	{
// 		int* data= srcTemp.ptr<int>(i);
// 		for(int j=0;j<srcTemp.cols;j++){
// 			int a=data[j];
// 			matSumNum=matSumNum+a;
// 		}
// 	}
// 	return matSumNum;
// }
// 
// int avgMat(Mat &src,int num)//��ƽ���������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�,��ʵ�õĸ����ԭ���Ǹ����������
// {
// 	int hh,mm;
// 	int h;
// 	h=num;
// 	hh=sumMat(src);
// 	if(h!=0)//���ڳ���һ��Ҫ���ǳ���Ϊ������
// 	{
// 		mm=hh/h;
// 		return mm;
// 	}
// 	else
// 	{
// 		return 0;
// 	}
// }

short medianOut(Mat&src,Point &tl,Point &br) //����������겻Ҫ��������ķ�Χ��ע��ѡȡ�ķ�Χ��Ҫ���󣬲�Ȼ�ٶȻ����
{
	vector<short> tempVec;
	for(int i=tl.y;i<br.y;i++)//�ȰѾ����з����Ԫ�ش洢����
	{
		short* data= src.ptr<short>(i);
		for(int j=tl.x;j<br.x;j++){
			short a=data[j];
			if (a!=0)
			{
				tempVec.push_back(a);
			}
		}
	}
	sort(tempVec.begin(),tempVec.end()); //������
	return tempVec[tempVec.size()/2]; // �����
}

int main()
{
	/*kalmanEasy kal;*/
	//���䲿��
	/*Mat buchong=imread("1.png",2);*/
	ofstream outfile("zhKal.txt",ios::app);
	char filename[10000];
	const int NUM = 5000;
	Mat frame;
	bool screenFlag=true;//���ձ�־λ
	time_t t1, t2, tTemp;
	time(&t1);
	tTemp=t1;
	string connectImageStr;
	string str2;
	//ֻ��¼һ��С���ڵ����ֵ
/*	Mat plot=Mat::zeros(400,1000,CV_8UC3);//�½����� �����滭��*/
	//ѭ��ͼ��
	for ( int i = 0; i < NUM; i++ )
	{
		sprintf_s( filename, "E://7��14����Ŀ���//��̨��Ƶ����//1//depth_%05d.PNG", i );
		frame = imread( filename, 2 );
		imshow( "video", frame );
		//outfile<<avgMat(deFrame,areaMatOri(deWindow))<<endl;
		//int zhi=avgMat(deFrame,areaMatOri(deWindow));
		int zhi=medianOut(frame,Point(157,328),Point(268,349));
		cout<<zhi<<endl;
		if(zhi<2500)//������������Ĳ��� ����������Ӧ����ֵ��ͻȻ�ļ�С ����һ�����ҵı仯����ʱ��ʼ��ʱ�������д���
		{
			if (screenFlag==true)
			{
				screenFlag=false;
				time(&t1);
				stringstream sstr2;
				sstr2<<t1;
				sstr2>>str2;
				//��ͼ���� ֻ����һ��
				connectImageStr=screeenShot(frame,t1);//����ͼƬͬʱ��·������	
			}
		}
		else//�����뿪������Ĳ���
		{
			screenFlag=true;//���½���־��Ϊtrue
			time(&t2);//��ȡ���ڵ�ʱ��
			if((t1!=tTemp)&&((t2-t1)<8))//�����˽�������ʱ��϶̵Ĳ���
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str charת����str
				remove(string(string(screenFilePath)+"quan//"+str2+".png").c_str());
			}
			else//��ʱ���㹻��ʱ�����ɹ����Ĳ���
			{

			}		
		}
		//setMouseCallback("video",onMouse,&frame); 
		/*tempOut=zhiPoint.x;*/
// 		if (i%5==1)
// 		{
// 			line(plot,tempPoint,Point((i/5),(400-zhiPoint.x/10)),Scalar(255,255,255),1,8);
// 			tempPoint=Point((i/5),(400-zhiPoint.x/10));
// 		}
// 		//circle(plot,Point((i),(400-zhi/10)),1,Scalar(255,255,255),-1);
// 		imshow("plot",plot);
		if ( waitKey(50) == 27 )
		{
			break;
		}
	}

	//�ƽ������Ȩֵ�ֲ�
	//�������䲿��
// 	while (1)
// 	{
// 		imshow("xx",buchong);
// 		setMouseCallback("xx",onMouse,&buchong); 
// 		waitKey(3);
// 	}

	outfile.close();
	return 0;
}