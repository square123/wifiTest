#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include "kalmanEasy.h" //不要使用kalman滤波，因为要考虑的是冲击量，滤波后将数据删除了不好
#define screenFilePath "E://projectTest//"
using namespace cv;
using namespace std;

string screeenShot(Mat &src, time_t t)//用时间来命名文件名
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

//为了简便 之间用鼠标返回坐标
//其他补充部分
// void onMouse(int event, int x, int y, int flags, void* param)  
// {  
// 	Mat *im = reinterpret_cast<Mat*>(param);  
// 	switch (event)  
// 	{  
// 	case CV_EVENT_LBUTTONDOWN:     //鼠标左键按下响应：返回坐标和灰度  
// 		std::cout<<x<<","<<y<<" "<<static_cast<int>(im->at<short>(cv::Point(x,y)))<<std::endl;  
// 		break;            
// 	}  
// }  
// 
// int areaMatOri(Mat &src)//返回矩阵的面积,要求输入数据格式为UC1
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
// int sumMat( Mat &src)//求和，必须是单通道的 注意这里数据格式是int型的
// {
// 	Mat srcTemp;
// 	src.convertTo(srcTemp,CV_32SC1);//注意int型对应的是CV_32SC1 这个在拷贝过程中有一个类型转换的过程
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
// int avgMat(Mat &src,int num)//求平均，必须是单通道的 注意这里数据格式是int型的,不实用的浮点的原因是浮点运算较慢
// {
// 	int hh,mm;
// 	int h;
// 	h=num;
// 	hh=sumMat(src);
// 	if(h!=0)//对于除法一定要考虑除数为零的情况
// 	{
// 		mm=hh/h;
// 		return mm;
// 	}
// 	else
// 	{
// 		return 0;
// 	}
// }

short medianOut(Mat&src,Point &tl,Point &br) //两个点的坐标不要超过矩阵的范围，注意选取的范围不要过大，不然速度会很慢
{
	vector<short> tempVec;
	for(int i=tl.y;i<br.y;i++)//先把矩阵中非零的元素存储起来
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
	sort(tempVec.begin(),tempVec.end()); //先排序
	return tempVec[tempVec.size()/2]; // 后输出
}

int main()
{
	/*kalmanEasy kal;*/
	//补充部分
	/*Mat buchong=imread("1.png",2);*/
	ofstream outfile("zhKal.txt",ios::app);
	char filename[10000];
	const int NUM = 5000;
	Mat frame;
	bool screenFlag=true;//拍照标志位
	time_t t1, t2, tTemp;
	time(&t1);
	tTemp=t1;
	string connectImageStr;
	string str2;
	//只记录一个小窗口的深度值
/*	Mat plot=Mat::zeros(400,1000,CV_8UC3);//新建画板 在上面画点*/
	//循环图像
	for ( int i = 0; i < NUM; i++ )
	{
		sprintf_s( filename, "E://7月14日项目相关//柜台视频拍摄//1//depth_%05d.PNG", i );
		frame = imread( filename, 2 );
		imshow( "video", frame );
		//outfile<<avgMat(deFrame,areaMatOri(deWindow))<<endl;
		//int zhi=avgMat(deFrame,areaMatOri(deWindow));
		int zhi=medianOut(frame,Point(157,328),Point(268,349));
		cout<<zhi<<endl;
		if(zhi<2500)//人物进入检测区的操作 进入检测区域应该是值会突然的减小 存在一个剧烈的变化，此时开始计时，并进行处理
		{
			if (screenFlag==true)
			{
				screenFlag=false;
				time(&t1);
				stringstream sstr2;
				sstr2<<t1;
				sstr2>>str2;
				//截图操作 只进行一次
				connectImageStr=screeenShot(frame,t1);//保存图片同时将路径返回	
			}
		}
		else//人物离开检测区的操作
		{
			screenFlag=true;//重新将标志设为true
			time(&t2);//获取现在的时间
			if((t1!=tTemp)&&((t2-t1)<8))//处理当人进入区域时间较短的操作
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str char转换成str
				remove(string(string(screenFilePath)+"quan//"+str2+".png").c_str());
			}
			else//当时间足够长时，即成功检测的操作
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

	//黄金比例的权值分布
	//其他补充部分
// 	while (1)
// 	{
// 		imshow("xx",buchong);
// 		setMouseCallback("xx",onMouse,&buchong); 
// 		waitKey(3);
// 	}

	outfile.close();
	return 0;
}