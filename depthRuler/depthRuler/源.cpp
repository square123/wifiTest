#include "myKinect.h"

void onMouse(int event, int x, int y, int flags, void* param)  
{  
	Mat *im = reinterpret_cast<Mat*>(param);  
	switch (event)  
	{  
	case CV_EVENT_LBUTTONDOWN:     //鼠标左键按下响应：返回坐标和灰度  
		std::cout<<x<<" "<<y<<" "<<static_cast<int>(im->at<short>(cv::Point(x,y)))<<std::endl;  
		break;            
	}  
}  

int main()
{
	Kinect myKinect;
	myKinect.InitKinect();
	myKinect.InitDepth();
	bool flag=0;
	while (1)
	{
		myKinect.depthProcess2();
		Mat ruler=myKinect.depthMat8;
		//构建网格线  512*424   50个像素值是一条线 先不管其他的
		for (int i=1;i<11;i++) //画竖线
		{
			line(ruler,Point(i*50,0),Point(i*50,424),255,1,8);

		}
		for (int j=1;j<9;j++)  //画横线
		{
			line(ruler,Point(0,j*50),Point(512,j*50),255,1,8);
		}
		imshow("depth",myKinect.depthMat8);
		setMouseCallback("depth",onMouse,&myKinect.depthMat16); 

		//输出对应的深度数据 辅助 鼠标按下输出对应深度

		waitKey(3);
	}
	return 0;
}