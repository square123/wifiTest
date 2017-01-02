
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

#define SERVER_PORT 2222 
#define BUFFER_SIZE 1024 
#define THD -30

struct cliprobeData {
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

int main(int argc,char*argv[])
{
	struct selMacRssi
	{
		unsigned char selMumac[6];          //发射信号MU的MAC地址
		char selRssi;                       //测量到的以dBm为单位的RSSI 整数值 十进制：-128 到127
	}sel[100];//定义结构体数组 一分钟100应该足够多了

	//文本类使用getchar(),openCV使用waitkey,当获取键盘按键时采用通用的GetKeyState(), system("pause")也是一个很好的选择
	/*加载winsock文件*/
	struct cliprobeData *packageData;
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp;

	//定义两个特定mac地址的存储变量：
	char myRssi=0;
	//unsigned char Mymac1[6]={176,226,53,43,218,224};//我的mac地址
	//unsigned char Mymac2[6]={240,37,183,193,7,151};//姜华的mac地址
	//getSpecialMac(Mymac1);//得到mac的数据，可选
	//getSpecialRssi(myRssi);
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
		return 1;
	}
	//创建嵌套字
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//绑定嵌套字
	sockaddr_in servAddr;//服务器地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 

	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	//文件存储可以到时候取消
	fp=fopen("text.csv","a+");
	//接收数据
	while(1){

		/* 定义一个地址，用于捕获客户端地址 */
		sockaddr_in clientAddr; 
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
		packageData = (struct cliprobeData *) buffer;//格式化数据 
		//只记录对应的mac码
		//kinect 获取深度得到一个判断条件,否则不对mac码进行计数
		//如果当获取的人的最近区域深度小于1米时，开始存储阈值内的探针数据，开始就拍下照片，
		//当人离开时，则统计数据的结果，屏幕输出最有可能的Mac码，以及次选mac码，次选mac码可有可无输，屏幕输出一个匹配结果，
		//数据不再存储，或记录下一个人。
		//实现时可以采用bodyIndex 下深度，并选择特定区域的深度值。

		//处理深度区域

		//要完成探测特定区域深度的功能，从而给探针提供使能端

		if(1){
		///
		if(int(packageData->Rssi)>THD){	//设置阈值
			printf("匹配到的最有可能的mac码：\n");
			printf("%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			cout<<int(packageData->Rssi)<<endl;
			
            //存到文件里
			//文件存储可以到时候取消
			for (int i=0;i<14;i++) 
			{
				fprintf(fp,"%c", packageData->Timestamp[i]);
			}
			fprintf(fp,",");
			fprintf(fp,"%d,", packageData->Rssi);
			fprintf(fp,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			fprintf(fp,"\n");

			//存到结构体数组	


			//当探针数据无法保存后，要对数据进行处理，即统计计算数据结果
	}
		}
		//exit
		if( GetKeyState( VK_ESCAPE ) < 0 ){
			closesocket(s);
			::WSACleanup();
			fclose(fp);
			exit(0);
		}
	}

}
