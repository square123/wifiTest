#pragma once
#include<windows.h>
#include<string>
#include<iostream>
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <sstream>
#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define BUFFER_SIZE 1024 //���Ը��ݾ������������С

//��װ��̽���������͸�ʽ�����߽����е����ݸ�ʽ����װ�ɺ�������Ƶĸ�ʽ
class mncatsWifi //��Ϊ�˷����滻����������ʽ)
{
public :
	//������ʽ���������Ӧ���е�����
	std::string initData;//�����洢charת������ַ���
	std::string mac1;//�洢̽���mac�� 0-11
	std::string mac2;//�洢̽���mac�� 13-24
	std::string rssi;//�洢Rssiֵ 26-28
	std::string dtype;//�ź����� 30-31 //�Ժ��ٷ���
	std::string wifidate;//���� 33-42
	std::string wifitime;//ʱ�� 44-51
	std::string timeStamp;//�ۺϵ��ַ���
	unsigned char cmac2[6];//���mac2�������
	char crssi;//char��ʽ��rssi
	char Timestamp[14];//�ӿڵ�ʱ���ʽ

	//������ʽ�Ĺ��캯�� 
	mncatsWifi() {};//ɶ������

	mncatsWifi(std::string &s):mac1(s.substr(0,12)),mac2(s.substr(13,12)),rssi(s.substr(26,3)),dtype(s.substr(30,2)),\
		wifidate(s.substr(33,10)),wifitime(s.substr(44,8)),timeStamp(0)
	{
		wifiReform(*this);
		crssi=atoi(rssi.c_str());
		memcpy(Timestamp,timeStamp.c_str(),sizeof(unsigned char)*14);
		for (int i = 0; i < 6; i++)
			cmac2[i]=strtol(mac2.substr(i*3,2).c_str(),NULL,16);
	}

	mncatsWifi(char ss[]):initData(ss),mac1(initData.substr(0,12)),mac2(initData.substr(13,12)),\
		rssi(initData.substr(26,3)),dtype(initData.substr(30,2)),wifidate(initData.substr(33,10)),\
		wifitime(initData.substr(44,8)){
			wifiReform(*this);
			crssi=atoi(rssi.c_str());
			memcpy(Timestamp,timeStamp.c_str(),sizeof(unsigned char)*14);
			for (int i = 0; i < 6; i++)
				cmac2[i]=strtol(mac2.substr(i*3,2).c_str(),NULL,16);	
	}

private:
	mncatsWifi& wifiReform(mncatsWifi & temp1)//��̽���ȡ�����ݸ�ʽ������
	{
		for (auto &i:temp1.mac2)
		{
			i=toupper(i);
		}
		temp1.mac1=temp1.mac1.substr(0,2)+":"+temp1.mac1.substr(2,2)+":"+temp1.mac1.substr(4,2)+":"+temp1.mac1.substr(6,2)+":"+temp1.mac1.substr(8,2)+":"+temp1.mac1.substr(10,2);
		temp1.mac2=temp1.mac2.substr(0,2)+":"+temp1.mac2.substr(2,2)+":"+temp1.mac2.substr(4,2)+":"+temp1.mac2.substr(6,2)+":"+temp1.mac2.substr(8,2)+":"+temp1.mac2.substr(10,2);
		temp1.timeStamp=temp1.wifidate.substr(0,4)+temp1.wifidate.substr(5,2)+temp1.wifidate.substr(8,2)+temp1.wifitime.substr(0,2)+\
			temp1.wifitime.substr(3,2)+temp1.wifitime.substr(6,2);
		return temp1;
	}
};

//̽����Ϣ�ռ��ͼ�¼����Ҫ����
class Wifi
{
public:

	//�����͹��캯��
	Wifi(const char* setMac,int setPort, int setTHD);//ֱ���ں��������ʱ��ͽ�����ȷ������
	~Wifi();

	//����������Ҫ���õĺ���
	void wifiProcess(bool &frag, mncatsWifi &outData, string &correctTime);

	//����Ҫ��ȡ�İ�����							
	set<string> denyList;//������Ҫ�޳���MAC��ַ

private:

	//��̽�뽻��ʱ��socketͨ�ű�����UDP����̫һ���Ͳ���mySocket��
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; 
	
	//����һ��ʱ����º��� ����Ҫ����
	void recorrectTime(mncatsWifi &probeData);

	//Ҫȷ���Ĳ���
	int wifiPort;
	int wifiTHD;
	string wifiMac;

	//У��ʱ��ƫ����
	time_t charToTimeInt(char ttt[14]);
	time_t deltaTime;
	bool isCorrected;
	string timeToString(char timeData[14]);//timeתstring ����

};

