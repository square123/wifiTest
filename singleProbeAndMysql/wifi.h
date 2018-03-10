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

#define BUFFER_SIZE 1024 //可以根据具体情况调整大小

//封装的探针数据类型格式，或者将所有的数据格式都封装成和这个类似的格式
class mncatsWifi //（为了方便替换采用内联形式)
{
public :
	//经过格式化后的类中应该有的数据
	std::string initData;//用来存储char转换后的字符串
	std::string mac1;//存储探针的mac码 0-11
	std::string mac2;//存储探测的mac码 13-24
	std::string rssi;//存储Rssi值 26-28
	std::string dtype;//信号类型 30-31 //以后再分析
	std::string wifidate;//日期 33-42
	std::string wifitime;//时间 44-51
	std::string timeStamp;//综合的字符串
	unsigned char cmac2[6];//存放mac2码的数组
	char crssi;//char格式的rssi
	char Timestamp[14];//接口的时间格式

	//两种形式的构造函数 
	mncatsWifi() {};//啥都不干

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
	mncatsWifi& wifiReform(mncatsWifi & temp1)//将探针获取的数据格式化处理
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

//探针信息收集和记录所需要的类
class Wifi
{
public:

	//析构和构造函数
	Wifi(const char* setMac,int setPort, int setTHD);//直接在函数构造的时候就将参数确定下来
	~Wifi();

	//程序运行主要调用的函数
	void wifiProcess(bool &frag, mncatsWifi &outData, string &correctTime);

	//开机要读取的白名单							
	set<string> denyList;//定义需要剔除的MAC地址

private:

	//与探针交互时的socket通信变量（UDP，不太一样就不用mySocket）
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; 
	
	//加入一个时间更新函数 开机要更新
	void recorrectTime(mncatsWifi &probeData);

	//要确定的参数
	int wifiPort;
	int wifiTHD;
	string wifiMac;

	//校正时间偏移量
	time_t charToTimeInt(char ttt[14]);
	time_t deltaTime;
	bool isCorrected;
	string timeToString(char timeData[14]);//time转string 函数

};

