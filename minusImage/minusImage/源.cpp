
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "ViBe.h"
#include "minusImage.h"


using namespace cv;
using namespace std;

//培养自己不怕麻烦实践的爱好，要将方法记录下来，一定要硬着头皮才能有效果

//是否可以计算一个连通区域间的距离，从而将连通区域进行连接，这个计算量其实几乎可以忽略不计。

//可以将人的脚底位置作为其所在位置，根据在地板上的位置来判断

//比较烦躁，就把任务肢解开，坚信自己是可以的。 至少完成一个简单场景下比较稳定的多目标跟踪

//1 就选取vibe作为检测区域，最后一定要膨胀到很厉害，并且用很大的开运算来处理，先不考虑多人合并时产生的问题，假设效果是良好的，
//   然后选取该连通区域下的最大包围区域，对图像进行去色调。或许这个需要单独写一个函数
//7 针对遮挡问题要克服 考虑采用对大块的连通区域做检测的方法进行人物分割   这个是这个要处理的 ，比如，缩小后，分割后的连通区域，并且应该能获取其比例关系

//3 将原来的overlap类进行缓冲，设计好一个可调参数，让缓冲机制可以成功运行   overlap要提供的应该是一个连续的标号，并且能够克服一定的丢失问题
//4 修改之前的重心，将数据放在脚底部
//2 利用openCV计算颜色直方图的数据，然后通过大块分块，要把数据按比例来计算，最后归一化，并将值记录下来

//8 掌握海康威视彩色摄像头的用法

int main()
{
	//测试视频
	VideoCapture capture;
	capture.open("bodyvideoJH.avi");
	//原始图像
	Mat ori=imread("ori.png");//将原始图片转换成YCRCB颜色空间下的，单独测试下色调分离的情况,最终发现和灰度处理的效果是一样的，灰度的公式和RGB的公式相同。

	//vibe部分   
	ViBeHue Vibe(ori);//输入初始图像

	//minusImg部分
		//minusImage minusClass(ori);

	while (1)//处理部分
	{	

		Mat framed,grayed;
		capture>>framed;
		if (framed.empty())
		{
			break;
		}

		//minus处理
//  		Mat showMinusMat;
//  		minusClass(framed,showMinusMat);

	
		//vibe处理
		Vibe(framed,grayed);
		imshow("shu",grayed);

		waitKey(100);
	}
}