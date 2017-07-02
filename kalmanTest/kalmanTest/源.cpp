#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "kalmanEasy.h"
#include <stdio.h>
using namespace cv;
using namespace std;

const int winHeight=600;
const int winWidth=800;

Point mousePosition= Point(winWidth>>1,winHeight>>1);

//mouse event callback
void mouseEvent(int event, int x, int y, int flags, void *param )
{
	if (event==CV_EVENT_MOUSEMOVE) {
		mousePosition = Point(x,y);
	}
}

int main (void)
{
	kalmanEasy kfe;
	namedWindow("kalman");
	setMouseCallback("kalman",mouseEvent);

	Mat image(winHeight,winWidth,CV_8UC3,Scalar(0));

	while (1)
	{
		//draw 
		image.setTo(Scalar(255,255,255,0));//将数据冲重新清空
		circle(image,kfe.filterOutput(mousePosition),5,Scalar(0,255,0),3);    //predicted point with green
		circle(image,mousePosition,5,Scalar(255,0,0),3); //current position with red		
		//circle(image,sss,5,Scalar(0,0,255),3); //current position with red		

		char buf[256];
		//sprintf_s(buf,256,"predicted position:(%3d,%3d)",predict_pt.x,predict_pt.y);
		putText(image,buf,Point(10,30),CV_FONT_HERSHEY_SCRIPT_COMPLEX,1,Scalar(0,0,0),1,8);
		sprintf_s(buf,256,"current position :(%3d,%3d)",mousePosition.x,mousePosition.y);
		putText(image,buf,cvPoint(10,60),CV_FONT_HERSHEY_SCRIPT_COMPLEX,1,Scalar(0,0,0),1,8);

		imshow("kalman", image);
		int key=waitKey(3);
		if (key==27){//esc   
			break;   
		}		
	}
}