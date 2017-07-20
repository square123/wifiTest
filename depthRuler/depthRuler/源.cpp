#include "myKinect.h"

void onMouse(int event, int x, int y, int flags, void* param)  
{  
	Mat *im = reinterpret_cast<Mat*>(param);  
	switch (event)  
	{  
	case CV_EVENT_LBUTTONDOWN:     //������������Ӧ����������ͻҶ�  
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
		//����������  512*424   50������ֵ��һ���� �Ȳ���������
		for (int i=1;i<11;i++) //������
		{
			line(ruler,Point(i*50,0),Point(i*50,424),255,1,8);

		}
		for (int j=1;j<9;j++)  //������
		{
			line(ruler,Point(0,j*50),Point(512,j*50),255,1,8);
		}
		imshow("depth",myKinect.depthMat8);
		setMouseCallback("depth",onMouse,&myKinect.depthMat16); 

		//�����Ӧ��������� ���� ��갴�������Ӧ���

		waitKey(3);
	}
	return 0;
}