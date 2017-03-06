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
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp1,*fp2,*fpSysc;
	//FILE	*fp3;
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; 
	unsigned char zeroMac[6];
	char zeroTimestamp[14];
	char zeroRssi;
	Probe();
	~Probe();
	void InitProbe();
	void probeProcess();
	void probeTimeFix(char src[],char dst[14] );//用来存储修复后的时间数据格式，根据系统时间来完成修正，因此需要在使用时要同步
	void rssiPurify(char time[14],FILE *f,int index);//进行同一时间的RSSI优选
	void probeSysc(int baseIndex,FILE *f);//处理同步结构体的函数
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
	bool timeCompare(char time1[14],char time2[],int delta);//返回时间两个时间是否相差delta秒
	time_t charToTimeInt(char ttt[14]);//字符串转换成时间int
	struct syscProbed
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi[ProbeNum];
	}syscResult[sameTimeMacNum];//用来输出同步后的结果

	//把实现函数过程的全局变量封装在private下
private:	
	//优选部分
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
	time_t syscTime;	//系统时间
	int storeIndex;//存储数据的索引
	int processIndex;//处理同步的索引
	bool saveFinshFlag[ProbeNum];
	struct syscProbe	//同步多探针的结构体	
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi;
	}syscStr[5][ProbeNum][sameTimeMacNum];//三维结构体数组，要寄存的时间、探针数量、每秒钟存储的数据，3代表缓冲池的放的数据
	time_t selectSysPrbTime(syscProbe sysc[sameTimeMacNum]);//输出结构体时间的函数
	void reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum]);//将结构体数据紧凑
	void AllreduceSyscProbe(syscProbe Allsysc[5][ProbeNum][sameTimeMacNum],syscProbe Allsysced[5][ProbeNum][sameTimeMacNum]);//将整个结构体数组缩减
	syscProbe zeroSysc;//定义存储全零为syscProbe格式
	//char timePool[3][14];//用于显示要处理的时间池
};

Probe::Probe()
{
	storeIndex=0;//要更新的数据存储索引，直接从索引0开始存储
	processIndex=3;//要使基准组的索引号，从2开始
	memset(zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的mac码量
	memset(zeroTimestamp,0,sizeof(char)*14);//定义一个全为零的时间戳
	//memset(&zeroRssi,0,sizeof(char));//定义一个全为零的RSSI
	zeroRssi=0;
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//输出变量初始化
	memset(syscStr,0,sizeof(syscProbe)*sameTimeMacNum*5*ProbeNum);
	memset(&zeroSysc,0,sizeof(syscProbe));//定义存储全零为syscProbe格式
	//memset(timePool,0,sizeof(char)*3*14);
	for(int i=0;i<ProbeNum;i++)
	{
		memset(sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//结构体初始化
		memset(timeTemp[i],0,sizeof(char)*14);//时间初始化
		indexForPure[i]=0;//索引初始化
		flag[i]=false;//标志位初始化
		saveFinshFlag[i]=0;//存储标志位初始化
		//for (int j=0;j<3;j++)//同步存储变量初始化
		//{
		//	memset(&syscStr[j][i],0,sizeof(syscProbe)*sameTimeMacNum);
		//}
	}
	syscTime=0;
}

Probe::~Probe()
{
	closesocket(s);
	::WSACleanup();
	fclose(fp1);
	fclose(fp2);
	fclose(fpSysc);
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
	fpSysc=fopen("probeSysc.csv","a+");
}


void Probe::probeTimeFix(char src[],char dst[14] ) //用来存储修复后的时间数据格式，根据系统时间来完成修正，因此需要在使用时要同步
{
	time_t tt=time(NULL);
	tm* t=localtime(&tt);
	char endPos=0;
	int endIndex=0;
	for (int k=0;k<14;k++)//输出最后一位
	{
		if (src[k]==0)
		{
			endPos=src[k-1];
			endIndex=k-1;
			break;
		}
	}
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
	if ((dst[13]==0))
	{
		dst[13]=dst[12];
		dst[12]='0';
	}
	//该函数有问题
	//再加入一个校验，当后面的时间不同时
	if (endPos!=dst[13])
	{
		dst[13]=src[endIndex];
		dst[12]=src[endIndex-1];
	}
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
	else //当时间不一致的时候要输出数据，并且要把第一个不同的数据存下来，并将时间记录下来
	{    //输出数据到文件
		if(flag[index]==true)//必须先存到数据才执行写数据
		{
			//清空storeIndex下同步结构体的数据
			memset(syscStr[storeIndex][index],0,sizeof(syscProbe)*sameTimeMacNum);//将同步结构体index索引下的数据置为零
			//
			bool douflag=timeCompare(timeBuffer[index],timeTemp[index],1);//写在外面减少运算量
			for(int j=0;j<indexForPure[index];j++)//使用这个索引号减少运算
			{
				//给数据加入一个Buffer，可以对数据进行筛选，不用全部存，只要把MAC存入应该就行
				//添加一个触发条件，即时间间隔必须为一秒，才进行是否重复的判断
				//减少邻秒输出的判断部分
				if (douflag)//已经出现的Mac码不再在第二秒输出，后面的数据不管了，因为已经输出了
				{
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
						continue;
					}
				}
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
				//探针同步存储部分
				memcpy(syscStr[storeIndex][index][j].Timestamp,timeTemp[index],sizeof(char)*14);
				memcpy(syscStr[storeIndex][index][j].selMumac,sel[index][j].selMumac,sizeof(unsigned char)*6);
				syscStr[storeIndex][index][j].Rssi=sel[index][j].maxRssi;
				//
			}
			//将上次的数据存入buffer中，为可能的下一秒备用，注意这里是完整保存的，所以，当连续三秒都出现时，其实只记录第一次出现的值
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//记录已经输出的文件下回可以参考的量
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			//
			flag[index]=false;//重新置为否，只运行一次
			memset(sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//将结构体重置为零
			indexForPure[index]=0;//重置
			saveFinshFlag[index]=1;
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
	//socket通信绑定部分
	bool enable=true;
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//bool syscZeroEnable=true;//记录系统时间使能端
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
	//printf("%s\n",ipaddr);		//显示接收信号的地址
	packageData = (struct cliprobeData *) buffer;//格式化数据 
	char retime[14]="";//初始化数据为空
	probeTimeFix(packageData->Timestamp,retime);//修复时间戳格式，使其变成14位
	//printf("修复格式前的时间：\n");
	//for (int ii=0;ii<14;ii++)
	//{
	//	printf("%c",packageData->Timestamp[ii]);
	//}
	//printf("\n");
	//printf("修复格式后的时间：\n");
	//for (int ii=0;ii<14;ii++)
	//{
	//	printf("%c",retime[ii]);
	//}
	//printf("\n");
	//当探针时间是同步的时候，其返回的时间是顺序排列的，不会超前传输，因此，就只记录每次时间存储的变化量，
	//相当于存储同一时间间隔的几个表，但是，只比较中间的那个，后面的不考虑，把时间的比较放在结构体中，因为探针返回的时间并不是连续的
	//需要每有一次时间变化就记录一次，将时间间隔记录下来，设置一个函数，用来处理结构体，且能设置以谁为基准
	//由于要方便清零，所以要设置结构体，可以直接通过索引来直接来清零
	//实现该功能为两部分，一部分在RSSI优选上该部分负责存储数据结构体，另一部分在process函数中，通过时间变化来对数据进行操作，该部分需要由基准和比较组成
	//设置3的目的，是为了克服探针一秒的误差，因为手机是发射体，电磁波的传输速度很快，近距离下三个探针接受到的信号一定是同步

	if(int(packageData->Rssi)>THD)//测试下阈值比较合适
	{	
	/*	for (int ii=0;ii<14;ii++) 
		{
			printf("%c", retime[ii]);
		}
		printf("\n");*/
		cout<<charToTimeInt(retime)<<endl;
		//应该在外面完成数据的统一合并
		
		if(addr[10]=='1')//定义的是地址
		{
			cout<<"探针1的数据"<<endl;
			rssiPurify(retime,fp1,0);
		}
		else if(addr[10]=='2')
		{
			
			cout<<"探针2的数据"<<endl;
			rssiPurify(retime,fp2,1);
		}
		//该部分暂时没用
		for (int mm=0;mm<ProbeNum;mm++)//必须要让两个都存储完///！！！！！！！！！！！！！！！！！！需要验证，寒假回来再说
		{
			enable=enable&&saveFinshFlag[mm];
		}
		if (enable)//输出时间不同时，将标志位保存下(syscTime<charToTimeInt(retime))&&
		{
			syscTime=charToTimeInt(retime);
			cout<<"同步了因为"<<syscTime<<endl;
			/*	printf("要同步的时间池: ");
			for (int man=0;man<3;man++)
			{
			for (int ii=0;ii<14;ii++) 
			{
			printf("%c", timePool[man][ii]);
			}
			printf("  ");
			}
			printf("\n");
			printf("要处理的时间： ");
			for (int ii=0;ii<14;ii++) 
			{
			printf("%c", timePool[processIndex][ii]);
			}
			printf("\n");
			memcpy(timePool[storeIndex],retime,sizeof(char)*14);*/
			probeSysc(0,fpSysc);//同步处理函数
			for (int mm=0;mm<ProbeNum;mm++)//处理完后重新置0///感觉还是有问题~~~~会出现数据大面积丢失的问题，应该还需要修改，修改在break地方上，或者两个时间域的判断上
			{
				saveFinshFlag[mm]=0;
			}
			enable=1;//重新重置
			storeIndex=(storeIndex+1)%5;//存储索引加1
			processIndex=(processIndex+1)%5;//处理索引加1
			
		}
	}
}
		
void Probe::probeSysc(int baseIndex,FILE *f)//探针同步函数,将同步后的数据存储到一个新的表中 数据有丢失怎么解决？？？
{
	//初始化部分
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//用于多探针集合表格式，清空

	syscProbe syscStrEd[5][ProbeNum][sameTimeMacNum];//定义用于存储紧凑后的变量
	memset(syscStrEd,0,(sizeof(syscProbe)*5*ProbeNum*sameTimeMacNum));//清空&初始化

	AllreduceSyscProbe(syscStr,syscStrEd);//通过该函数将数据紧凑一些
	int jianshao=0;
	//判断部分
	for (int m=0;m<sameTimeMacNum;m++)//该循环是将baseIndex探针中processIndex时间下的结构体数据与其他索引进行比较
	{
		if (memcmp(&syscStrEd[processIndex][baseIndex][m],&zeroSysc,sizeof(syscProbe))==0)//为空跳出，减少运算
		{
			break;
		}	
		jianshao++;
		//先把变量存下来
		memcpy(syscResult[m].Timestamp,syscStrEd[processIndex][baseIndex][m].Timestamp,sizeof(char)*14);
		memcpy(syscResult[m].selMumac,syscStrEd[processIndex][baseIndex][m].selMumac,sizeof(unsigned char)*6);
		syscResult[m].Rssi[baseIndex]=syscStrEd[processIndex][baseIndex][m].Rssi;
		for(int n=0;n<ProbeNum;n++)//该循环是剔除baseIndex的索引，方便与其他探针比较,n表示探针索引
		{
			if(n==baseIndex)//跳出基准索引
			{
				continue;
			}	
			for (int k=0;k<5;k++)//遍历时间,这里做了一点小改动，希望从最早的时间开始记录。就将k变成((processIndex+2+k)%3)，默认最先到的最好、、、、、！！！！需要验证
			{	
				bool skip=false;
				if (!(timeCompare(syscStrEd[((processIndex+2+k)%5)][n][0].Timestamp,syscStrEd[processIndex][baseIndex][0].Timestamp,1)))//当时间不合格时，直接跳出,因为一块的时间是相同的，所以用0就可以
				{
					continue;//通过该判断去除时间不合格的变量
				}
				for (int l=0;l<sameTimeMacNum;l++)//遍历一个时间块下, l表示块内存储索引
				{	
					if (memcmp(&syscStrEd[((processIndex+2+k)%5)][n][l],&zeroSysc,sizeof(syscProbe))==0)
					{
						break;//因为已经让数据紧凑了，所以可以直接使用break
					}
					if(memcmp(syscStrEd[processIndex][baseIndex][m].selMumac,syscStrEd[((processIndex+2+k)%5)][n][l].selMumac,sizeof(unsigned char)*6)==0)//如果匹配就把RSSI值导入
					{
						syscResult[m].Rssi[n]=syscStrEd[((processIndex+2+k)%5)][n][l].Rssi;
						skip=true;
						break;
					}
				}
				if (skip)//减少运算量，要是检测到一个值，就直接跳出这个时间
				{
					break;
				}
			}
		}
	}
	//输出部分，检测均有值才输出
	for (int q=0;q<jianshao;q++)
	{
		bool outflag=1;
		outflag=outflag&&memcmp(syscResult[q].selMumac,zeroMac,sizeof(unsigned char)*6);//有一个就好
		for (int r=0;r<ProbeNum;r++)
		{
			outflag=outflag&&memcmp(&syscResult[q].Rssi[r],&zeroRssi,sizeof(char));
		}
		if(outflag)
		{
			for (int ii=0;ii<14;ii++) 
			{
				fprintf(fpSysc,"%c", syscResult[q].Timestamp[ii]);
			}
			fprintf(fpSysc,",");
			fprintf(fpSysc,"%02X:%02X:%02X:%02X:%02X:%02X,",syscResult[q].selMumac[0], syscResult[q].selMumac[1], syscResult[q].selMumac[2], \
				syscResult[q].selMumac[3], syscResult[q].selMumac[4], syscResult[q].selMumac[5]);
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpSysc,"%d", syscResult[q].Rssi[r]);
				if(r!=ProbeNum-1)//最后一个不输出逗号
				{
					fprintf(fpSysc,",");
				}
			}
			fprintf(fpSysc,"\n");
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
	timeInt2=charToTimeInt(time2);
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

time_t Probe::selectSysPrbTime(syscProbe sysc[sameTimeMacNum])//输出syscProbe结构体的时间，好像弄的比较复杂了
{
	time_t timeout;
	for (int i=0;i<sameTimeMacNum;i++)
	{
		if (memcmp(sysc[i].Timestamp,zeroTimestamp,sizeof(char)*14)==0)
		{
			continue;
		}else
		{
			timeout=charToTimeInt(sysc[i].Timestamp);
			break;
		}
	}
	return timeout;
}

void Probe::reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum])
{
	memset(sysced,0,sizeof(syscProbe)*sameTimeMacNum);//将输出结构体数组置零
	int j=0;
	for (int i=0;i<sameTimeMacNum;i++)
	{
		if (memcmp(&zeroSysc,&sysc[i],sizeof(syscProbe))!=0)//如果不为空，就保存在新的数据集合中
		{
			//memcpy(&sysced[j],&sysc[i],sizeof(syscProbe));//这句写的不对，不能取地址
			memcpy(sysced[j].selMumac,sysc[i].selMumac,sizeof(unsigned char)*6);
			memcpy(sysced[j].Timestamp,sysc[i].Timestamp,sizeof(char)*14);
			sysced[j].Rssi=sysc[i].Rssi;
			j++;
		}
	}
}

void Probe::AllreduceSyscProbe(syscProbe Allsysc[5][ProbeNum][sameTimeMacNum],syscProbe Allsysced[5][ProbeNum][sameTimeMacNum])
{
	for (int i=0;i<5;i++)
	{
		for (int j=0;j<ProbeNum;j++)
		{
			reduceSyscProbe(Allsysc[i][j],Allsysced[i][j]);
		}
	}
}

int main(int argc,char*argv[])
{
	Probe probe;
	probe.InitProbe();
	while(1)
	{
		probe.probeProcess();
		if( GetKeyState( VK_ESCAPE ) < 0 )
		{
			probe.~Probe();
		}
	}  
}

//memset,memcpy.memcmp,bug总结，数组类的可以不取地址，因为数组本身就是指针，可以传递，非数组类，结构体必须要取地址，才能进行地址传递


//等数据最终合理后，并且把C++重新过一遍后，一定要抽时间把程序重新写一遍，减少数组的使用。