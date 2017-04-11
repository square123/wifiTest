#ifndef WIFIPROBE_H
#define WIFIPROBE_H
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
#include<time.h>//加入时间
#include <fstream>//希望输入时间
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
	//主要函数
	void InitProbe();//初始化探针数据
	void probeProcess();//探针处理数据部分 
	void rssiIntegrate(char time[14],mncatsWifi &Probedata,int index);//作为新探针的整合程序
	void rssiMissGet();//用于找出数据为空的集合
	void rssiMissNot();//用于填补数据为空的集合
	void rssiForMac();//用于找出对应Mac的RSSI序列集合
	void rssiForMacAnalyticed();//用于特定序列的变换 暂定，或许以后可以合并

	//其他子函数
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
	char NormRssi(char a[],int ccc);//利用高斯滤波来选取rssi
	bool timeCompare(char time1[14],char time2[],int delta);//返回时间两个时间是否相差delta秒
	time_t charToTimeInt(char ttt[14]);//字符串转换成时间int
	void timesSysc(time_t &syscTime,mncatsWifi &Probedata);//同步多探针的时间
	int charTimeGetSecond(char ttt[14]);//获取时间的后两位并转换成int型
	void mobileManuOutput(mncatsWifi &Probedata);//输出手机网卡的厂商

private:	

	//全局常量定义部分
	   //探针部分
	static const int sameTimeMacNum=30; //表示同一时刻内探针默认最多存储的数据类型数
	static const int ProbeNum=3; //表示一共有几个探针返回数据
	static const int THD=-70;
	static const int buffNum=5; //这个参数很关键，要调的合适一些，调的过大过小都会出现出现数据丢失的问题，目前为4的时候数据较好。
	   //socket通信部分
	static const int SERVER_PORT=2222 ;
	static const int BUFFER_SIZE=2048 ;

	//中间变量部分
		//socket通信用
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; //客户端地址

		//mac码厂商查找部分
	std::map<std::string,std::string> mobileManu;//Mac码映射关系表

		//文件存储部分
	FILE *fpGet,*fpNot;

		//探针rssiIntegrate()优选部分
	unsigned char zeroMac[6];//提前定义好0变量
	char zeroTimestamp[14];
	char zeroRssi;
	struct rssiTemp //定义处理数据的结构体
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][60][sameTimeMacNum];
	int rssiTempIndex[ProbeNum][60];//存储探针记录的索引

		//探针rssiIntegrate()优选部分
	struct rssiMiss //定义补全后数据的结构体
	{
		char Timestamp[14];
		unsigned char Mumac[6];
		char Rssi[ProbeNum];//记录探针的数据
	}seled[60][sameTimeMacNum];//用来存储填补好rssi数据的结构体
	int rssiMissIndex[60];//存储探针记录的索引
	time_t processGetIndex,processNotIndex;

		//时间校准部分
	time_t syscTimeBuff[ProbeNum];
	time_t detProbeTime[ProbeNum];//用来存储同步时间差异的

	//其他子程序
	double myErf(double x);//erf函数
	double myNormCdf(double x);//正态分布函数
};

#endif