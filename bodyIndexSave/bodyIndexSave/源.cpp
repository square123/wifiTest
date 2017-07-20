#include "myKinect.h"
#define imagePathSave "E://imageInput//2//"
int main()
{
	Kinect myKinect;
	myKinect.InitKinect();
	myKinect.InitColor();
	myKinect.InitDepth();
	myKinect.InitBodyIndex();
	bool flag=0;
	while (1)
	{

		myKinect.colorProcess();
		myKinect.depthProcess2();
		myKinect.bodyIndexProcess();
		imshow("color",myKinect.colorHalfSizeMat);
		imshow("depth",myKinect.depthMat8);
		imshow("bodyIndex",myKinect.bodyIndexMat);
		myKinect.kinectSaveAll(imagePathSave);

		waitKey(3);
	}
	return 0;
}