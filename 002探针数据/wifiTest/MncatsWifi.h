#ifndef MNCATSWIFI
#define MNCATSWIFI
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
class mncatsWifi 
{
public :
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
	mncatsWifi& wifiReform(mncatsWifi & temp1)
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

#endif