#include "otherFuncs.h"

Mat element3=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
Mat element5=getStructuringElement(MORPH_ELLIPSE,Size(5,5));
Mat element7=getStructuringElement(MORPH_ELLIPSE,Size(7,7));
Mat elementL=getStructuringElement(MORPH_RECT,Size(1,7));

void fillHole(const Mat srcBw, Mat &dstBw)//�ο����ϵ���亯����������
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
void selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
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
void hullArea(Mat &src,Mat &dst) 
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

void deNoise(Mat &src,Mat &dst)//Ҫ���ݲ������е��������ڴ��ͼ��Ҫ���зָ
{
	
	Mat tempMat;
	//���ÿ����㴦��,ȥ��ϸС���  ���������С��ֻ����С�����������
	erode(src,src,element3);
	dilate(src,src,element3);
	fillHole(src,tempMat);
	//���ÿ����㴦��,ȥ��ϸС���
	dilate(tempMat,tempMat,elementL);
	erode(tempMat,tempMat,elementL);
// 	erode(src,src,element7);
// 	dilate(src,src,element7);
	//Mat tempMat2;
	fillHole(tempMat,dst);
}