#include "minusImage.h"

//��ʼ������
minusImage::minusImage(Mat &src)
{
	src.copyTo(oriImg);
	GaussianBlur(oriImg,oriImgGauss,Size(3,3),0,0);
	cvtColor(oriImgGauss,oriImgCovert,CV_BGR2HLS_FULL);
	cvtColor(oriImgGauss,oriImgGray,CV_RGB2GRAY);
	split(oriImgCovert,orisplit);
}

//��������
minusImage::~minusImage()
{
}

//�������������
void minusImage::operator()(Mat &image, Mat &fgmask)
{
	//�������
	Mat ImgGauss; //��˹�˲����ͼƬ
	Mat ImgGray;//�Ҷ�ת����ͼƬ
	Mat ImgCovert; //ɫ��ת�����ͼƬ
	vector<Mat> imgSplit;//��Ҫ�����hls����
	Mat diffGray,diffHue;//��ֵ
	Mat dwMatRaw;//��С�����ݲ����ľ���
	Mat dwOstuMat; //��򷨴����ľ���

	//��û����ľ���
	GaussianBlur(image,ImgGauss,Size(3,3),0,0);
	cvtColor(ImgGauss,ImgCovert,CV_BGR2HLS_FULL);
	split(ImgCovert,imgSplit);
	cvtColor(ImgGauss,ImgGray,CV_RGB2GRAY);

	//step1 ��gray������,�õ������¶�ֵ����ͼ��
	absdiff(ImgGray,oriImgGray,diffGray);
	threshold(diffGray,dwMatRaw,5,255,THRESH_TOZERO);//����ֵ���µ�����Ĩƽ
	threshold(dwMatRaw,dwOstuMat,0,255,THRESH_OTSU);//ostu��
	imshow("step1",dwOstuMat);

	//step2 ��hue������
	Mat hueSmooth;
	absdiff(imgSplit.at(0),orisplit.at(0),diffHue);
	
	threshold(diffHue,hueSmooth,30,255,THRESH_BINARY);
	imshow("step2",diffHue);

	Mat zong,ozong;
	addWeighted(hueSmooth&dwOstuMat,0.2,dwOstuMat&diffGray,0.8,0,zong);
	threshold(zong,ozong,0,255,THRESH_OTSU);
	imshow("z",ozong);
}


void minusImage::fillHole(const Mat srcBw, Mat &dstBw)//�ο����ϵ���亯����������
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//��չͼ��
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));//�ǳ������������㷨�ķ�����

	Mat cutImg;//�ü���չ��ͼ��
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

//ѡ����ʷ�Χ����ͨ����
void minusImage::selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //ȥ���߽�ֵ
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
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

//��������ͨ������͹���ϲ�
void minusImage::hullArea(Mat &src,Mat &dst) 
{
	vector<vector<Point>> contours;  //ͨ���÷���Ҳ�ǿ��Եģ������䵱��ԭ������vector<Point>��Ч������˫ͨ������
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> hull(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		convexHull(Mat(contours[i]),hull[i],false);
	}
	if(contours.size()<1)  //ȥ���߽�ֵ
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,hull,i,(255),CV_FILLED);
		}
	}
}