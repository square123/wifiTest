#include "otherFuncs.h"

Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
Mat elementL=getStructuringElement(MORPH_RECT,Size(1,5));

void fillHole(const Mat srcBw, Mat &dstBw)//参考网上的填充函数，很巧妙
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//延展图像
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));//非常巧妙，用了填充算法的反运算

	Mat cutImg;//裁剪延展的图像
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

//选择合适范围的连通区域
void selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
		for (int i = 0; i < contours.size(); i++)
		{
			if (maxNum>0?(contourArea(contours[i])>maxNum||contourArea(contours[i])<minNum):(contourArea(contours[i])<minNum))
			{
				drawContours(dst,contours,i,0,CV_FILLED);
			} 
			else
			{
				drawContours(dst,contours,i,255,CV_FILLED);
			}
		}
	}
}

//用来将连通区域用凸包合并
void hullArea(Mat &src,Mat &dst) 
{
	vector<vector<Point>> contours;  //通过该方法也是可以的，其矩阵充当了原来类似vector<Point>的效果，是双通道矩阵
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> hull(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		convexHull(Mat(contours[i]),hull[i],false);
	}
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,hull,i,(255),CV_FILLED);
		}
	}
}

void deNoise(Mat &src,Mat &dst)//该函数有问题 要修改
{
	//先用开运算处理,去除细小噪点
	Mat tempMat;

	erode(src,src,element);
	dilate(src,src,element);
	fillHole(src,tempMat);

	//dilate(tempMat,tempMat,elementL);
	Mat tempMat2;
	fillHole(tempMat,dst);
}