#include "WifiProbe.h"

Probe::Probe()//数据初始化
{
	storeIndex=0;//要更新的数据存储索引，直接从索引0开始存储
	processIndex=(storeIndex+1)%buffNum;//要使基准组的索引号，从2开始 storeIndex只负责表示baseIndex的数据
	storeIndexBuffer=storeIndex;
	memset(zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的mac码量
	memset(zeroTimestamp,0,sizeof(char)*14);//定义一个全为零的时间戳
	zeroRssi=0;
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//输出变量初始化
	memset(syscStr,0,sizeof(syscProbe)*sameTimeMacNum*60*ProbeNum);//非baseIndex探针的初始化
	memset(syscStrForIndex,0,sizeof(syscProbe)*buffNum*sameTimeMacNum);//baseIndex探针的初始化
	memset(&zeroSysc,0,sizeof(syscProbe));//定义存储全零为syscProbe格式
	memset(rssiData,0,sizeof(char)*ProbeNum*sameTimeMacNum*rssiCapacity);//初始化
	memset(rssiIndex,0,sizeof(int)*ProbeNum*sameTimeMacNum);
	for(int i=0;i<ProbeNum;i++)
	{
		memset(sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//结构体初始化
		memset(timeTemp[i],0,sizeof(char)*14);//时间初始化
		indexForPure[i]=0;//索引初始化
		flag[i]=0;//标志位初始化
		detProbeTime[i]=0;
		syscTimeBuff[i]=0;
	}
}

Probe::~Probe()//用来关闭文件和socket接口
{
	closesocket(s);
	::WSACleanup();
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fpSysc);
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
	//文件创建部分
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	fp3=fopen("probe3.csv","a+");
	fpSysc=fopen("probeSysc.csv","a+");
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
	time_t syscTime=time(NULL);;	//记录系统时间，用来将其触发时间同步的机制
	localtime(&syscTime);
	mncatsWifi datatemp=mncatsWifi(buffer);//格式化数据
	timesSysc(syscTime,datatemp);//负责探针的时间同步
	char timeFix[16];//用来修复时间
	time_t timeFixtt=time(NULL);
	if(datatemp.mac1=="C8:E7:D8:D4:A3:75")//定义的是地址
	{
		std::cout<<"探针1的数据"<<std::endl;
		timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[0];//localtime只是用来转换格式,它并没有获取系统时间的功能，它和gmtime相对。
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
		rssiPurify(timeFix,datatemp,fp1,0);
	}
	else if(datatemp.mac1=="C8:E7:D8:D4:A3:02")
	{		
		std::cout<<"探针2的数据"<<std::endl;
		timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[1];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
		rssiPurify(timeFix,datatemp,fp2,1);
	}else if(datatemp.mac1=="C8:E7:D8:D4:A3:60")
	{
		std::cout<<"探针3的数据"<<std::endl;
		timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[2];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//时间修复
		rssiPurify(timeFix,datatemp,fp3,2);
	}

	if (storeIndex!=storeIndexBuffer)//只要baseIndex存储完毕后就去处理数据
	{
		storeIndexBuffer=storeIndex;
		std::cout<<"同步函数运行"<<std::endl;
		probeSysc(fpSysc);//同步处理函数
	}
}

//废弃该函数
void Probe::rssiPurify(char time[14],mncatsWifi &Probedata,FILE *f,int index)//进行同一时间的RSSI优选,即合并同一时间并选出最大的RSSI
{	
	if(memcmp(timeTemp[index],time,sizeof(char)*14)==0)//时间一致时
	{
		for(int i=0;i<sameTimeMacNum;i++)
		{
			if(memcmp(sel[index][i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//当检测到区域为零时，把测得值赋值给该数组，这个是增加种类的部分
			{
				memcpy(sel[index][i].selMumac,Probedata.cmac2,sizeof(unsigned char)*6);
				//最大值滤波
				sel[index][i].maxRssi=Probedata.crssi;
				indexForPure[index]++;
				break;//跳出循环
			}
			else if(memcmp(Probedata.cmac2,sel[index][i].selMumac,sizeof(unsigned char)*6)==0)//比较是否出现这样的组合,memcmp 相同返回0,不同返回非零
			{

				//最大值滤波
				sel[index][i].maxRssi=MaxRssi(sel[index][i].maxRssi,Probedata.crssi);//选取最大的RSSI值
				break;//跳出循环
			}
		}
	}
	else //当时间不一致的时候要输出数据，并且要把第一个不同的数据存下来，并将时间记录下来
	{    //输出数据到文件
		if(flag[index]==true)//必须先存到数据才执行写数据
		{
			//清空storeIndex下同步结构体的数据
			if (index==baseIndex)//如果是baseIndex则将其清零
			{
				memset(syscStrForIndex[storeIndex],0,sizeof(syscProbe)*sameTimeMacNum);//将同步结构体index索引下的数据置为零
			}else//这个地方写的不够好，其实只能默认baseIndex为0，若为其他值，这里会出错，先不管那么多了
			{
				memset(syscStr[charTimeGetSecond(timeTemp[index])][index-1],0,sizeof(syscProbe)*sameTimeMacNum);//将数据对应位置清零
			}
			//下面部分是RSSI优化要完成的功能
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
				fprintf(f,"%d,", sel[index][j].maxRssi);//输出最大值
				fprintf(f,"\n");
				//探针同步存储部分-------分为两种情况：是baseIndex和非baseIndex
				if (index==baseIndex)
				{
					memcpy(syscStrForIndex[storeIndex][j].Timestamp,timeTemp[index],sizeof(char)*14);
					memcpy(syscStrForIndex[storeIndex][j].selMumac,sel[index][j].selMumac,sizeof(unsigned char)*6);
					syscStrForIndex[storeIndex][j].Rssi=sel[index][j].maxRssi;
				}else
				{
					memcpy(syscStr[charTimeGetSecond(timeTemp[index])][index-1][j].Timestamp,timeTemp[index],sizeof(char)*14);
					memcpy(syscStr[charTimeGetSecond(timeTemp[index])][index-1][j].selMumac,sel[index][j].selMumac,sizeof(unsigned char)*6);
					syscStr[charTimeGetSecond(timeTemp[index])][index-1][j].Rssi=sel[index][j].maxRssi;
				}
			}
			//将上次的数据存入buffer中，为可能的下一秒备用，注意这里是完整保存的，所以，当连续三秒都出现时，其实只记录第一次出现的值
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//记录已经输出的文件下回可以参考的量
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			flag[index]=false;//重新置为否，只运行一次
			memset(sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//将结构体重置为零
			memset(rssiData[index],0,sizeof(char)*sameTimeMacNum*rssiCapacity);//将RSSI值的存储置零
			indexForPure[index]=0;//重置
			memset(rssiIndex[index],0,sizeof(int)*sameTimeMacNum);//将存储RSSI的索引置零
			//探针同步紧凑部分
			syscProbe temp[sameTimeMacNum];//将使函数紧凑的部分改在这里
			if (index==baseIndex)
			{
				reduceSyscProbe(syscStrForIndex[storeIndex],temp);
				memcpy(syscStrForIndex[storeIndex],temp,sizeof(syscProbe)*sameTimeMacNum);
				//推进同步程序的发展
				storeIndex=(storeIndex+1)%buffNum;
				processIndex=(storeIndex+1)%buffNum;
			}else
			{
				reduceSyscProbe(syscStr[charTimeGetSecond(timeTemp[index])][index-1],temp);
				memcpy(syscStr[charTimeGetSecond(timeTemp[index])][index-1],temp,sizeof(syscProbe)*sameTimeMacNum);
			}
		}
		//不加这个就把第一个数据给漏掉了,并且当数据发生变化输出到文件也会丢掉数据	
		memcpy(sel[index][0].selMumac,Probedata.cmac2,sizeof(unsigned char)*6);
		sel[index][0].maxRssi=Probedata.crssi;
		indexForPure[index]++;
		flag[index]=true;
		memcpy(timeTemp[index],time,sizeof(char)*14);//把变化的时间赋值给timeTemp
	}
}
//废弃该函数
void Probe::probeSysc(FILE *f)//探针同步函数,将同步后的数据存储到一个新的表中
{
	//初始化部分
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//用于多探针集合表格式，清空
	int jianshao=0;
	int timePoint=charTimeGetSecond(syscStrForIndex[processIndex][0].Timestamp);//记下时间
	std::cout<<"同步时间："<<timePoint<<std::endl;
	//判断部分
	for (int m=0;m<sameTimeMacNum;m++)//该循环是将baseIndex探针中processIndex时间下的结构体数据与其他索引进行比较
	{
		if (memcmp(&syscStrForIndex[processIndex][m],&zeroSysc,sizeof(syscProbe))==0)//为空跳出，减少运算
		{
			break;
		}	
		++jianshao;
		//先把变量存下来
		memcpy(syscResult[m].Timestamp,syscStrForIndex[processIndex][m].Timestamp,sizeof(char)*14);
		memcpy(syscResult[m].selMumac,syscStrForIndex[processIndex][m].selMumac,sizeof(unsigned char)*6);
		syscResult[m].Rssi[baseIndex]=syscStrForIndex[processIndex][m].Rssi;

		for(int n=0;n<ProbeNum-1;n++)//该循环剔除baseIndex，只比较其他两个探针的数据
		{	
			for (int k=(59+timePoint)%60;k!=(timePoint+62)%60;k=(k+61)%60)//用这个来将时间的限制在上下一秒内
			{
				bool skip=0;
				for (int l=0;l<sameTimeMacNum;l++)//遍历一个时间块下, l表示块内存储索引
				{	
					if (memcmp(&syscStr[k][n][l],&zeroSysc,sizeof(syscProbe))==0)
					{
						break;//因为已经让数据紧凑了，所以可以直接使用break
					}
					if(timeCompare(syscStr[k][n][l].Timestamp,syscStrForIndex[processIndex][m].Timestamp,1))//如果匹配就把RSSI值导入
					{
						if (memcmp(syscStrForIndex[processIndex][m].selMumac,syscStr[k][n][l].selMumac,sizeof(unsigned char)*6)==0)
						{
							syscResult[m].Rssi[n+1]=syscStr[k][n][l].Rssi;//因为第一位已经占用了，只能从第二位开始
							//syscResult[m].NRssi[n+1]=syscStr[k][n][l].NRssi;//因为第一位已经占用了，只能从第二位开始//高斯
							skip=1;
							break;
						}
					}
				}
				if (skip)//减少运算量，要是检测到一个值，就直接跳出这个时间
				{
					break;
				}
			}
		}
	}
	//输出文件部分，检测均有值才输出
	for (int q=0;q<jianshao;++q)
	{
		bool outflag=1;
		outflag=outflag&&memcmp(syscResult[q].selMumac,zeroMac,sizeof(unsigned char)*6);//有一个就好
		for (int r=0;r<ProbeNum;r++)
		{
			outflag=outflag&&memcmp(&syscResult[q].Rssi[r],&zeroRssi,sizeof(char));//这里就不检查了，因为max有，norm必定有
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
			//输出三个探针的RSSI信息部分
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpSysc,"%d", syscResult[q].Rssi[r]);
				if(r!=ProbeNum-1)//最后一个不输出逗号
				{
					fprintf(fpSysc,",");
				}
			}
			//输出三个探针的差值信息部分
			//fprintf(fpSysc,",%d,%d,%d",syscResult[q].Rssi[0]-syscResult[q].Rssi[1],syscResult[q].Rssi[1]-syscResult[q].Rssi[2],syscResult[q].Rssi[2]-syscResult[q].Rssi[0]);
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
			memcpy(&sysced[j],&sysc[i],sizeof(syscProbe));//这句写的不对，不能取地址
			//memcpy(sysced[j].selMumac,sysc[i].selMumac,sizeof(unsigned char)*6);
			//memcpy(sysced[j].Timestamp,sysc[i].Timestamp,sizeof(char)*14);
			//sysced[j].Rssi=sysc[i].Rssi;
			//sysced[j].NRssi=sysc[i].NRssi;
			j++;
		}
	}
}
//废弃该函数，不使用
void Probe::AllreduceSyscProbe(syscProbe Allsysc[buffNum][ProbeNum][sameTimeMacNum],syscProbe Allsysced[buffNum][ProbeNum][sameTimeMacNum])
{
	for (int i=0;i<buffNum;i++)
	{
		for (int j=0;j<ProbeNum;j++)
		{
			reduceSyscProbe(Allsysc[i][j],Allsysced[i][j]);
		}
	}
}