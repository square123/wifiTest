#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <set>
#include<fstream>
#include <time.h>
#include "vibe.h"
#include <math.h>

#define count_num 20 //检测的连通区域最大数目
#define max_num 20 //同一时间连通区域的最大数目
using namespace cv;



class overlapPro //该类用于将连通区域通过overlap来完成标号
{
private:
	struct bwMix //定义连通区域的结构体
	{
		Rect bwRect;//矩形区域
		int markLabel;//标记
		Mat bwEdge;//边缘信息
		//Point center;//图像的重心 暂时不需要加入重心，因为考虑到后续会有相同分块会分散，所以就不加入重心了，后续合并后再考虑重心
		time_t bwTime;//探针收集到的时刻
	};
	vector<bwMix> picBwMix;
	struct bwMixRawData
	{
		char Timestamp[14];
		Point center;
	}bwMixTimeRaw[60][count_num][max_num];//时间 最多检测的连通区域 在同一时间能采集的最大区域
	int bwMixIndex[60][count_num];//存储上述索引
	time_t processIndex;
public:
	overlapPro();
	~overlapPro();
	void fillHole(const Mat srcBw, Mat &dstBw);//参考网上的填充函数，很巧妙
	void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);//选择合适范围的连通区域
	void hullArea(Mat &src,Mat &dst);//用来将连通区域用凸包合并
	void getBwMix(Mat &src,vector<bwMix> &dst);
	void bwMixToColorMat(vector<bwMix> &src,Mat &dst);
	float rectOverlap(const Rect & preBox,const Rect & nowBox);
	void renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd);
	int charTimeGetSecond(char ttt[14]);//获得得到数据的后两位
	void bwMixToOrbit(vector<bwMix> &src);
	void process(Mat &src);//将GMM建模的方法独立出来，该类要完成的只是overlap跟踪
	void timeRawProcess();//负责将信息压缩成1秒
	Mat selectedMat;
	Mat hullMat;
	Mat showColor;
	vector<bwMix> prePicBwMix;//缓冲变量便于更新
	//ofstream outfile;//用于存储轨迹数据的
};