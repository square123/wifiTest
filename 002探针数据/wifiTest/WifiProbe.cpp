#include "WifiProbe.h"

Probe::Probe()//数据初始化
{
	memset(zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的mac码量
	memset(zeroTimestamp,0,sizeof(char)*14);//定义一个全为零的时间戳
	memset(sel,0,sizeof(rssiTemp)*ProbeNum*60*sameTimeMacNum);
	memset(seled,0,sizeof(rssiMiss)*60*sameTimeMacNum);
	memset(rssiTempIndex,0,sizeof(int)*ProbeNum*60);
	memset(rssiMissIndex,0,sizeof(int)*60);
	memset(detProbeTime,0,sizeof(int)*ProbeNum);
	memset(syscTimeBuff,0,sizeof(int)*ProbeNum);
	zeroRssi=0;
	processGetIndex=0;//先暂时这么定义
	processNotIndex=0;//先暂时这么定义
}

Probe::~Probe()//用来关闭文件和socket接口
{
	closesocket(s);
	::WSACleanup();
	fclose(fpNot);
	fclose(fpGet);
	exit(0);
}

void Probe::InitProbe()//初始化
{
	//socket通信部分
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
	//可选 Mac厂商查找部分
	Csv csv("精简版手机品牌对应表.csv");//该品牌对应表不是很全，当时有删掉的部分，下次可以只把主要手机的Mac地址记录下来即可。
	for (auto i=csv.table.begin();i!=csv.table.end();i++)
	{
		auto i1=i->begin();
		auto i2=i->begin()+1;
		mobileManu[*i1]=*i2;
	}
	//文件创建部分
	fpGet=fopen("probeGet.csv","a+");
	fpNot=fopen("probeNot.csv","a+");
	time_t syscTimee=time(NULL);	//这里只执行一次，后续就不需要再操作了
	processGetIndex=syscTimee+4;//因为是要滞后处理，所以要将处理的标志位滞后，具体的滞后时间参数可以修改
	processNotIndex=syscTimee+6;
}

void Probe::mobileManuOutput(mncatsWifi &Probedata)//输出手机网卡的对应厂商
{
	std::string mac;
	mac=Probedata.mac2.substr(0,2)+Probedata.mac2.substr(3,2)+Probedata.mac2.substr(6,2);
	auto itt = mobileManu.find(mac);
	if (itt != mobileManu.end())
	{
		std::cout<<itt->second;
	}
	else
	{
		std::cout<<"其他品牌";
	}
}


void Probe::timesSysc(time_t &syscTime,mncatsWifi &Probedata)//同步多探针的时间
{
	int index=0;
	if(Probedata.mac1=="C8:E7:D8:D4:A3:75")
	{
		index=0;
	}
	else if(Probedata.mac1=="C8:E7:D8:D4:A3:02")
	{		
		index=1;
	}
	else if(Probedata.mac1=="C8:E7:D8:D4:A3:60")
	{
		index=2;
	}

	if (abs(syscTime-syscTimeBuff[index])>3600)//设置两个小时完成一次同步
	{
		syscTimeBuff[index]=syscTime;
		detProbeTime[index]=syscTime-charToTimeInt(Probedata.Timestamp);//因为探针的时钟走的较慢，所以需要进行调整。
	}
}

void Probe::probeProcess()
{
	//socket通信绑定部分
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
	time_t syscTime=time(NULL);	//记录系统时间，用来将其触发时间同步的机制
	mncatsWifi datatemp=mncatsWifi(buffer);//格式化数据
	timesSysc(syscTime,datatemp);//负责探针的时间同步
	if ((datatemp.dtype!="80")&&(int(datatemp.crssi)>THD))//在这里设置权重，并且去除80beacon帧  
	{
		char timeFix[16];//用来修复时间,以后如果出问题可以考虑下是否是这个原因
		time_t timeFixtt=0;
		if(datatemp.mac1=="C8:E7:D8:D4:A3:75")//定义的是地址
		{
			std::cout<<"探针1的数据"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[0];//localtime只是用来转换格式,它并没有获取系统时间的功能，它和gmtime相对。
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
			rssiIntegrate(timeFix,datatemp,0);
		}
		else if(datatemp.mac1=="C8:E7:D8:D4:A3:02")
		{		
			std::cout<<"探针2的数据"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[1];
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
			rssiIntegrate(timeFix,datatemp,1);
		}
		else if(datatemp.mac1=="C8:E7:D8:D4:A3:60")
		{
			std::cout<<"探针3的数据"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[2];
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
			rssiIntegrate(timeFix,datatemp,2);
		}
		if (syscTime>=processGetIndex)//进行插空操作 最终达到循环操作,改成大于等于是因为数据有可能存在跳秒
		{
			rssiMissGet();
			processGetIndex++;
		}
		if (syscTime>=processNotIndex)//进行补空操作
		{
			rssiMissNot();
			processNotIndex++;
		}
	}
}

void  Probe::rssiIntegrate(char time[14],mncatsWifi &Probedata,int index)//作为新探针的整合程序，精简了文件存储部分，让代码简化了,使用秒钟作为空间的存储，能够起到纠错的作用
{
	int second=charTimeGetSecond(time);
	//std::cout<<"rssiint运行的时间为"<<second<<std::endl;
	bool storeflag=1;
	for (int i=0;i<rssiTempIndex[index][second];i++)//这样设计的主要原因在于数据只有一次
	{
		if (memcmp(sel[index][second][i].selMumac,Probedata.cmac2,sizeof(unsigned char)*6)==0)//当数据已经存在将数据的最大Rssi更新
		{
			sel[index][second][i].maxRssi=MaxRssi(sel[index][second][i].maxRssi,Probedata.crssi);
			storeflag=0;//如果匹配到就无需保存了
			break;
		}
	}
	if (storeflag)//如果未匹配到则保存
	{
		memcpy(sel[index][second][rssiTempIndex[index][second]].Timestamp,time,sizeof(char)*14);
		memcpy(sel[index][second][rssiTempIndex[index][second]].selMumac,Probedata.cmac2,sizeof(unsigned char)*6);
		sel[index][second][rssiTempIndex[index][second]].maxRssi=Probedata.crssi;
		rssiTempIndex[index][second]++;//将索引向后推进一位
	}
}

void  Probe::rssiMissGet()//用于找出数据为空的集合，负责整合和清空，应该还拥有输出文件的功能。
{
	//std::cout<<"rssiMissGET运行了 "<<std::endl;
	char timeFixed[16];
	time_t processGetIndexInit=processGetIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processGetIndexInit));
	int second=charTimeGetSecond(timeFixed);
	//得到数据的并集
	struct tempMac
	{
		unsigned char Mac[6];
	}tempmac[sameTimeMacNum];
	rssiMissIndex[second]=rssiTempIndex[0][second];//先把第一个探针的数据写进去
	for(int i1=0;i1< rssiTempIndex[0][second];i1++)
	{
		if (rssiTempIndex[0][second]==0)//若没有存到数据则跳出
			break;
		memcpy(tempmac[i1].Mac,sel[0][second][i1].selMumac,sizeof(unsigned char)*6);
	}
	for (int i = 1; i < ProbeNum; i++)//找到其他探针的并集
	{
		if (rssiTempIndex[i][second]==0)//若没有存到数据则跳出
			continue;
		for (int j = 0; j < rssiTempIndex[i][second]; j++)
		{
			bool sstoreflag=1;
			for (int k = 0; k <rssiMissIndex[second]; k++)
			{
				if (memcmp(sel[i][second][j].selMumac,tempmac[k].Mac,sizeof(unsigned char)*6)==0)//如果存在,就什么都不做，因为已经存到并集了
				{
					sstoreflag=0;
					break;
				}
			}
			if (sstoreflag)//如果不存在，则将其存入并集中
			{
				memcpy(tempmac[rssiMissIndex[second]].Mac,sel[i][second][j].selMumac,sizeof(unsigned char)*6);
				rssiMissIndex[second]++;
			}
		}
	}
	//要加入是否为空的判断
	if (rssiMissIndex[second]!=0)
	{
		//根据并集将数据重组
		for (int i = 0; i < rssiMissIndex[second]; i++)//探针所存在的元素
		{
			memcpy(seled[second][i].Timestamp,timeFixed,sizeof(char)*14);
			memcpy(seled[second][i].Mumac,tempmac[i].Mac,sizeof(unsigned char)*6);
			for (int j = 0; j < ProbeNum; j++)//填补各个探针
			{
				bool flagg=1;
				for (int k = 0; k < rssiTempIndex[j][second]; k++)
				{
					if (memcmp(seled[second][i].Mumac,sel[j][second][k].selMumac,sizeof(unsigned char)*6)==0)
					{
						seled[second][i].Rssi[j]=sel[j][second][k].maxRssi;
						flagg=0;
						break;
					}
				}
				if (flagg)//如果没有查到，只能将其置为0
				{
					seled[second][i].Rssi[j]=0;
				}
			}
		}
		//检测该函数部分,以后可以删除
		//std::cout<<"Get函数输出了"<<timeFixed<<std::endl;
		for (int index=0;index<rssiMissIndex[second];index++)
		{
			for (int ii=0;ii<14;ii++) 
			{
				fprintf(fpGet,"%c", seled[second][index].Timestamp[ii]);
			}
			fprintf(fpGet,",");
			fprintf(fpGet,"%02X:%02X:%02X:%02X:%02X:%02X,",seled[second][index].Mumac[0], seled[second][index].Mumac[1], seled[second][index].Mumac[2], \
				seled[second][index].Mumac[3],seled[second][index].Mumac[4], seled[second][index].Mumac[5]);
			//输出三个探针的RSSI信息部分
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpGet,"%d", seled[second][index].Rssi[r]);
				if(r!=ProbeNum-1)//最后一个不输出逗号
				{
					fprintf(fpGet,",");
				}
			}
			fprintf(fpGet,"\n");
		}
	}
	//在操作完成后，结尾完成清空，无论是否有，都需要在该操作中将数据清空，要包括索引
	for (int i=0;i<ProbeNum;i++)
	{
		memset(sel[i][second],0,sizeof(rssiTemp)*sameTimeMacNum);
		rssiTempIndex[i][second]=0;
	}
}

void  Probe::rssiMissNot()//用于填补数据为空的集合 明天分析不能连续输出的原因
{
	//std::cout<<"rssiMissNot运行了 "<<std::endl;
	char timeFixed[16];
	time_t processNotIndexInit=processNotIndex-6;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processNotIndexInit));
	int second=charTimeGetSecond(timeFixed);
	//要加入是否为空的判断，才输出
	if (rssiMissIndex[second]!=0)
	{
		//填补函数的部分
		for (int i = 0; i < rssiMissIndex[second]; i++)//遍历组合后的结果
		{
			for (int j = 0; j < ProbeNum; j++)//遍历RSSI
			{
				if (seled[second][i].Rssi[j]==0)//找出发现RSSI为0的部分
				{
					//开始上下查找，填补空值,取相邻时间相同Mac地址有值的RSSI的最大值，默认都遍历一回，没有就算了
					for (int k = 0; k < rssiMissIndex[(second-1)%60]; k++)//向前一秒查找
					{
						if (rssiMissIndex[(second-1)%60]==0)
							break;
						if (memcmp(seled[(second-1)%60][k].Mumac,seled[second][i].Mumac,sizeof(unsigned char)*6)==0)
						{
							seled[second][i].Rssi[j]=MaxRssi(seled[(second-1)%60][k].Rssi[j],seled[second][i].Rssi[j]);
							break;
						}
					}
					for (int k = 0; k < rssiMissIndex[(second+1)%60]; k++)//向后一秒查找
					{
						if (rssiMissIndex[(second+1)%60]==0)
							break;
						if (memcmp(seled[(second+1)%60][k].Mumac,seled[second][i].Mumac,sizeof(unsigned char)*6)==0)
						{
							seled[second][i].Rssi[j]=MaxRssi(seled[(second+1)%60][k].Rssi[j],seled[second][i].Rssi[j]);
							break;
						}
					}
				}
			}
		}
		//文件输出函数&检测该函数部分
		//std::cout<<"Not函数输出了"<<timeFixed<<std::endl;
		for (int index=0;index<rssiMissIndex[second];index++)
		{
			for (int ii=0;ii<14;ii++) 
			{
				fprintf(fpNot,"%c", seled[second][index].Timestamp[ii]);
			}
			fprintf(fpNot,",");
			fprintf(fpNot,"%02X:%02X:%02X:%02X:%02X:%02X,",seled[second][index].Mumac[0], seled[second][index].Mumac[1], seled[second][index].Mumac[2], \
				seled[second][index].Mumac[3],seled[second][index].Mumac[4], seled[second][index].Mumac[5]);
			//输出多个探针的RSSI信息部分
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpNot,"%d", seled[second][index].Rssi[r]);
				if(r!=ProbeNum-1)//最后一个不输出逗号
				{
					fprintf(fpNot,",");
				}
			}
			fprintf(fpNot,"\n");
		}
	}	
	//清空，并且要更加滞后才行,清空的应该是上一个变量
	memset(seled[(second-1)%60],0,sizeof(rssiMiss)*sameTimeMacNum);
	rssiMissIndex[(second-1)%60]=0;
	//后续分析函数 等Kinect函数封装好再写，kinect函数应该有输入的位置，还应该把旋转矩阵集成到程序中。可以设置坐标。查找的方法不知能否通过关联容器来写
	//rssiForMac();
	//rssiForMacAnalyticed();
}

void  Probe::rssiForMac()//用于找出对应Mac的RSSI序列集合，希望能够连接数据库提高性能
{

}

void  Probe::rssiForMacAnalyticed()//用于特定序列的变换 暂定，或许以后可以合并
{
	//可以设计自己的权值和算法
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

char Probe::NormRssi(char a[],int ccc)//通过高斯函数来滤波
{
	double sum=0.0;//求和
	double mean=0.0;
	double stdev=0.0;
	int num=0;
	for (int kk=0;kk<ccc;kk++)
	{
		num++;
		sum+=a[kk];
	}
	if (1==num)//要加入个数的判断，至少有两个数才能判断。
	{
		return char(sum);
	}else
	{
		mean =sum/num;//求均值
		double accum=0.0;
		for (int kk=0;kk<ccc;kk++)
		{
			accum+=(a[kk]-mean)*(a[kk]-mean);
		}
		stdev=sqrt(accum/num);//求标准差
		if (stdev==0)
		{
			return char(mean);
		}
		int num2=0,sum2=0;
		for (int kk=0;kk<ccc;kk++)//剔除值
		{
			if (myNormCdf((a[kk]-mean)/stdev)>=0.5)
			{
				sum2+=a[kk];
				++num2;
			}
		}	
		if(num2==0)
			return char(mean);
		return char(sum2/num2);
	}	
}

double Probe::myErf(double x)//误差函数，参考网上利用泰勒公式展开求积分
{
	double res = x;
	double factorial = 1;	//n!
	double x_pow = x;
	int one = 1, n;
	for( n=1; n<10; n++ ){
		factorial *= n;
		one *= -1;
		x_pow *= x*x;
		res += one / factorial * x_pow / ( 2*n+1 );
	}
	res *= 2 / sqrt(3.1416);
	return res;
}

double Probe::myNormCdf(double x)//高斯分布函数
{
	return ( 1 + myErf( x / sqrt(2) ) ) / 2;
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

int Probe::charTimeGetSecond(char ttt[14])//获得得到数据的后两位
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}
