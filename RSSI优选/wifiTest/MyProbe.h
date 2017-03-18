#ifndef MYPROBE_H
#define MYPROBE_H
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
#include<time.h>//加入时间
#include <fstream>//希望输入时间
#include <cmath>
#pragma comment(lib,"WS2_32.lib")
class Probe
{
public:

	Probe();
	~Probe();
	//主要函数
	void InitProbe();//初始化探针数据
	void probeProcess();//探针处理数据部分
	void probeTimeFix(char src[],char dst[14] );//用来存储修复后的时间数据格式，根据系统时间来完成修正，因此需要在使用时要同步
	void rssiPurify(char time[14],FILE *f,int index);//进行同一时间的RSSI优选
	void probeSysc(FILE *f);//处理多探针同步的函数
	//其他子函数
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
	char NormRssi(char a[],int ccc);//利用高斯滤波来选取rssi
	bool timeCompare(char time1[14],char time2[],int delta);//返回时间两个时间是否相差delta秒
	time_t charToTimeInt(char ttt[14]);//字符串转换成时间int
	int charTimeGetSecond(char ttt[14]);//获取时间的后两位并转换成int型

private:	

	//全局常量定义部分
	static const int sameTimeMacNum=10; //表示同一时刻内探针默认最多存储的数据类型数
	static const int ProbeNum=3; //表示一共有几个探针返回数据
	static const int SERVER_PORT=2222 ;
	static const int BUFFER_SIZE=1024 ;
	static const int THD=-80;
	static const int buffNum=5; //这个参数很关键，要调的合适一些，调的过大过小都会出现出现数据丢失的问题，目前为4的时候数据较好。
	static const int baseIndex=0;
	static const int rssiCapacity=30;//这个数是存同一数据同一时间的RSSI的容量

	//探针数据结构部分
	struct cliprobeData {
		unsigned char Apmac[6];          //源 AP 的 MAC 地址
		unsigned short int Vendorid;     //厂商 ID
		unsigned short int Reserved1;    //保留位
		unsigned char Bssid[6];          //AP 的 BSSID
		unsigned char Radiotype;         //接收消息的无线电类型 0x01：802.11b   0x02：802.11g    0x03：802.11a	0x04：802.11n	0x05：802.11ac
		unsigned char Channel;           //AP接收MU报文时所在的信道
		unsigned char Associated;        //MU是否连接到 AP：0x01：是 0x02：否
		unsigned char Messagetype;       //报文类型 0x00：Probe Request 0x01：Association Request
		char Timestamp[14];              //接收MU消息的时间点，毫秒级的时间戳
		unsigned short int Reserved2;    //保留位
		unsigned char Mutype;            //MU的类型：0x01：未知类型 0x02：MU
		unsigned short int Reserved3;    //保留位
		char Rssi;                       //测量到的以dBm为单位的RSSI 整数值 十进制：-128 到127
		unsigned short int Reserved4;    //保留位
		unsigned char Noisefloor;        //测量到的以dBm为单位的底噪整数值
		unsigned short int Reserved5;    //保留位
		unsigned char Datarate;          //接收消息的无线电类型：0x01：802.11b    0x02：802.11g	0x03：802.11a	0x04：802.11n		0x05：802.11ac
		unsigned char MPDUflags;         //根据MPDU的flag标记确定MU报文是否携带Frame Control和Sequence Control字段//1：代表携带对应字段，0：代表不携带对应字段：
		unsigned char Mumac[6];          //发射信号MU的MAC地址
		unsigned short int Framecontrol;   //MPDU中“FrameControl”字段
		unsigned short int Sequencecontrol;//MPDU 中“SequenceControl”字段
		unsigned short int Reserved6;      //保留位
	};
	struct cliprobeData *packageData;
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; //客户端地址

	//文件存储部分
	FILE *fp1,*fp2,*fp3,*fpSysc;

	//探针优选部分
	unsigned char zeroMac[6];
	char zeroTimestamp[14];
	char zeroRssi;
	char timeTemp[ProbeNum][14];//用来区分时间变化的变量
	char timeBuffer[ProbeNum][14];//时间的寄存器，用来存储上上次的时间
	int indexForPure[ProbeNum];
	int indexForPureBuf[ProbeNum];//作为上一变量的寄存器
	bool flag[ProbeNum];
	struct rssiTemp
	{
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][sameTimeMacNum];
	rssiTemp selBuffer[ProbeNum][sameTimeMacNum];//是上一变量的寄存器

	//同步部分
	struct syscProbed
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi[ProbeNum];
	}syscResult[sameTimeMacNum];//用来输出同步后的结果
	time_t syscTime;	//系统时间
	int storeIndex;//存储数据的索引
	int processIndex;//处理同步的索引
	struct syscProbe	//同步多探针的结构体	配合baseIndex的要完成同步的数据，这里直接改成60，按秒取余存取，虽然浪费了空间但节约了时间，提高了鲁棒性
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi;
	}syscStr[60][ProbeNum][sameTimeMacNum];//三维结构体数组，要寄存的时间、探针数量、每秒钟存储的数据
	syscProbe syscStrForIndex[buffNum][sameTimeMacNum];//baseIndex使用 负责存储的数据，只是要完成延时的功能
	syscProbe zeroSysc;//定义存储全零为syscProbe格式
	char rssiData[ProbeNum][sameTimeMacNum][rssiCapacity];//存rssi的for normrssi
	int rssiIndex[ProbeNum][sameTimeMacNum];//用于推进RSSI的索引
	int storeIndexBuffer;//同步函数的标志位
	
	//其他子程序
	double myErf(double x);//erf函数
	double myNormCdf(double x);//正态分布函数
	time_t selectSysPrbTime(syscProbe sysc[sameTimeMacNum]);//输出结构体时间的函数
	void reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum]);//将结构体数据紧凑
	void AllreduceSyscProbe(syscProbe Allsysc[buffNum][ProbeNum][sameTimeMacNum],syscProbe Allsysced[buffNum][ProbeNum][sameTimeMacNum]);//将整个结构体数组缩减
};
#endif