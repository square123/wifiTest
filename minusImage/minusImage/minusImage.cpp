#include "minusImage.h"

//初始化函数
minusImage::minusImage(Mat &src)
{
	src.copyTo(oriImg);
	GaussianBlur(oriImg,oriImgGauss,Size(3,3),0,0);
	cvtColor(oriImgGauss,oriImgCovert,CV_BGR2HLS_FULL);
	cvtColor(oriImgGauss,oriImgGray,CV_RGB2GRAY);
	split(oriImgCovert,orisplit);
}

//析构函数
minusImage::~minusImage()
{
}

//函数运算符重载
void minusImage::operator()(Mat &image, Mat &fgmask)
{
	//定义矩阵
	Mat ImgGauss; //高斯滤波后的图片
	Mat ImgGray;//灰度转换后图片
	Mat ImgCovert; //色域转换后的图片
	vector<Mat> imgSplit;//需要分离的hls区域
	Mat diffGray,diffHue;//差值
	Mat dwMatRaw;//将小的数据擦掉的矩阵
	Mat dwOstuMat; //大津法处理后的矩阵

	//获得基本的矩阵
	GaussianBlur(image,ImgGauss,Size(3,3),0,0);
	cvtColor(ImgGauss,ImgCovert,CV_BGR2HLS_FULL);
	split(ImgCovert,imgSplit);
	cvtColor(ImgGauss,ImgGray,CV_RGB2GRAY);

	//step1 在gray中做差,得到亮度下二值化的图像。
	absdiff(ImgGray,oriImgGray,diffGray);
	threshold(diffGray,dwMatRaw,5,255,THRESH_TOZERO);//将阈值以下的数据抹平
	threshold(dwMatRaw,dwOstuMat,0,255,THRESH_OTSU);//ostu法
	imshow("step1",dwOstuMat);

	//step2 在hue中做差
	Mat hueSmooth;
	absdiff(imgSplit.at(0),orisplit.at(0),diffHue);
	
	threshold(diffHue,hueSmooth,30,255,THRESH_BINARY);
	imshow("step2",diffHue);

	Mat zong,ozong;
	addWeighted(hueSmooth&dwOstuMat,0.2,dwOstuMat&diffGray,0.8,0,zong);
	threshold(zong,ozong,0,255,THRESH_OTSU);
	imshow("z",ozong);
}


void minusImage::fillHole(const Mat srcBw, Mat &dstBw)//参考网上的填充函数，很巧妙
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
void minusImage::selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
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
void minusImage::hullArea(Mat &src,Mat &dst) 
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