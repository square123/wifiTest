#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

//ѧ��д�̳к��Լ�����һ�����ͼ���࣬�Ժ�ĳ��򶼼̳и���

//minusImage����  �޷�ʹ��KCF�ķ�����������ʶ������ʱ���õ�����ǰȦ���ķ�����Ч���������룬�����Ҫ�Լ��Ϊ����Ȼ��������Ƕȿ˷��ڵ������⣨����Ӧ�ý���ɫ���ȥ��
//��Hɫ�������ȥ����ȥ����Ӱ�����ڵ�˼·�ǲ�����ͨ��������޳�һ��������Ȼ�������е���ȡH���������Ƿ��ܷ��ü��ķ����������Ӱ�ļ���,��Ϊ��ɫ��Ӱ�죬Ӧ������С��ΧȦ��˼·
//�㷨����
//���и�˹�˲�(3*3)�ٶȽϿ죬��ȥ�������
//������Ҷ�ͼ�����ûҶ�ͼ���������ostu����Ӧ�ָ�
//�Էָ����ͼ������ȡH�������õ�һ����С��������򣬶�����ֱ���������㣬�Ӷ��޳���Ӱ

//����vibe�㷨�ٶ�Ҫ��һЩ����ĳЩ����¸��ܸ��ӱ�������ˣ���������˫���գ�ѡȡ����������vibe����С��������ѡȡͳ�������������ֵ���ĸ�ֵԽ��������ѡȡ�ĸ���hue���д����Ӷ�ȥ����Ӱ��Ч����



class minusImage
{
public:
	minusImage(Mat &src);
	~minusImage();
	void operator()(Mat &image, Mat &fgmask);//�������
	

	Mat oriImg;//����ͼƬ
	Mat oriImgGauss; //��˹�˲����ͼƬ
	Mat oriImgGray;//�Ҷ�ת����ͼƬ
	Mat oriImgCovert; //ɫ��ת�����ͼƬ
	vector<Mat> orisplit;//��Ҫ�����hls����
private:
	void fillHole(const Mat srcBw, Mat &dstBw);
	void selectArea(Mat &src,Mat &dst, int minNum,int maxNum);
	void hullArea(Mat &src,Mat &dst);
};

