#include "overlapPro.h"

overlapPro::overlapPro()
{
	prePicBwMix.reserve(100);
	memset(bwMixTimeRaw,0,sizeof(bwMixTimeRaw));
	memset(bwMixIndex,0,sizeof(bwMixIndex)); 
	//加入进行处理时间的时间基准，理论上是对4秒的数据进行压缩
	time_t syscTimee=time(NULL);	//这里只执行一次，后续就不需要再操作了
	processIndex=syscTimee+4;//因为是要滞后处理，所以要将处理的标志位滞后，具体的滞后时间参数可以修改
}

overlapPro::~overlapPro()
{
	outfile.close();
}
void overlapPro::fillHole(const Mat srcBw, Mat &dstBw)//参考网上的填充函数，很巧妙
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
void overlapPro::selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
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
void overlapPro::hullArea(Mat &src,Mat &dst) 
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
		dst=Mat::zeros(src.size(),CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,hull,i,(255),CV_FILLED);
		}
	}
}

//构建结构体的函数
void overlapPro::getBwMix(Mat &src,vector<bwMix> &dst)
{
	vector<Mat> contours; //这个地方好好思考下
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()>=1)  //去除边界值
	{
		for (int i = 0; i < contours.size(); i++)
		{
			bwMix temp;
			temp.bwTime=time(NULL);//获取时间
			temp.markLabel=i;
			temp.bwEdge=contours[i];
			temp.bwRect=boundingRect(contours[i]);
			//添加一个构建重心的函数
			//Moments m=moments(contours[i],false);
			//temp.center.x=m.m10/m.m00;
			//temp.center.y=m.m01/m.m00;
			dst.push_back(temp);
		}
	}
}

//根据结构体转换成彩色矩阵//颜色处理函数 进行列表化，控制最多的检测人数，不考虑人会出现割裂的痕迹 定义最多的检测的人数为10人，也是通过结构体来传递图像信息
void overlapPro::bwMixToColorMat(vector<bwMix> &src,Mat &dst)
{
	Scalar globalColor[10]; //构建颜色表
	globalColor[0] = Scalar( 67, 128, 6 );
	globalColor[1] = Scalar( 84, 80, 153 );
	globalColor[2] = Scalar( 57, 60, 79 );
	globalColor[3] = Scalar( 255, 255, 0 );
	globalColor[4] = Scalar( 61, 63, 17 );
	globalColor[5] = Scalar( 155, 175, 131 );
	globalColor[6] = Scalar( 101, 67, 254 );
	globalColor[7] = Scalar( 154, 157, 252 );
	globalColor[8] = Scalar( 173, 205, 249 );
	globalColor[9] = Scalar( 169, 200, 200 );
	if (src.size()>=1&&src.size()<11)//添加颜色的边界值，不要让值太多，当数量超过时不显示图像
	{
		for (int i=0;i<src.size();i++)
		{
			vector<Mat> tempMat;
			tempMat.push_back(src[i].bwEdge);//做了转存，将变量变成了一个整体
			drawContours(dst,tempMat,0,globalColor[src[i].markLabel],CV_FILLED,8);//将对应的标签的颜色填充
		}
	}
}

//矩形区域比较函数
float overlapPro::rectOverlap(const Rect & preBox,const Rect & nowBox) //直接使用Rect函数
{ 
	Rect rect=preBox & nowBox;//取两个矩阵的交集
	if (nowBox.area()!=0)
	{
		return float(rect.area())/float(nowBox.area());
	}else
	{
		return 0.0;
	}
}

//用旧的矩阵图像标号，更新新的标号的函数 让连续的帧overlap更新，通过结构体向量来传递图像的信息,当没有标号时，想办法去赋值，不能超过最大的数量先假定是10个
//这里先不考虑中断的情况，如果存在中断的情况，应该要有一个缓冲的感觉，比如多次取平均，感觉构造一个表比较好，这样思路比较清晰
void overlapPro::renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd)
{
	//构建一个表 n*m大小
	int m=src.size();
	int n=dst.size();
	std::set<int> S;//通过设置s筛选数据
	//构建一维数组，替代二维数组
	Mat table(n,m,CV_32FC1,0.0);//在openCV下使用mat作为2维数组更方便
	for (int i = 0; i < n; i++)
	{
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++)
		{
			data[j]=rectOverlap(src[j].bwRect,dst[i].bwRect);
			S.insert(src[j].markLabel);//剔除重复的元素
		}
	}
	//通过表进行查询，完成标号的更新 其中行号n对应的是要更新的标号，m是可以筛选的标号
	for (int i = 0; i < n; i++)//开始更新
	{
		float maxNum=thd;
		int maxIndex=-1;
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++) //找出每个列表中，最大的值
		{
			if (maxNum<data[j])
			{
				maxNum=data[j];
				maxIndex=j;
			}
		}
		//更新存在的标号 要考虑新加入和后来没有的索引
		if (maxIndex!=-1)
		{
			dst[i].markLabel=src[maxIndex].markLabel;
		}else//添加新的标号
		{
			for (int k=0;k<30;k++)//只添加0-19内的元素
			{
				if(S.find(k)==S.end())//添加不存在的标号，当其标号不存在时
				{
					dst[i].markLabel=k;
					S.insert(k);
					break;
				}
			}
		}
	}
}

//获得得到数据的后两位
int overlapPro::charTimeGetSecond(char ttt[14])
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}

//轨迹计算公式 根据相邻的重心来输出轨迹 需要考虑到中断的问题，因为轨迹会出现中断，当中断时要输出轨迹，以后再考虑,在轨迹中应该加入滤波的过程，因为图像在运动过程中图像会发生变化
//轨迹可能会出现遗漏的场景，现在就不需要加入判断
//对于分散的人应该有一个合并的过程，把相同标号的物体合并到同一物体。
//首先需要将同一标号的合并，然后将同一标号的信息取时间平均
void overlapPro::bwMixToOrbit(vector<bwMix> &src)  //这里还可能存在数据突然检测不到的可能
{ 
	//先找出来有多少重复的标号
	std::set<int> S;//通过设置s筛选数据
	for (int i = 0; i < src.size(); i++)
	{
		S.insert(src[i].markLabel);
	}
	//对重复标号的重新合并为一个矩阵，计算其重心,完后空域上的统一        如果计算连通区域间的最近距离，其标号分布应该在这里完成！！！！！！！！！！
	for (auto i = S.begin(); i != S.end(); i++)//set不能通过下标运算符访问，因此通过迭代器访问
	{
		vector<Point> tempMat;//每个标号输出一个坐标
		for (int j = 0; j < src.size(); j++)
		{
			if (*i==src[j].markLabel)
			{ 
				//cout<<*i<<endl;
				int rowNumber=src[j].bwEdge.rows;
				//int colNumber=src[j].bwEdge.cols*src[j].bwEdge.channels();
				for (int ii=0;ii<rowNumber;ii++)
				{
					int* data=src[j].bwEdge.ptr<int>(ii);
					Point pointt;
					pointt.x=data[0];
					pointt.y=data[1];
					tempMat.push_back(pointt);//做了转存，将变量变成了一个整体
				}
			}
		}
		//将信息存入结构体内，为下一步平均做准备
		char timeFix[16];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&src[0].bwTime));//时间修复
		int second=charTimeGetSecond(timeFix);
		//获取的是重心，这个对于标定不太方便
// 		Moments m=moments(tempMat,false);
// 		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.x=m.m10/m.m00;
// 		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.y=m.m01/m.m00;

		//选取最大外接矩形的底边中心点 可能还需要进行具体的测试
		Rect tempRect=boundingRect(Mat(tempMat));
		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.x = (tempRect.br().x)-(tempRect.width*0.5);
		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.y = tempRect.br().y;

		memcpy(bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].Timestamp,timeFix,sizeof(char)*14);
		//cout<<"标号为"<<*i<<"的中心是"<<bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center<<endl;
		bwMixIndex[second][*i]++;
	}
}

//将结构体的函数取平均，并将之前的数字清空，可以选择将数据输出，完成时域上的统一
//负责将信息压缩成1秒
void overlapPro::timeRawProcess()
{
	char timeFixed[16];
	time_t processIndexInit=processIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processIndexInit));
	int second=charTimeGetSecond(timeFixed);
	
	for (int i=0;i<count_num;i++)//每个count
	{
		double xSum=0.0,ySum=0.0;
		if (bwMixIndex[second][i]!=0)//如果空不操作
		{
			for (int j=0;j<bwMixIndex[second][i];j++)//计算变量
			{
				xSum+=bwMixTimeRaw[second][i][j].center.x;
				ySum+=bwMixTimeRaw[second][i][j].center.y;
			}
			int xAve=xSum/bwMixIndex[second][i]; //由于是图像上的矩阵，就还是取整比较好
			int yAve=ySum/bwMixIndex[second][i];

			//暂时加入数据
// 			int r1=sqrt((xAve-342)*(xAve-342)+(yAve-209)*(yAve-209));
// 			int r2=sqrt((xAve-113)*(xAve-113)+(yAve-188)*(yAve-188));
// 			int r3=sqrt((xAve-293)*(xAve-293)+(yAve-112)*(yAve-112));
// 			int r4=sqrt((xAve-180)*(xAve-180)+(yAve-99)*(yAve-99));
// 
// 			//文件输出操作(暂时的函数，可以后续加入Kalman进行更新)
// 			cout<<timeFixed<<","<<i<<","<<r1<<","<<r2<<","<<r3<<","<<r4<<endl;
// 			outfile<<timeFixed<<","<<i<<","<<r1<<","<<r2<<","<<r3<<","<<r4<<endl;
		}
	}
	//清空操作
	memset(bwMixTimeRaw[second],0,sizeof(bwMixRawData)*count_num*max_num);
	memset(bwMixIndex[second],0,sizeof(int)*count_num);
}

void overlapPro::deHue(Mat &src,Mat &colorSrc,Mat &oriImgHue,Mat &dst) //连通域，图像序列，初值图像，输出图像
{
	//先完成hue分量的提取
	Mat ImgCovert; //色域转换后的图片
	vector<Mat> imgSplit;//需要分离的hls区域
	cvtColor(colorSrc,ImgCovert,CV_BGR2HLS_FULL);
	split(ImgCovert,imgSplit);
	Mat diffHue,hueSmooth;
	absdiff(imgSplit.at(0),oriImgHue,diffHue);
	threshold(diffHue,hueSmooth,20,255,THRESH_BINARY); //选择出hue分量
	//合并
	vector<vector<Point>> contours;  //通过该方法也是可以的，其矩阵充当了原来类似vector<Point>的效果，是双通道矩阵
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> deHueEdge(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		Mat temp=Mat::zeros(src.size(),CV_8UC1);
		drawContours(temp,contours,i,(255),CV_FILLED);//建立一个暂存矩阵，在该矩阵中进行操作
		approxPolyDP(temp&hueSmooth,deHueEdge[i],3,true); //取矩阵的交集，然后选取区域的Hue分量的最近包围轮廓
	}
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,deHueEdge,i,(255),CV_FILLED);
		}
	}
}

void overlapPro::process(Mat &src,Mat & colorSrc, Mat &oriSrcHue)     //老师提出了可以采用人脸肤色来实现跟踪，或许在某些情况下也是一种比较好的方法，在处理中加入彩色矩阵
{
	vector<bwMix> picBwMix;
	picBwMix.reserve(100);
	selectArea(src,selectedMat,180,-1);//通过函数对一些连通域小的进行筛选  以后可以根据实地判断筛选 针对稀疏人物目标的跟踪,或者尝试采用人物重叠时的分割
	//imshow("dd",selectedMat); 
	Mat ImgGauss; //高斯滤波后的图片
	GaussianBlur(colorSrc,ImgGauss,Size(3,3),0,0); //这个操作拿到外面去，少一次操作
	//还有长宽的判断 这里就不考虑了，在特殊情况下再考虑
	Mat ll;
	//利用hue去掉一些多余的分量！！！！ 该方案是针对环境的可选方案
	deHue(selectedMat,ImgGauss,oriSrcHue,ll);
	imshow("xx",ll);
	//将连通区域用凸多边形表示并填充
	hullArea(ll,hullMat);
	/*imshow("hh",hullMat);*/

	//再将连通区域合并 防止出人像有缺失  （暂时不考虑，默认性能比较好）
	//获取结构体（定义函数结构体）
	time_t systime=time(NULL);//加入系统时间
	getBwMix(hullMat,picBwMix);//得到结构体    在得到结构体中加入彩色概率分量！！！！！！！！！

	//对结构体进行更新
	//下幅图像标记引用上幅图像的标记 （输入是前一帧和后一帧）
	//如果没有标记则重新赋值一个新的标记    
	//当标记已经不存在时，要想办法去重（怎么处理丢失问题，这个也很重要，现在先不考虑，等5月结束后，写完论文再考虑该事情，加大算法的复杂度）
	if (prePicBwMix.size()!=0)  //kalman滤波应该可以完成在输出的目标丢失后的操作，将目标的预测进行再次预测。！！！！！！
	{
		renewVec(prePicBwMix,picBwMix,0.0);//在进行匹配时应该考虑彩色分量对匹配进行的更新！！！！！！！
	}
	prePicBwMix=picBwMix;//对数据进行更新
	showColor=Mat::zeros(hullMat.size(),CV_8UC3);
	bwMixToColorMat(picBwMix,showColor);
	imshow("color",showColor);
	//输出轨迹
	bwMixToOrbit(picBwMix);   //kalman滤波应该可以完成在输出的目标丢失后的操作，将目标的预测进行再次预测。！！！！！
	if (systime>=processIndex)//进行插空操作 最终达到循环操作,改成大于等于是因为数据有可能存在跳秒
	{
		timeRawProcess();
		processIndex++;
	}
}
