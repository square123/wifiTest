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
	//imshow("step1",dwOstuMat);

	//step2 ��hue������
	Mat hueSmooth;
	absdiff(imgSplit.at(0),orisplit.at(0),diffHue);
	threshold(diffHue,hueSmooth,30,255,THRESH_BINARY);
	//imshow("step2",diffHue);

	Mat zong,ozong;
	addWeighted(hueSmooth&dwOstuMat,0.2,dwOstuMat&diffGray,0.8,0,zong);
	threshold(zong,ozong,0,255,THRESH_OTSU);
	imshow("z",ozong);
}


