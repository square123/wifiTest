#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include <kinect.h>

using namespace cv;
using namespace std;

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease !=NULL)
	{
		pInterfaceToRelease ->Release();
		pInterfaceToRelease = NULL;
	}

}

int main()
{
	// Sensor
	IKinectSensor* pSensor;   
	HRESULT hResult = S_OK;

	hResult = GetDefaultKinectSensor( &pSensor );   
	if( FAILED( hResult ) )
	{
		cerr<< "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();  
	if( FAILED( hResult ) )
	{
		std::cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// Source
	IColorFrameSource* pColorSource;   
	hResult = pSensor->get_ColorFrameSource( &pColorSource );  
	if( FAILED( hResult ) )
	{
		std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
		return -1;
	}

	// Reader
	IColorFrameReader* pColorReader;  
	hResult = pColorSource->OpenReader( &pColorReader );   
	if( FAILED( hResult ) )
	{
		std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	int width = 1920;   
	int height = 1080;   
	unsigned int bufferSize = width * height * 4 * sizeof( unsigned char );   
	Mat bufferMat( height, width, CV_8UC4 );   
	Mat colorMat( height / 2, width / 2, CV_8UC4 );   
	namedWindow( "Color" );

	int n = 0;
	string dst_img_name = "G://My vibe//colorSim1//colorSim1//out//";
	char chari[1000];
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	while( 1 )
	{
		// Frame
		IColorFrame* pColorFrame=nullptr;   
		hResult=pColorReader->AcquireLatestFrame( &pColorFrame );  
		if( SUCCEEDED( hResult ) )
		{
			hResult = pColorFrame->CopyConvertedFrameDataToArray( bufferSize, reinterpret_cast<BYTE*>( bufferMat.data ), ColorImageFormat_Bgra );  
			if( SUCCEEDED( hResult ) )
			{
				resize( bufferMat, colorMat, cv::Size(), 0.5, 0.5 );  
			} 
		}    
		SafeRelease( pColorFrame );
		// Show Window
		imshow( "Color", colorMat );

		// 按下“空格键”，触发保存机制

			sprintf_s(chari, "%04d", n);
			dst_img_name += "color_";
			dst_img_name += chari;
			dst_img_name += ".PNG";
			imwrite(dst_img_name, colorMat, compression_params);
			dst_img_name = "G://My vibe//colorSim1//colorSim1//out//";
			n++;

		if( waitKey(1) == VK_ESCAPE )
		{
			break;
		}
	}

	return 0;
}