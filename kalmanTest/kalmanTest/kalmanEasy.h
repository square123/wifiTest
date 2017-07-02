#pragma once
#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

class kalmanEasy  //����ֻ�����ڶ�����ֱ���˶������˲����۲�������Ļ�е�����λ�ã�������ʱ����֪����ô��������Ϊ�˷��㣬�ǽ�openCV��kalman�˲�����װ������
{
public:
	kalmanEasy();
	~kalmanEasy();
	Point filterOutput(Point &x);//�����ǹ۲�ֵ�����Ϊ�˲����ֵ

private:
	RNG rng;
	KalmanFilter KF;
	//1.kalman filter setup    ֵ��ע����ǣ���Ϊ�۲�������������������Ҫ�õ��Ǿ����д���������ǵ���һ��������д��
	static const int stateNum=4;                                      //״ֵ̬4��1����(x,y,��x,��y) ���С�x,��yָ��Ӧ�����ٶȣ�ÿ�θ���Ĭ����һ����λ
	static const int measureNum=2;                                    //����ֵ2��1����(x,y)	
	int count; //����ǰ��ĳ�ʼֵ�趨�Ĳ�̫�ã����Ե�Ϊǰ��֡ʱ��Ĭ������ľ�����ʵֵ����Ԥ�����ֵΪ���º��Ԥ��ֵ
	Mat measurement;
};
