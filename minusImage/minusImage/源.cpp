
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "ViBe.h"

using namespace cv;
using namespace std;

//图像相减 先转换成灰度去相减 //记得取绝对值   应该不对，直接减很奇怪，应该是将与图像颜色不同的筛选出来
//应该要有一个滤波的过程 图像也在不断的颤抖
//可以将图像分成RGB三通道分别相减感觉下结果。
//应该有个阈值的处理。

//背景减除效果当已知背景后，其背景减除法要比其他检测效果要好。

//6月底将中文写完，投一篇中文的文献
//将H色调分离出去可以去除阴影，现在的思路是采用先通过做差法先剔除一部分区域，然后将区域中的提取H分量，看是否能否用减的方法来完成阴影的减除
//连通区域就直接用一个长的因子来膨胀，是否可以计算一个连通区域间的距离，从而将连通区域进行连接，这个计算量其实几乎可以忽略不计。
//在overlap的中添加缓冲机制，提高鲁棒性

int main()
{
	//测试视频
	VideoCapture capture;
	capture.open("bodyvideoJH.avi");
	//原始图像
	Mat ori=imread("ori.png",0);//将原始图片转换成YCRCB颜色空间下的，单独测试下色调分离的情况,最终发现和灰度处理的效果是一样的，灰度的公式和RGB的公式相同。

	Mat ori1=imread("ori.png");//将原始图片转换成YCRCB颜色空间下的，单独测试下色调分离的情况,最终发现和灰度处理的效果是一样的，灰度的公式和RGB的公式相同。
 	Mat oriT,oriTed;//定义彩色的矩阵和高斯后的矩阵
	GaussianBlur(ori1,oriT,Size(5,5),0,0);
	cvtColor(oriT,oriTed,CV_BGR2HLS_FULL);
	vector<Mat> oriTedSplit;
	split(oriTed,oriTedSplit);//提取色调，尝试去除阴影效果
	

 	Mat oried;
	GaussianBlur(ori,oried,Size(5,5),0,0);

	//cv::BackgroundSubtractorGMG bgSubtractor; 

	//vibe部分   利用vibe算法，其对颜色敏感程度较高，但在处理进行跟踪时，图像会出现较大的损失。
	Mat  mask;
	ViBe_BGS Vibe_Bgs;
	int count = 0;

	while (1)//处理部分
	{	
		//色调分离再次测试 再试一次
		imshow("hsl",oriTedSplit.at(0));
		Mat framedTed,framedTeded;//获取变换后的矩阵以及高斯后的矩阵
		vector<Mat> framedTedSplit;//分离后要存储的矩阵

		////获取已保存的数据部分
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

		//色调分离
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
		imshow("差",diframe);
		threshold(diframe,dograyed,30,255,THRESH_BINARY);
		imshow("读视频",dograyed);
		
		//openCV subtractor
// 		Mat imask;
// 		bgSubtractor(framed,imask);
// 		imshow("gmm2",imask);

		////vibe部分
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

			//vibe 显示部分
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