//#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
//#include<time.h>//加入时间
#include <fstream>//希望输入时间

#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP绑定端口号
#define BUFFER_SIZE 1024 
#define RSSITHD -30   //wifi探针探测的阈值
#define REC_RSSI_COUNT 10 //系统默认存储rssi的种类个数

struct cliprobeData { //探针返回数据格式
	unsigned char Apmac[6];          //源 AP 的 MAC 地址
	unsigned short int Vendorid;     //厂商 ID
	unsigned short int Reserved1;    //保留位
	unsigned char Bssid[6];          //AP 的 BSSID
	unsigned char Radiotype;         //接收消息的无线电类型 0x01：802.11b
	//0x02：802.11g
	//0x03：802.11a
	//0x04：802.11n
	//0x05：802.11ac
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
	unsigned char Datarate;          //接收消息的无线电类型：0x01：802.11b
	//0x02：802.11g
	//0x03：802.11a
	//0x04：802.11n
	//0x05：802.11ac
	unsigned char MPDUflags;         //根据MPDU的flag标记确定MU报文是否携带Frame Control和Sequence Control字段
	//1：代表携带对应字段，0：代表不携带对应字段：
	unsigned char Mumac[6];          //发射信号MU的MAC地址
	unsigned short int Framecontrol;   //MPDU中“FrameControl”字段
	unsigned short int Sequencecontrol;//MPDU 中“SequenceControl”字段
	unsigned short int Reserved6;      //保留位
};

//将char类型转换成02X字符串型
string charTo02XStr(unsigned char input)
{
	int high,low;
	char out[2]={0};
	
	high=(input&240)>>4;//高四位
	low=input&15;		//低四位
	if (high>=10)
	{
		out[0]=(high-10)+'A';
	}
	else
	{
		out[0]=(high)+'0';
	}
	if (low>=10)
	{
		out[1]=(low-10)+'A';
	}
	else
	{
		out[1]=(low)+'0';
	}
	string output(out,2);//第二个参数用来控制string的长度，不然会出现乱码
	return output;
}

string macToString(unsigned char Mymac[6])//完成将char类型转换成字符串
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+":"+charTo02XStr(Mymac[i]);
	}
	return output;
}

void getSpecialMac(unsigned char Mymac[6])//转换字符串的形式的函数
{
	//unsigned char Mymac[6]={0};//b0,e2,35,2b,da,e0
	cout<<"输入要特定识别的字符串格式：（请以冒号格式输入，且为小写）"<<endl;
	scanf("%02x:%02x:%02x:%02x:%02x:%02x",&Mymac[0],&Mymac[1],&Mymac[2],&Mymac[3],&Mymac[4],&Mymac[5]);
	cout<<"输出转换后形式："<<endl;
	printf("%hhu,%hhu,%hhu,%hhu,%hhu,%hhu\n",unsigned char(Mymac[0]),Mymac[1],Mymac[2],Mymac[3],Mymac[4],Mymac[5]);
	//system("pause");
}

void getSpecialRssi(char MyRssi)//转换字符串的形式的函数
{
	cout<<"输入最小的dB："<<endl;
	scanf("%d",MyRssi);
	cout<<"输出转换后形式："<<endl;
	printf("%d\n",MyRssi);
	//system("pause");
}

float meanRssiSpecial(float lastOutput,float input,int num)//直接计算平均值,用时间换取空间，用上次已知的值和统计的次数，直接求得几个值的平均数
{														   //后来觉得该函数不好，用float或许太麻烦了，通过统计和直接去求似乎精度也会高些
	float avgNum;
	float fnum;
	fnum=float(num);
	if(num==1)
	{
		avgNum=input;
	}
	else
	{
		avgNum=((fnum-1.0)/fnum)*lastOutput+(1.0/fnum)*input;
	}
	printf("均值函数返回的float值：%d\n",avgNum);
	return avgNum;
}

char MaxRssi(char rssi1,char rssi2)//返回较大的RSSI值，且该值不能能等于0
{
	char output;
	char t_1,t_2;
	t_1=rssi1;
	t_2=rssi2;
	if(int(t_1)==0)
	{
		output=rssi2;
	}
	else if(int(t_2)==0)
	{
		output=rssi1;
	}
	else
	{
		output=int(t_1)>int(t_2)?rssi1:rssi2;
	}
	return output;
}

//wifi的类
class Wifi
{
public:

	struct selMacRssi//定义该结构体数组来统计出现RSSI值的情况，避免使用变长的存储，由于短时间内不会出现很多Mac地址小于某阈值，所以这里只设置10个值
	{
		unsigned char selMumac[6];      //发射信号MU的MAC地址
		int sumRssi;                    //返回RSSI的总和
		char maxRssi;                   //返回RSSI的最大值
		int num;						//统计该RSSI出现的次数
	}sel[REC_RSSI_COUNT];//定义要存储的结构体数组
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp;
	sockaddr_in servAddr;//服务器地址
	struct cliprobeData *packageData;
	sockaddr_in clientAddr; 
	Wifi();
	~Wifi();
	void InitWifi();
	void wifiProcess();
	void wifiProcessed();
	void reSelMacRssi();
private:

};

Wifi::Wifi()
{
	memset(&zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的量
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

void Wifi::InitWifi()//初始化server服务器端
{
	//文本类使用getchar(),openCV使用waitkey,当获取键盘按键时采用通用的GetKeyState(), system("pause")也是一个很好的选择
	/*加载winsock文件*/
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	//创建嵌套字
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//绑定嵌套字

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 

	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
}
void Wifi::wifiProcess()//记录Mac值和对应的RSSI值
{
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//定义接收
	if (recvfrom(s,buffer,BUFFER_SIZE,0,(SOCKADDR *)&clientAddr,&clientAddrLength) == SOCKET_ERROR)
	{
		printf("recvfrom() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	packageData = (struct cliprobeData *) buffer;
	if(int(packageData->Rssi)>RSSITHD)
	{
		cout<<"已检测到信号:\n"<<endl;
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",packageData->Mumac[0],packageData->Mumac[1],\
		packageData->Mumac[2],packageData->Mumac[3],packageData->Mumac[4],packageData->Mumac[5]);
		printf( "%d\n",packageData->Rssi);
		for(int i=0;i<REC_RSSI_COUNT;i++)
		{
			if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//当检测到区域为零时，把测得值赋值给该数组
			{
				memcpy(sel[i].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
				sel[i].num=1;
				sel[i].maxRssi=packageData->Rssi;
				sel[i].sumRssi=int(packageData->Rssi);
				break;//跳出循环
			}
			else if(memcmp(packageData->Mumac,sel[i].selMumac,sizeof(unsigned char)*6)==0)//比较是否出现这样的组合,memcmp 相同返回0,不同返回非零
			{
				sel[i].num=sel[i].num+1;
				sel[i].maxRssi=MaxRssi(sel[i].maxRssi,packageData->Rssi);
				sel[i].sumRssi=sel[i].sumRssi+int(packageData->Rssi);
				//cout<<sel[i].sumRssi<<endl;//方便检验
				//cout<<sel[i].maxRssi<<endl;
				break;//跳出循环
			}
		}
	}
}

void Wifi::reSelMacRssi()//将记录的结构体数组重新置零
{
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

void Wifi::wifiProcessed()//判断和输出程序
{
	int rssiTemp=-100;
	int index=0;
	int del;
	string str;
	for(int i=0;i<REC_RSSI_COUNT;i++)
		{
			if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//当检测到区域为零时，跳出程序
			{
				break;
			}
			else //记录最大值的程序
			{
				del=sel[i].sumRssi/sel[i].num;
				if(del>rssiTemp)
				{
					rssiTemp=del;
					index=i;
				}
			}
		}
	if(memcmp(sel[index].selMumac,zeroMac,sizeof(unsigned char)*6)==0)
	{
		printf("对不起,系统未匹配到Mac地址\n");
	}
	else
	{
	//输出最大值的部分
	printf("匹配到的最可能的mac码：\n");
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n",sel[index].selMumac[0],sel[index].selMumac[1],\
		sel[index].selMumac[2],sel[index].selMumac[3],sel[index].selMumac[4],sel[index].selMumac[5]);
	printf("对应的平均信号值：%d\n",rssiTemp);
	printf("对应的最大信号值：%d\n",sel[index].maxRssi);
	str=sel[index].selMumac[0]+':'+sel[index].selMumac[1]+':'+sel[index].selMumac[2]+':'+sel[index].selMumac[3]+':'+sel[index].selMumac[4]+':'+sel[index].selMumac[5];
	cout<<str<<endl;
	//printf("对应最大的统计次数:%d",sel[index].num);
	}
}

int main()
{
	Wifi wifi;
	wifi.InitWifi();
	string del;
	unsigned char myMac1[6]={176,226,53,43,218,224};
	del=macToString(myMac1);
	cout<<del<<endl;
	while(1)
	{
		wifi.wifiProcess();
		if(GetKeyState(VK_ESCAPE)<0)
		{
			wifi.wifiProcessed();
			wifi.~Wifi();
			system("pause");
		}
	}
}