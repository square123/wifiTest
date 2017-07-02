#include "kalmanEasy.h"

kalmanEasy::kalmanEasy()
{
	KF.init(stateNum, measureNum, 0);	
	KF.transitionMatrix = *(Mat_<float>(4, 4) <<1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1);  //ת�ƾ���A �˶�״̬���̣������پ������
	setIdentity(KF.measurementMatrix);                                             //��������H
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));                            //ϵͳ�����������Q
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));                        //���������������R
	setIdentity(KF.errorCovPost, Scalar::all(1));                                  //����������Э�������P
	rng.fill(KF.statePost,RNG::UNIFORM,0,0);   //��ʼ״ֵ̬x(0)
	measurement = Mat::zeros(measureNum, 1, CV_32F);                           //��ʼ����ֵx'(0)����Ϊ����Ҫ�������ֵ�����Ա����ȶ��� �����м����
	count=0;//ͳ�ƴ���
}

kalmanEasy::~kalmanEasy()
{
}

Point kalmanEasy::filterOutput(Point &src) //��������Ҫ���˲��ģ����ʱҪ�˲���
{
	if (count>3)//ǰ��֡������Ƿ��˲���ĸ���
	{
		KF.predict();//��Ҫ��������� ��Ȼ�޷�����
		measurement.at<float>(0) = (float)src.x;
		measurement.at<float>(1) = (float)src.y;	
		Mat res=KF.correct(measurement);//��ʱ���º�
		return Point(res.at<float>(0),res.at<float>(1));
	}else
	{
		count++;
		return src;
	}
}