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
#include "MncatsWifi.h"
#pragma comment(lib,"WS2_32.lib")
class Probe
{
public:

	Probe();
	~Probe();
	//��Ҫ����
	void InitProbe();//��ʼ��̽������
	void probeProcess();//̽�봦�����ݲ���
	void rssiPurify(char time[14],mncatsWifi &Probedata,FILE *f,int index);//����ͬһʱ���RSSI��ѡ
	void probeSysc(FILE *f);//�����̽��ͬ���ĺ���
	//�����Ӻ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
	char NormRssi(char a[],int ccc);//���ø�˹�˲���ѡȡrssi
	bool timeCompare(char time1[14],char time2[],int delta);//����ʱ������ʱ���Ƿ����delta��
	time_t charToTimeInt(char ttt[14]);//�ַ���ת����ʱ��int
	void timesSysc(time_t &syscTime,mncatsWifi &Probedata);//ͬ����̽���ʱ��
	int charTimeGetSecond(char ttt[14]);//��ȡʱ��ĺ���λ��ת����int��

private:	

	//ȫ�ֳ������岿��
	static const int sameTimeMacNum=30; //��ʾͬһʱ����̽��Ĭ�����洢������������
	static const int ProbeNum=3; //��ʾһ���м���̽�뷵������
	static const int SERVER_PORT=2222 ;
	static const int BUFFER_SIZE=2048 ;
	static const int THD=-90;
	static const int buffNum=5; //��������ܹؼ���Ҫ���ĺ���һЩ�����Ĺ����С������ֳ������ݶ�ʧ�����⣬ĿǰΪ4��ʱ�����ݽϺá�
	static const int baseIndex=0;
	static const int rssiCapacity=30;//������Ǵ�ͬһ����ͬһʱ���RSSI������

	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; //�ͻ��˵�ַ

	//�ļ��洢����
	FILE *fp1,*fp2,*fp3,*fpSysc;

	//̽����ѡ����
	unsigned char zeroMac[6];
	char zeroTimestamp[14];
	char zeroRssi;
	char timeTemp[ProbeNum][14];//��������ʱ��仯�ı���
	char timeBuffer[ProbeNum][14];//ʱ��ļĴ����������洢���ϴε�ʱ��
	int indexForPure[ProbeNum];
	int indexForPureBuf[ProbeNum];//��Ϊ��һ�����ļĴ���
	bool flag[ProbeNum];
	struct rssiTemp
	{
		unsigned char selMumac[6];
		char maxRssi;
		//char normRssi;//����
	}sel[ProbeNum][sameTimeMacNum];
	rssiTemp selBuffer[ProbeNum][sameTimeMacNum];//����һ�����ļĴ���

	//ͬ������
	struct syscProbed
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi[ProbeNum];
		//char NRssi[ProbeNum];//����
	}syscResult[sameTimeMacNum];//�������ͬ����Ľ��

	
	time_t syscTimeBuff[ProbeNum];
	time_t detProbeTime[ProbeNum];//�����洢ͬ��ʱ������
	int storeIndex;//�洢���ݵ�����
	int processIndex;//����ͬ��������
	struct syscProbe	//ͬ����̽��Ľṹ��	���baseIndex��Ҫ���ͬ�������ݣ�����ֱ�Ӹĳ�60������ȡ���ȡ����Ȼ�˷��˿ռ䵫��Լ��ʱ�䣬�����³����
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi;
		//char NRssi;//����
	}syscStr[60][ProbeNum][sameTimeMacNum];//��ά�ṹ�����飬Ҫ�Ĵ��ʱ�䡢̽��������ÿ���Ӵ洢������
	syscProbe syscStrForIndex[buffNum][sameTimeMacNum];//baseIndexʹ�� ����洢�����ݣ�ֻ��Ҫ�����ʱ�Ĺ���
	syscProbe zeroSysc;//����洢ȫ��ΪsyscProbe��ʽ
	char rssiData[ProbeNum][sameTimeMacNum][rssiCapacity];//��rssi��for normrssi
	int rssiIndex[ProbeNum][sameTimeMacNum];//�����ƽ�RSSI������
	int storeIndexBuffer;//ͬ�������ı�־λ

	//�����ӳ���
	double myErf(double x);//erf����
	double myNormCdf(double x);//��̬�ֲ�����
	time_t selectSysPrbTime(syscProbe sysc[sameTimeMacNum]);//����ṹ��ʱ��ĺ���
	void reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum]);//���ṹ�����ݽ���
	void AllreduceSyscProbe(syscProbe Allsysc[buffNum][ProbeNum][sameTimeMacNum],syscProbe Allsysced[buffNum][ProbeNum][sameTimeMacNum]);//�������ṹ����������
};

#endif