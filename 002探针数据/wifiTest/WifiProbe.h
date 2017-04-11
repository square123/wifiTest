#ifndef WIFIPROBE_H
#define WIFIPROBE_H
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
#include<time.h>//����ʱ��
#include <fstream>//ϣ������ʱ��
#include <cmath>
#include <vector>
#include <map>
#include "MncatsWifi.h"
#include "myCsv.h"
#pragma comment(lib,"WS2_32.lib")
class Probe
{
public:

	Probe();
	~Probe();
	//��Ҫ����
	void InitProbe();//��ʼ��̽������
	void probeProcess();//̽�봦�����ݲ��� 
	void rssiIntegrate(char time[14],mncatsWifi &Probedata,int index);//��Ϊ��̽������ϳ���
	void rssiMissGet();//�����ҳ�����Ϊ�յļ���
	void rssiMissNot();//���������Ϊ�յļ���
	void rssiForMac();//�����ҳ���ӦMac��RSSI���м���
	void rssiForMacAnalyticed();//�����ض����еı任 �ݶ��������Ժ���Ժϲ�

	//�����Ӻ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
	char NormRssi(char a[],int ccc);//���ø�˹�˲���ѡȡrssi
	bool timeCompare(char time1[14],char time2[],int delta);//����ʱ������ʱ���Ƿ����delta��
	time_t charToTimeInt(char ttt[14]);//�ַ���ת����ʱ��int
	void timesSysc(time_t &syscTime,mncatsWifi &Probedata);//ͬ����̽���ʱ��
	int charTimeGetSecond(char ttt[14]);//��ȡʱ��ĺ���λ��ת����int��
	void mobileManuOutput(mncatsWifi &Probedata);//����ֻ������ĳ���

private:	

	//ȫ�ֳ������岿��
	   //̽�벿��
	static const int sameTimeMacNum=30; //��ʾͬһʱ����̽��Ĭ�����洢������������
	static const int ProbeNum=3; //��ʾһ���м���̽�뷵������
	static const int THD=-70;
	static const int buffNum=5; //��������ܹؼ���Ҫ���ĺ���һЩ�����Ĺ����С������ֳ������ݶ�ʧ�����⣬ĿǰΪ4��ʱ�����ݽϺá�
	   //socketͨ�Ų���
	static const int SERVER_PORT=2222 ;
	static const int BUFFER_SIZE=2048 ;

	//�м��������
		//socketͨ����
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; //�ͻ��˵�ַ

		//mac�볧�̲��Ҳ���
	std::map<std::string,std::string> mobileManu;//Mac��ӳ���ϵ��

		//�ļ��洢����
	FILE *fpGet,*fpNot;

		//̽��rssiIntegrate()��ѡ����
	unsigned char zeroMac[6];//��ǰ�����0����
	char zeroTimestamp[14];
	char zeroRssi;
	struct rssiTemp //���崦�����ݵĽṹ��
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][60][sameTimeMacNum];
	int rssiTempIndex[ProbeNum][60];//�洢̽���¼������

		//̽��rssiIntegrate()��ѡ����
	struct rssiMiss //���岹ȫ�����ݵĽṹ��
	{
		char Timestamp[14];
		unsigned char Mumac[6];
		char Rssi[ProbeNum];//��¼̽�������
	}seled[60][sameTimeMacNum];//�����洢���rssi���ݵĽṹ��
	int rssiMissIndex[60];//�洢̽���¼������
	time_t processGetIndex,processNotIndex;

		//ʱ��У׼����
	time_t syscTimeBuff[ProbeNum];
	time_t detProbeTime[ProbeNum];//�����洢ͬ��ʱ������

	//�����ӳ���
	double myErf(double x);//erf����
	double myNormCdf(double x);//��̬�ֲ�����
};

#endif