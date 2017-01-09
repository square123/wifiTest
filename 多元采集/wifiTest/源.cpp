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

#define SERVER_PORT 2222 

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

int main(int argc,char*argv[])
{
	/*加载winsock文件*/
	struct cliprobeData *packageData;
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp1,*fp2;


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
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	//接收数据
	while(1){
#define BUFFER_SIZE 1024 
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
		//输出数据
		//time_t t = time( 0 );
		//char tmp[64];//定义时间
		//strftime( tmp, sizeof(tmp), "%Y/%m/%d %X \n\t", localtime(&t) );//提取系统时间
		
		char * ipaddr=NULL;
		char addr[20];
		ipaddr= inet_ntoa(clientAddr.sin_addr);//获取IP地址
		strcpy(addr,ipaddr);  
		printf("%s\n",ipaddr);		
		packageData = (struct cliprobeData *) buffer;//格式化数据 
		//printf("得到的探针数据：\n");
		//printf("源AP的MAC地址:%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Apmac[0], packageData->Apmac[1], packageData->Apmac[2], \
		//	packageData->Apmac[3], packageData->Apmac[4], packageData->Apmac[5]);
		//////printf("无线电类型:%02X\n", packageData->Radiotype);
		//printf("信道:%d\n", packageData->Channel);
		//printf("时间戳:");
		///*   for (int i=0;i<14;i++) {
		//printf("%c", packageData->Timestamp[i]);
		//}*/
		//printf("%c%c%c%c-%c%c-%c %c:%c%c:%c%c",packageData->Timestamp[0],packageData->Timestamp[1],packageData->Timestamp[2],packageData->Timestamp[3],\
		//	packageData->Timestamp[4],packageData->Timestamp[5],packageData->Timestamp[6],packageData->Timestamp[7],\
		//	packageData->Timestamp[8],packageData->Timestamp[9],packageData->Timestamp[10],packageData->Timestamp[11]);
		//printf("\n");
		//printf("客户端MAC地址:%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
		//	packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
		//printf("Rssi值:%d\n", packageData->Rssi);
	/*	printf("Noise值：%d\n",packageData->Noisefloor);*/

		//printf("\n");
		//ofstream myfile("time.csv",ios::app);  //文件输出
		//if(!myfile)
		//{
		//	cout<<"error !";
		//}
		//else
		//{
		//	//myfile<<tmp<<endl;//去掉一些没用的时间
		//	myfile<<packageData->Timestamp[7]<<":"<<packageData->Timestamp[8]<<packageData->Timestamp[9]<<":"<<packageData->Timestamp[10]<<packageData->Timestamp[11]<<",";
		//	myfile<<"02X"<<hex<<packageData->Mumac[0]<<":"<<"02X"<<hex<<packageData->Mumac[1]<<":"<<"02X"<<hex<<packageData->Mumac[2]<<":"<<"02X"<<hex<<packageData->Mumac[3]<<":"<<"02X"<<hex<<packageData->Mumac[4]<<":"<<"02X"<<hex<<packageData->Mumac[5]<<",";
		//	myfile<<packageData->Rssi<<endl;
		//	
		//}

		//if((fp=fopen("text.csv","a+")) == NULL)  
		//{  
		//	printf("file open failed!");  
		//	return 0;  
		//}  
		//else
		//{

		//	/*fprintf(fp,"%c%c%c%c-%c%c-%c %c:%c%c:%c%c,",packageData->Timestamp[0],packageData->Timestamp[1],packageData->Timestamp[2],packageData->Timestamp[3],\
		//	packageData->Timestamp[4],packageData->Timestamp[5],packageData->Timestamp[6],packageData->Timestamp[7],\
		//	packageData->Timestamp[8],packageData->Timestamp[9],packageData->Timestamp[10],packageData->Timestamp[11]);*/

		//}
		//exit
		if(addr[10]=='1')
		{
			for (int i=0;i<14;i++) {
				fprintf(fp1,"%c", packageData->Timestamp[i]);
			}
			fprintf(fp1,",");
			fprintf(fp1,"%d,", packageData->Rssi);
			fprintf(fp1,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			fprintf(fp1,"\n");
		}
		else if(addr[10]=='2')
		{
			for (int i=0;i<14;i++) {
				fprintf(fp2,"%c", packageData->Timestamp[i]);
			}
			fprintf(fp2,",");
			fprintf(fp2,"%d,", packageData->Rssi);
			fprintf(fp2,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			fprintf(fp2,"\n");
		}
		if( GetKeyState( VK_ESCAPE ) < 0 ){
			closesocket(s);
			::WSACleanup();
			/*myfile.close();*/
			fclose(fp1);
			fclose(fp2);
			exit(0);
		}
	}  

}
