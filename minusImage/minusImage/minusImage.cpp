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
	//imshow("step1",dwOstuMat);

	//step2 在hue中做差
	Mat hueSmooth;
	absdiff(imgSplit.at(0),orisplit.at(0),diffHue);
	threshold(diffHue,hueSmooth,30,255,THRESH_BINARY);
	//imshow("step2",diffHue);

	Mat zong,ozong;
	addWeighted(hueSmooth&dwOstuMat,0.2,dwOstuMat&diffGray,0.8,0,zong);
	threshold(zong,ozong,0,255,THRESH_OTSU);
	imshow("z",ozong);
}


