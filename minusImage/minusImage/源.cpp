
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "ViBe.h"
#include "minusImage.h"


using namespace cv;
using namespace std;

//�����Լ������鷳ʵ���İ��ã�Ҫ��������¼������һ��ҪӲ��ͷƤ������Ч��

//�Ƿ���Լ���һ����ͨ�����ľ��룬�Ӷ�����ͨ����������ӣ������������ʵ�������Ժ��Բ��ơ�

//���Խ��˵Ľŵ�λ����Ϊ������λ�ã������ڵذ��ϵ�λ�����ж�

//�ȽϷ��꣬�Ͱ�����֫�⿪�������Լ��ǿ��Եġ� �������һ���򵥳����±Ƚ��ȶ��Ķ�Ŀ�����

//1 ��ѡȡvibe��Ϊ����������һ��Ҫ���͵��������������úܴ�Ŀ������������Ȳ����Ƕ��˺ϲ�ʱ���������⣬����Ч�������õģ�
//   Ȼ��ѡȡ����ͨ�����µ�����Χ���򣬶�ͼ�����ȥɫ�������������Ҫ����дһ������
//7 ����ڵ�����Ҫ�˷� ���ǲ��öԴ�����ͨ���������ķ�����������ָ�   ��������Ҫ����� �����磬��С�󣬷ָ�����ͨ���򣬲���Ӧ���ܻ�ȡ�������ϵ

//3 ��ԭ����overlap����л��壬��ƺ�һ���ɵ��������û�����ƿ��Գɹ�����   overlapҪ�ṩ��Ӧ����һ�������ı�ţ������ܹ��˷�һ���Ķ�ʧ����
//4 �޸�֮ǰ�����ģ������ݷ��ڽŵײ�
//2 ����openCV������ɫֱ��ͼ�����ݣ�Ȼ��ͨ�����ֿ飬Ҫ�����ݰ����������㣬����һ��������ֵ��¼����

//8 ���պ������Ӳ�ɫ����ͷ���÷�

int main()
{
	//������Ƶ
	VideoCapture capture;
	capture.open("bodyvideoJH.avi");
	//ԭʼͼ��
	Mat ori=imread("ori.png");//��ԭʼͼƬת����YCRCB��ɫ�ռ��µģ�����������ɫ����������,���շ��ֺͻҶȴ����Ч����һ���ģ��ҶȵĹ�ʽ��RGB�Ĺ�ʽ��ͬ��

	//vibe����   
	ViBeHue Vibe(ori);//�����ʼͼ��

	//minusImg����
		//minusImage minusClass(ori);

	while (1)//������
	{	

		Mat framed,grayed;
		capture>>framed;
		if (framed.empty())
		{
			break;
		}

		//minus����
//  		Mat showMinusMat;
//  		minusClass(framed,showMinusMat);

	
		//vibe����
		Vibe(framed,grayed);
		imshow("shu",grayed);

		waitKey(100);
	}
}