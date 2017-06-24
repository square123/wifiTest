#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	char filename[10000];
	const int NUM = 121;

	Mat frame;
	VideoWriter write( "bodyvideoO.avi", CV_FOURCC( 'M','J','P','G' ), 100, Size(960,540) );
	namedWindow( "video", CV_WINDOW_NORMAL );

	for ( int i = 7; i < NUM; i++ )
	{
		sprintf_s( filename, "G://My vibe//colorSim2//ConsoleApplication1//ConsoleApplication1//out//4//color_%04d.PNG", i );
		frame = imread( filename, 1 );
		imshow( "video", frame );
		write<<frame;
		if ( waitKey(50) == 27 )
		{
			break;
		}
	}
	return 0;
}