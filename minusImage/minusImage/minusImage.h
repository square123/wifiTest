#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

//学会写继承后自己定义一个大的图像类，以后的程序都继承该类

//minusImage方法  无法使用KCF的方法是由于在识别人物时采用的是提前圈定的方法，效果并不理想，因此需要以检测为主，然后从其他角度克服遮挡的问题（还是应该将颜色算进去）
//将H色调分离出去可以去除阴影，现在的思路是采用先通过做差法先剔除一部分区域，然后将区域中的提取H分量，看是否能否用减的方法来完成阴影的减除,因为肤色的影响，应该是缩小包围圈的思路
//算法流程
//进行高斯滤波(3*3)速度较快，有去噪的作用
//分离出灰度图像，利用灰度图像做差，进行ostu自适应分割
//对分割出的图像域提取H分量，得到一个更小的外接区域，而不是直接做与运算，从而剔除阴影

//觉得vibe算法速度要快一些，在某些情况下更能更加饱满，因此，决定采用双保险，选取减除背景和vibe中最小的区域，再选取统计区域中最饱满的值，哪个值越饱满，就选取哪个与hue进行处理，从而去除阴影的效果。



class minusImage
{
public:
	minusImage(Mat &src);
	~minusImage();
	void operator()(Mat &image, Mat &fgmask);//处理程序
	

	Mat oriImg;//背景图片
	Mat oriImgGauss; //高斯滤波后的图片
	Mat oriImgGray;//灰度转换后图片
	Mat oriImgCovert; //色域转换后的图片
	vector<Mat> orisplit;//需要分离的hls区域
private:
	void fillHole(const Mat srcBw, Mat &dstBw);
	void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);
	void hullArea(Mat &src,Mat &dst);
};

