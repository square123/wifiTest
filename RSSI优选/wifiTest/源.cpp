#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
#include<time.h>//加入时间
#include <fstream>//希望输入时间
#include<math.h>
#pragma comment(lib,"WS2_32.lib")

using namespace std;
#define sameTimeMacNum 10 //表示同一时刻内探针默认最多存储的数据类型数
#define ProbeNum 2 //表示一共有几个探针返回数据
#define SERVER_PORT 2222 
#define BUFFER_SIZE 1024 
#define THD -80

class Probe
{
public:
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
	struct cliprobeData *packageData;
	struct rssiTemp
	{
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][sameTimeMacNum];
	rssiTemp selBuffer[ProbeNum][sameTimeMacNum];//是上一变量的寄存器
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp1,*fp2;
	//FILE	*fp3;
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; 
	unsigned char zeroMac[6];
	Probe();
	~Probe();
	void InitProbe();
	void probeProcess();
	void probeTimeFix(char src[],char dst[14] );//用来存储修复后的时间数据格式，根据系统时间来完成修正，因此需要在使用时要同步
	void rssiPurify(char time[14],FILE *f,int index);//进行同一时间的RSSI优选
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
	bool timeCompare(char time1[14],char time2[],int delta);//返回时间两个时间是否相差delta秒
	time_t charToTimeInt(char ttt[14]);//字符串转换成时间int


private:
	char timeTemp[ProbeNum][14];//用来区分时间变化的变量
	char timeBuffer[ProbeNum][14];//时间的寄存器，用来存储上上次的时间
	int indexForPure[ProbeNum];
	int indexForPureBuf[ProbeNum];//作为上一变量的寄存器
	bool flag[ProbeNum];
};

Probe::Probe()
{
	memset(&zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的量
	for(int i=0;i<ProbeNum;i++)
	{
		memset(&sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//结构体初始化
		memset(&timeTemp[i],0,sizeof(char)*14);//时间初始化
		indexForPure[i]=0;//索引初始化
		flag[i]=false;//标志位初始化
	}
}

Probe::~Probe()
{
	closesocket(s);
	::WSACleanup();
	fclose(fp1);
	fclose(fp2);
	//fclose(fp3);
	exit(0);
}

void Probe::InitProbe()
{
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
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	//fp3=fopen("prb2.csv","a+");
}


void Probe::probeTimeFix(char src[],char dst[14] ) //用来存储修复后的时间数据格式，根据系统时间来完成修正，因此需要在使用时要同步
{
	time_t tt=time(NULL);
	tm* t=localtime(&tt);
	/*printf("%d-%02d-%02d %02d:%02d:%02d\n", t->tm_year + 1900,t->tm_mon + 1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);*/
	dst[0]=src[0];
	dst[1]=src[1];
	dst[2]=src[2];
	dst[3]=src[3];
	int i=4,j=4;
	if((t->tm_mon + 1)<10)//月
	{
		dst[j]='0';
		j++;
	}
	else
	{
		dst[j]=src[i];	
		i++;
		j++;
	}
	dst[j]=src[i];
	i++;
	j++;
	if((t->tm_mday)<10) //天
	{
		dst[j]='0';
		j++;
	}
	else
	{
		dst[j]=src[i];	
		i++;
		j++;
	}
	dst[j]=src[i];	
	i++;
	j++;
	if((t->tm_hour)<10) //小时
	{
		dst[j]='0';
		j++;
	}
	else
	{
		dst[j]=src[i];	
		i++;
		j++;
	}
	dst[j]=src[i];	
	i++;
	j++;
	if((t->tm_min)<10) //分
	{
		dst[j]='0';
		j++;
	}
	else
	{
		dst[j]=src[i];	
		i++;
		j++;
	}
	dst[j]=src[i];	
	i++;
	j++;
	if((t->tm_sec)<10) //秒
	{
		dst[j]='0';
		j++;
	}
	else
	{
		dst[j]=src[i];	
		i++;
		j++;
	}
	dst[j]=src[i];	
	//加入校验，因为可能存在在读秒时，电脑比探针快一点
	//这里只对最后一位进行判断，因为时间跟不上只有可能是在"秒"级，"分"级不可能
	if (dst[13]==0)
	{
		dst[13]=dst[12];
		dst[12]='0';
	}
	///////
	//输出部分
	//for (int i=0;i<14;i++) {
	//	printf("%c", dst[i]);
	//}
	//printf("\n");
}

void Probe::rssiPurify(char time[14],FILE *f,int index)//进行同一时间的RSSI优选,即合并同一时间并选出最大的RSSI
{
	if(memcmp(timeTemp[index],time,sizeof(char)*14)==0)//时间一致时
	{
		for(int i=0;i<sameTimeMacNum;i++)
		{
			if(memcmp(sel[index][i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//当检测到区域为零时，把测得值赋值给该数组，这个是增加种类的部分
			{
				memcpy(sel[index][i].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
				sel[index][i].maxRssi=packageData->Rssi;
				indexForPure[index]++;
				break;//跳出循环
			}
			else if(memcmp(packageData->Mumac,sel[index][i].selMumac,sizeof(unsigned char)*6)==0)//比较是否出现这样的组合,memcmp 相同返回0,不同返回非零
			{
				sel[index][i].maxRssi=MaxRssi(sel[index][i].maxRssi,packageData->Rssi);
				break;//跳出循环
			}
		}
	}
	else //当时间不一致的时候要输出数据，并且要把第一个不同的数据存下来，并且将时间记录下来
	{//输出数据到文件
		if(flag[index]==true)//必须先存到数据才执行写数据
		{
			bool douflag=timeCompare(timeBuffer[index],timeTemp[index],1);//写在外面减少运算量
			
			for(int j=0;j<indexForPure[index];j++)//使用这个索引号减少运算
			{
				//给数据加入一个Buffer，可以对数据进行筛选，不用全部存，只要把MAC存入应该就行
				//添加一个触发条件，即时间间隔必须为一秒，才进行是否重复的判断
				if (douflag)//当出现已经出现的Mac码直接不再在第二秒输出，后面的数据不管了，因为已经输出了
				{
					//cout<<"时间间隔为1秒"<<endl;
					bool skipflag=false;
					for (int k=0;k<indexForPureBuf[index];k++)
					{
						if (memcmp(sel[index][j].selMumac,selBuffer[index][k].selMumac,sizeof(unsigned char)*6)==0)//将要输出的分别与上一秒的数据进行比较
						{
							skipflag=true;//如果有重复的，就输出flag,并跳出for循环
							//cout<<"有重复"<<endl;
							break;
						}
					}
					if (skipflag==true)//如果有重复的，就跳出这个环节，比较下一个Mac码
					{
						//cout<<"应该少了一个"<<endl;
						continue;
					}
				}
				//显示测试部分
				/*cout<<"要输出的时间："<<charToTimeInt(timeTemp[index])<<",";
				for (int ii=0;ii<14;ii++) 
				{
					printf("%c", timeTemp[index][ii]);
				}
				printf(",");
				printf("%02X:%02X:%02X:%02X:%02X:%02X,", sel[index][j].selMumac[0], sel[index][j].selMumac[1], sel[index][j].selMumac[2], \
					sel[index][j].selMumac[3], sel[index][j].selMumac[4], sel[index][j].selMumac[5]);
				printf("%d", sel[index][j].maxRssi);
				printf("\n");*/
				//输出文件部分
				for (int ii=0;ii<14;ii++) 
				{
					fprintf(f,"%c", timeTemp[index][ii]);
				}
				fprintf(f,",");
				fprintf(f,"%02X:%02X:%02X:%02X:%02X:%02X,", sel[index][j].selMumac[0], sel[index][j].selMumac[1], sel[index][j].selMumac[2], \
					sel[index][j].selMumac[3], sel[index][j].selMumac[4], sel[index][j].selMumac[5]);
				fprintf(f,"%d", sel[index][j].maxRssi);
				fprintf(f,"\n");
				
			}
			//将上次的数据存入buffer中，为可能的下一秒备用，注意这里是完整保存的，所以，当连续三秒都出现时，其实只记录第一次出现的值
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//记录已经输出的文件下回可以参考的量
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			//
			flag[index]=false;//重新置为否，只运行一次
			memset(&sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//将结构体重置为零
			indexForPure[index]=0;//重置
		}
		//不加这个就把第一个数据给漏掉了,并且当数据发生变化输出到文件也会丢掉数据	
		memcpy(sel[index][0].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
		sel[index][0].maxRssi=packageData->Rssi;
		indexForPure[index]++;
		flag[index]=true;
		memcpy(timeTemp[index],time,sizeof(char)*14);//把变化的时间赋值给timeTemp
	}
}

void Probe::probeProcess()
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
	char * ipaddr=NULL;
	char addr[20];
	ipaddr= inet_ntoa(clientAddr.sin_addr);//获取IP地址
	strcpy(addr,ipaddr);  
	printf("%s\n",ipaddr);		//显示接收信号的地址
	packageData = (struct cliprobeData *) buffer;//格式化数据 
	char retime[14]="";//初始化数据为空
	probeTimeFix(packageData->Timestamp,retime);//修复时间戳格式，使其变成14位
	if(int(packageData->Rssi)>THD){
		//应该在外面完成数据的统一合并
		if(addr[10]=='1')//定义的是地址
		{
			//for (int i=0;i<14;i++) 
			//{
			//	fprintf(fp1,"%c", retime[i]);
			//}
			//fprintf(fp1,",");
			//fprintf(fp1,"%d,", packageData->Rssi);
			//fprintf(fp1,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
			//	packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			//fprintf(fp1,"\n");
			
			rssiPurify(retime,fp1,0);
		}
		else if(addr[10]=='2')
		{
			/*for (int i=0;i<14;i++)
			{
			fprintf(fp3,"%c", retime[i]);
			}
			fprintf(fp3,",");
			fprintf(fp3,"%d,", packageData->Rssi);
			fprintf(fp3,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
			packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			fprintf(fp3,"\n");*/
			
			rssiPurify(retime,fp2,1);
		}
	}
}

char Probe::MaxRssi(char rssi1,char rssi2)//返回较大的RSSI值，且该值不能能等于0
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

bool Probe::timeCompare(char time1[14],char time2[],int delta)//由于时间是进制类的，所以最简单的方法是直接转换到秒去判断间隔
{
	time_t timeInt1,timeInt2;
	timeInt1=charToTimeInt(time1);
	//cout<<"已输出到文件的时间:"<<timeInt1<<endl;
	timeInt2=charToTimeInt(time2);
	//cout<<"现在即将输出的时间:"<<timeInt2<<endl;
	if ((abs(timeInt1-timeInt2)<=delta))
	{
		return true;
	} 
	else
	{
		return false;
	}
}

time_t Probe::charToTimeInt(char ttt[14])
{
	time_t timeInt1;
	char year1[4],mon1[2],day1[2],hour1[2],min1[2],second1[2];
	memcpy(year1,ttt,sizeof(char)*4);//把时间1分开
	memcpy(mon1,ttt+4,sizeof(char)*2);
	memcpy(day1,ttt+6,sizeof(char)*2);
	memcpy(hour1,ttt+8,sizeof(char)*2);
	memcpy(min1,ttt+10,sizeof(char)*2);
	memcpy(second1,ttt+12,sizeof(char)*2);
	struct tm tt1;
	memset(&tt1,0,sizeof(tt1));
	tt1.tm_year=atoi(year1)-1900;    //atoi函数是将字符串转换成数字类型，早知有这个函数就不用自己写了
	tt1.tm_mon=atoi(mon1)-1;    
	tt1.tm_mday=atoi(day1);    
	tt1.tm_hour=atoi(hour1);    
	tt1.tm_min=atoi(min1);    
	tt1.tm_sec=atoi(second1);    
	timeInt1=mktime(&tt1); 
	return timeInt1;
}
int main(int argc,char*argv[])//以后写程序都要向类的方式去靠，便于修改和滤清思路。
{
	Probe probe;
	probe.InitProbe();
	while(1){
		probe.probeProcess();
		if( GetKeyState( VK_ESCAPE ) < 0 )
		{
			probe.~Probe();
		}
	}  
}

