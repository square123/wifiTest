#include "myKinect.h"

Kinect::Kinect()  
{
	pColorFrame = nullptr;
	pDepthFrame = nullptr;
	pBodyIndexFrame=nullptr;
	pBodyFrame = nullptr;
	colorHalfSizeMat.create(ColorHeight/2, ColorWidth/2, CV_8UC4);
	colorMat.create( ColorHeight, ColorWidth, CV_8UC4);
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );
	bodyIndexMat.create(DepthHeight, DepthWidth, CV_8UC3);
	bodyIndexMatInt.create(DepthHeight, DepthWidth, CV_8UC1);
	InfraredMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	InfraredMat8.create(DepthHeight, DepthWidth, CV_8UC1);
	saveTmpIntClr=0;
	saveTmpIntDep=0;
	saveTmpIntBoI=0;
}
Kinect::~Kinect()
{
	SafeRelease( pDepthSource );
	SafeRelease( pDepthReader );
	SafeRelease( pDepthDescription );
	SafeRelease( pColorSource );
	SafeRelease( pColorReader );
	SafeRelease( pColorDescription );
	SafeRelease( pBodyIndexSource );
	SafeRelease( pBodyIndexReader );
	SafeRelease( pInfraredSource );
	SafeRelease( pInfraredReader );
	SafeRelease( pCoordinateMapper );
	SafeRelease( pBodySource );
	SafeRelease( pBodyReader );
	if ( pSensor )
	{
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();
}
HRESULT Kinect::InitKinect()
{
	hResult = GetDefaultKinectSensor( &pSensor );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:GetDefaultKinectSensor" <<endl;
		return -1;
	}
	hResult = pSensor->Open();
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::Open()" <<endl;
		return -1;
	}
}
HRESULT Kinect::InitColor()//������InitKinect()
{
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}
	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		return -1; 
	}

}
HRESULT Kinect::InitDepth()//������InitKinect()
{
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
		return -1;
	}
}

HRESULT Kinect::InitBodyIndex()
{
	hResult = pSensor->get_BodyIndexFrameSource( &pBodyIndexSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_BodyIndexFrameSource"<<endl;
		return -1;
	}
	hResult = pBodyIndexSource->OpenReader( &pBodyIndexReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IBodyIndexFrameSource::OpenReader()"<<endl;
		return -1;
	}
}

HRESULT Kinect::InitInfrared()
{
	hResult = pSensor->get_InfraredFrameSource( &pInfraredSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_InfraredFrameSource()"<<endl;
		return -1;
	}
	hResult = pInfraredSource->OpenReader( &pInfraredReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IInfraredFrameSource::OpenReader()"<<endl;
		return -1;
	}
}

HRESULT Kinect::InitCoorMap()
{
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}
}
HRESULT Kinect::Initbody()
{
	hResult = pSensor->get_BodyFrameSource( &pBodySource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
		return -1;
	}
	hResult = pBodySource->OpenReader( &pBodyReader ); 
	if( FAILED( hResult ) ){
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return -1;
	}
}

void Kinect::depthProcess()//16λ��ʾ�ᱨ�� ���øú�������
{
	unsigned int depthBufferSize = DepthWidth * DepthHeight * sizeof( unsigned short );
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthMat16.data));
		if ( SUCCEEDED(hResult) )
		{
			depthMat16.convertTo(depthMat8, CV_8U, -255.0f/8000.0f, 255.0f );
		}
	}
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}
void Kinect::depthProcess2()
{
	UINT16 *pBuffer = NULL;
	UINT nBufferSize = 0; 
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8

			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pBuffer;  //16   �������ֽ�Ϊ��λ���ж�ȡ
				ptrDepth16[j] = depth;        //ֱ�Ӵ洢��ʾ
				ptrDepth8[j] = depth%256; //תΪ8λ�洢��ʾ
				pBuffer++;               //16   �������ֽ�Ϊ��λ���ж�ȡ
			}
		}
	}
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}
void Kinect::colorProcess()
{
	unsigned int colorBufferSize = ColorWidth * ColorHeight * 4 * sizeof( unsigned char );
	hResult = pColorReader->AcquireLatestFrame( &pColorFrame );      
	if ( SUCCEEDED(hResult) )
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray( colorBufferSize,
			reinterpret_cast<BYTE*> (colorMat.data), ColorImageFormat::ColorImageFormat_Bgra );
		if ( SUCCEEDED(hResult) )
		{
			resize( colorMat,colorHalfSizeMat , Size(), 0.5, 0.5 );
		}
	}
	SafeRelease( pColorFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}

void Kinect::bodyIndexProcess()
{
	cv::Vec3b color[6]; 
	color[0] = cv::Vec3b( 255, 0, 0 );
	color[1] = cv::Vec3b( 0, 255, 0 );
	color[2] = cv::Vec3b( 0, 0, 255 );
	color[3] = cv::Vec3b( 255, 255, 0 );
	color[4] = cv::Vec3b( 255, 0, 255 );
	color[5] = cv::Vec3b( 0, 255, 255 );
	UINT8 bodyInt[6]={15,55,95,135,175,215};//��������6����
	hResult = pBodyIndexReader->AcquireLatestFrame( &pBodyIndexFrame ); 
	if( SUCCEEDED( hResult ) ){
		unsigned int bufferSize = 0;
		unsigned char* buffer = nullptr;
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer( &bufferSize, &buffer );
		if( SUCCEEDED( hResult ) ){
			for( int y = 0; y < DepthHeight; y++ ){
				for( int x = 0; x < DepthWidth; x++ ){
					unsigned int index = y * DepthWidth + x;
					if( buffer[index] != 0xff ){
						bodyIndexMat.at<cv::Vec3b>( y, x ) = color[buffer[index]];
						bodyIndexMatInt.at<char>(y,x)=bodyInt[buffer[index]];
					}
					else{
						bodyIndexMat.at<cv::Vec3b>( y, x ) = cv::Vec3b( 0, 0, 0 ); 
						bodyIndexMatInt.at<char>(y,x)=0;
					}
				}
			}
		}
	}
	SafeRelease( pBodyIndexFrame );
}
//ֱ���ùؽڵ���Ϊ�����������ټ���,�ƺ�ͨ���ؽڵ����̫�ɿ�����������Ϊ����̫���ˣ����Բ��ɿ������ܻ���ȶ������
float Kinect::detPeopleDepth()//�����볢����ֵ�˲�������Ч�����ܲ�����,�Ժ�Ҫ��סҪʵ��ѭ���������ʱ�򣬿�����ȡ��ķ�����ʵ��
{
	float depthTemp[6]={10.0};
	float Temp=8;
	hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
	if( SUCCEEDED( hResult ) )
	{
		IBody* pBody[BODY_COUNT] = { 0 }; //Ĭ�ϵ��� 6 ������ ����ʼ�����еĹ�����Ϣ
		hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//���¹������ݣ�
		if( SUCCEEDED( hResult ) ){
			for( int count = 0; count < BODY_COUNT; count++ ){  //count����0��5��6�����ҹ�������ʾ
				BOOLEAN bTracked = false;    //��ʼ������׷�ٵ����塱��ֵΪ��
				hResult = pBody[count]->get_IsTracked( &bTracked );  //ȷ������׷�ٵ����塣
				if( SUCCEEDED( hResult ) && bTracked ){
					Joint joint[JointType::JointType_Count];   //ȡ������Joint(�ؽ�)��JointType��һ��ö�����ͣ���ͬλ�õĹؽڵ㶼�ǲ�ͬ�ı�ű�ʾ�ġ�count��һ����ֵ25��
					hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //ȡ������Joint(�ؽ�)��
					float JointMean=0;
					JointMean=(joint[ JointType_Neck].Position.Z+joint[ JointType_SpineShoulder].Position.Z+joint[ JointType_ShoulderLeft].Position.Z+joint[ JointType_ShoulderRight].Position.Z)/4.0;
					depthTemp[count]=JointMean;//����⵽�����ֵ���������о�ֻ��һ���ؽڵ㲻���ɿ�
					////����任 ���ܻ��õ�
					//float xx,yy,zz;
					//xx=joint[ JointType_SpineMid].Position.X;
					//yy=(cos(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Y+sin(-27.0/180*PI)*joint[JointType_SpineMid].Position.Z);
					//zz=((-1)*sin(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Y+cos(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Z);
					//cout<<"x="<<xx<<"z="<<zz<<endl;
				}
				else 
				{
					depthTemp[count]=6.0;
				}
			}
		}	
		//��������������
		for(int i=0;i<BODY_COUNT;i++)
		{
			Temp=min(Temp,depthTemp[i]);
		}
		//ѭ����ȫ�ͷ�6����Ϊbody��ָ��
		for( int count = 0; count < BODY_COUNT; count++ )
		{   
			SafeRelease( pBody[count] );
		}
	}
	SafeRelease( pBodyFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����	
	return Temp;
}

int Kinect::detPeopleDepth2()
{
	UINT8 bodyInt[6]={15,55,95,135,175,215};//��������6����
	bodyIndexProcess();
	int Temp=7000;
	depthProcess2();//ʹ������������������
	Mat body0=(bodyIndexMatInt==bodyInt[0]);
	Mat body1=(bodyIndexMatInt==bodyInt[1]);
	Mat body2=(bodyIndexMatInt==bodyInt[2]);
	Mat body3=(bodyIndexMatInt==bodyInt[3]);
	Mat body4=(bodyIndexMatInt==bodyInt[4]);
	Mat body5=(bodyIndexMatInt==bodyInt[5]);
	//Ҫ�ø�ʴ�㷨��ȥ�룬ֻ��ע��ȣ����Ը�ʴ�㹻
	Mat element =getStructuringElement(MORPH_ELLIPSE,Size(7,7));
	erode(body0,body0,element);
	erode(body1,body1,element);
	erode(body2,body2,element);
	erode(body3,body3,element);
	erode(body4,body4,element);
	erode(body5,body5,element);
	//�������ȵ������
	Mat bodyDepth0,bodyDepth1,bodyDepth2,bodyDepth3,bodyDepth4,bodyDepth5;
	depthMat16.copyTo(bodyDepth0,body0);
	depthMat16.copyTo(bodyDepth1,body1);
	depthMat16.copyTo(bodyDepth2,body2);
	depthMat16.copyTo(bodyDepth3,body3);
	depthMat16.copyTo(bodyDepth4,body4);
	depthMat16.copyTo(bodyDepth5,body5);
    int bodyDepthMean[6];//��������ÿ���˵����
	bodyDepthMean[0]=avgMat(bodyDepth0,areaMatOri(body0));
	bodyDepthMean[1]=avgMat(bodyDepth1,areaMatOri(body1));
	bodyDepthMean[2]=avgMat(bodyDepth2,areaMatOri(body2));
	bodyDepthMean[3]=avgMat(bodyDepth3,areaMatOri(body3));
	bodyDepthMean[4]=avgMat(bodyDepth4,areaMatOri(body4));
	bodyDepthMean[5]=avgMat(bodyDepth5,areaMatOri(body5));
	for(int i=0;i<BODY_COUNT;i++)
	{
		if(bodyDepthMean[i]!=0)
		{
			Temp=min(Temp,bodyDepthMean[i]);
		}
	}
	return Temp;
}

void Kinect::InfraredProcess(float fGamma)
{
	UINT16 *pBuffer = NULL;
	UINT nBufferSize = 0; 
	hResult = pInfraredReader->AcquireLatestFrame(&pInfraredFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pInfraredFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrInfrared16 = InfraredMat16.ptr<ushort>(i);  // 16
			uchar* ptrInfrared8 = InfraredMat8.ptr<uchar>(i);  //8
			for(int j=0;j<DepthWidth;j++)
			{
				USHORT Infrared = *pBuffer;  //16   �������ֽ�Ϊ��λ���ж�ȡ
				ptrInfrared16[j] = Infrared;        //ֱ�Ӵ洢��ʾ
				//�ο���΢��Ĺٷ�������Ч���ܸܵ�
				// normalize the incoming infrared data (ushort) to a float ranging from 
				// [InfraredOutputValueMinimum, InfraredOutputValueMaximum] by
				// 1. dividing the incoming value by the source maximum value
				float intensityRatio = static_cast<float>(*pBuffer) / InfraredSourceValueMaximum;
				// 2. dividing by the (average scene value * standard deviations)
				intensityRatio /= InfraredSceneValueAverage * InfraredSceneStandardDeviations;
				// 3. limiting the value to InfraredOutputValueMaximum
				intensityRatio = min(InfraredOutputValueMaximum, intensityRatio);
				// 4. limiting the lower value InfraredOutputValueMinimym
				intensityRatio = max(InfraredOutputValueMinimum, intensityRatio);
				// 5. converting the normalized value to a byte and using the result
				byte intensity = static_cast<byte>(intensityRatio * 255.0f); 
				ptrInfrared8[j] = uchar(intensity); //תΪ8λ�洢��ʾ
				pBuffer++;               //16   �������ֽ�Ϊ��λ���ж�ȡ
			}
		}
	}
	//٤��У��
	Mat temp=InfraredMat8.clone();
	MyGammaCorrection(InfraredMat8,temp,fGamma);
	temp.copyTo(InfraredMat8);
	SafeRelease( pInfraredFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}

void Kinect::kincetSave(Mat a,string savePath,int opt)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	char optTemp[2];
	sprintf_s(optTemp,sizeof(optTemp),"%d",opt);
	string savePathTemp=savePath;
	switch (opt)
	{
	case saveDepth:
		{
			if( GetKeyState( VK_NUMPAD1 ) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +  optTemp +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,a);//�洢ͼ��
				saveTmpIntDep++;   
				savePathTemp =savePath;
			}
			break;
		}
	case saveColor:
		{
			if( GetKeyState( VK_NUMPAD2 ) < 0 ){
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp + optTemp +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,a);//�洢ͼ��
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
			break;
		}
	}
}

void Kinect::kinectSaveAll(string savePath)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string savePathTemp=savePath;
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%05d",saveTmpIntDep); 
				savePathTemp = savePathTemp +"depth" +"_"+ saveTmpStrDep + ".png";
				//imwrite(savePathTemp,depthDeTemp);//�洢ȥ������ͼ��
				imwrite(savePathTemp,depthMat16);//�洢ȥ������ͼ��
				saveTmpIntDep++;   
				savePathTemp =savePath;
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%05d",saveTmpIntClr); 
				savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,colorHalfSizeMat);//�洢��ɫͼ��
				saveTmpIntClr++;   
				savePathTemp =savePath;
				sprintf_s(saveTmpStrBoI,sizeof(saveTmpStrBoI),"%05d",saveTmpIntBoI); 
				savePathTemp = savePathTemp+"bodyIndex" +"_"+ saveTmpStrBoI + ".png";
				imwrite(savePathTemp,bodyIndexMat);//�洢bodyIndexͼ��
				saveTmpIntBoI++;   
				savePathTemp =savePath;
}

int Kinect::areaMatForSpecNum(Mat &src,int num)//���ؾ������ض�ֵ�����
{
	int areaSum=0;
	Mat srcTemp=(src==(num));//��ʱ�����ݸ�ʽת����UC1
	for(int i=0;i<srcTemp.rows;i++)
	{
		uchar* data= srcTemp.ptr<uchar>(i);
		for(int j=0;j<srcTemp.cols;j++){
			uchar a=data[j];
			areaSum=areaSum+int(a);
		}
	}
	return areaSum=areaSum/255;
}

int Kinect::areaMatOri(Mat &src)//���ؾ�������,Ҫ���������ݸ�ʽΪUC1
{
	int areaSum=0;
	for(int i=0;i<src.rows;i++)
	{
		uchar* data= src.ptr<uchar>(i);
		for(int j=0;j<src.cols;j++){
			uchar a=data[j];
			areaSum=areaSum+int(a);
		}
	}
	return areaSum=areaSum/255;
}

void Kinect::MyGammaCorrection(Mat& src, Mat& dst, float fGamma)  //٤��У�� �ο����ϵĳ���
{  
    CV_Assert(src.data);  
    // accept only char type matrices  
    CV_Assert(src.depth() != sizeof(uchar));  
    // build look up table  
    unsigned char lut[256];  
    for( int i = 0; i < 256; i++ )  
    {  
        lut[i] = saturate_cast<uchar>(pow((float)(i/255.0), fGamma) * 255.0f);  
    }  
    dst = src.clone();  
    const int channels = dst.channels();  
    switch(channels)  
    {  
        case 1:  
            {  
                MatIterator_<uchar> it, end;  
                for( it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++ )  
                    //*it = pow((float)(((*it))/255.0), fGamma) * 255.0;  
                    *it = lut[(*it)];  
                break;  
            }  
        case 3:   
            {  
                MatIterator_<Vec3b> it, end;  
                for( it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++ )  
                {  
                    //(*it)[0] = pow((float)(((*it)[0])/255.0), fGamma) * 255.0;  
                    //(*it)[1] = pow((float)(((*it)[1])/255.0), fGamma) * 255.0;  
                    //(*it)[2] = pow((float)(((*it)[2])/255.0), fGamma) * 255.0;  
                    (*it)[0] = lut[((*it)[0])];  
                    (*it)[1] = lut[((*it)[1])];  
                    (*it)[2] = lut[((*it)[2])];  
                }  
                break;  
            }  
    }  
}  

int Kinect::avgMat(Mat &src,int num)//��ƽ���������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�,��ʵ�õĸ����ԭ���Ǹ����������
{
	int hh,mm;
	int h;
	h=num;
	hh=sumMat(src);
	if(h!=0)//���ڳ���һ��Ҫ���ǳ���Ϊ������
	{
		mm=hh/h;
		return mm;
	}
	else
	{
		return 0;
	}
}

int Kinect::sumMat( Mat &src)//��ͣ������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�
{
	Mat srcTemp;
	src.convertTo(srcTemp,CV_32SC1);//ע��int�Ͷ�Ӧ����CV_32SC1
	int matSumNum=0;
	for(int i=0;i<srcTemp.rows;i++)
	{
		int* data= srcTemp.ptr<int>(i);
		for(int j=0;j<srcTemp.cols;j++){
			int a=data[j];
			matSumNum=matSumNum+a;
		}
	}
	return matSumNum;
}