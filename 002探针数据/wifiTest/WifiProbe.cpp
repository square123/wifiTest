#include "WifiProbe.h"

Probe::Probe()//���ݳ�ʼ��
{
	memset(zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ���mac����
	memset(zeroTimestamp,0,sizeof(char)*14);//����һ��ȫΪ���ʱ���
	memset(sel,0,sizeof(rssiTemp)*ProbeNum*60*sameTimeMacNum);
	memset(seled,0,sizeof(rssiMiss)*60*sameTimeMacNum);
	memset(rssiTempIndex,0,sizeof(int)*ProbeNum*60);
	memset(rssiMissIndex,0,sizeof(int)*60);
	memset(detProbeTime,0,sizeof(int)*ProbeNum);
	memset(syscTimeBuff,0,sizeof(int)*ProbeNum);
	zeroRssi=0;
	processGetIndex=0;//����ʱ��ô����
	processNotIndex=0;//����ʱ��ô����
}

Probe::~Probe()//�����ر��ļ���socket�ӿ�
{
	closesocket(s);
	::WSACleanup();
	fclose(fpNot);
	fclose(fpGet);
	exit(0);
}

void Probe::InitProbe()//��ʼ��
{
	//socketͨ�Ų���
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	//����Ƕ����
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//��Ƕ����
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 
	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	//��ѡ Mac���̲��Ҳ���
	Csv csv("������ֻ�Ʒ�ƶ�Ӧ��.csv");//��Ʒ�ƶ�Ӧ���Ǻ�ȫ����ʱ��ɾ���Ĳ��֣��´ο���ֻ����Ҫ�ֻ���Mac��ַ��¼�������ɡ�
	for (auto i=csv.table.begin();i!=csv.table.end();i++)
	{
		auto i1=i->begin();
		auto i2=i->begin()+1;
		mobileManu[*i1]=*i2;
	}
	//�ļ���������
	fpGet=fopen("probeGet.csv","a+");
	fpNot=fopen("probeNot.csv","a+");
	time_t syscTimee=time(NULL);	//����ִֻ��һ�Σ������Ͳ���Ҫ�ٲ�����
	processGetIndex=syscTimee+4;//��Ϊ��Ҫ�ͺ�������Ҫ������ı�־λ�ͺ󣬾�����ͺ�ʱ����������޸�
	processNotIndex=syscTimee+6;
}

void Probe::mobileManuOutput(mncatsWifi &Probedata)//����ֻ������Ķ�Ӧ����
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
		std::cout<<"����Ʒ��";
	}
}


void Probe::timesSysc(time_t &syscTime,mncatsWifi &Probedata)//ͬ����̽���ʱ��
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

	if (abs(syscTime-syscTimeBuff[index])>3600)//��������Сʱ���һ��ͬ��
	{
		syscTimeBuff[index]=syscTime;
		detProbeTime[index]=syscTime-charToTimeInt(Probedata.Timestamp);//��Ϊ̽���ʱ���ߵĽ�����������Ҫ���е�����
	}
}

void Probe::probeProcess()
{
	//socketͨ�Ű󶨲���
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//�������
	if (recvfrom(s,buffer,BUFFER_SIZE,0,(SOCKADDR *)&clientAddr,&clientAddrLength) == SOCKET_ERROR)
	{
		printf("recvfrom() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	time_t syscTime=time(NULL);	//��¼ϵͳʱ�䣬�������䴥��ʱ��ͬ���Ļ���
	mncatsWifi datatemp=mncatsWifi(buffer);//��ʽ������
	timesSysc(syscTime,datatemp);//����̽���ʱ��ͬ��
	if ((datatemp.dtype!="80")&&(int(datatemp.crssi)>THD))//����������Ȩ�أ�����ȥ��80beacon֡  
	{
		char timeFix[16];//�����޸�ʱ��,�Ժ������������Կ������Ƿ������ԭ��
		time_t timeFixtt=0;
		if(datatemp.mac1=="C8:E7:D8:D4:A3:75")//������ǵ�ַ
		{
			std::cout<<"̽��1������"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[0];//localtimeֻ������ת����ʽ,����û�л�ȡϵͳʱ��Ĺ��ܣ�����gmtime��ԡ�
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//ʱ���޸�
			rssiIntegrate(timeFix,datatemp,0);
		}
		else if(datatemp.mac1=="C8:E7:D8:D4:A3:02")
		{		
			std::cout<<"̽��2������"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[1];
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//ʱ���޸�
			rssiIntegrate(timeFix,datatemp,1);
		}
		else if(datatemp.mac1=="C8:E7:D8:D4:A3:60")
		{
			std::cout<<"̽��3������"<<" ";
			mobileManuOutput(datatemp);
			std::cout<<std::endl;
			timeFixtt=charToTimeInt(datatemp.Timestamp)+detProbeTime[2];
			strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&timeFixtt));//ʱ���޸�
			rssiIntegrate(timeFix,datatemp,2);
		}
		if (syscTime>=processGetIndex)//���в�ղ��� ���մﵽѭ������,�ĳɴ��ڵ�������Ϊ�����п��ܴ�������
		{
			rssiMissGet();
			processGetIndex++;
		}
		if (syscTime>=processNotIndex)//���в��ղ���
		{
			rssiMissNot();
			processNotIndex++;
		}
	}
}

void  Probe::rssiIntegrate(char time[14],mncatsWifi &Probedata,int index)//��Ϊ��̽������ϳ��򣬾������ļ��洢���֣��ô������,ʹ��������Ϊ�ռ�Ĵ洢���ܹ��𵽾��������
{
	int second=charTimeGetSecond(time);
	//std::cout<<"rssiint���е�ʱ��Ϊ"<<second<<std::endl;
	bool storeflag=1;
	for (int i=0;i<rssiTempIndex[index][second];i++)//������Ƶ���Ҫԭ����������ֻ��һ��
	{
		if (memcmp(sel[index][second][i].selMumac,Probedata.cmac2,sizeof(unsigned char)*6)==0)//�������Ѿ����ڽ����ݵ����Rssi����
		{
			sel[index][second][i].maxRssi=MaxRssi(sel[index][second][i].maxRssi,Probedata.crssi);
			storeflag=0;//���ƥ�䵽�����豣����
			break;
		}
	}
	if (storeflag)//���δƥ�䵽�򱣴�
	{
		memcpy(sel[index][second][rssiTempIndex[index][second]].Timestamp,time,sizeof(char)*14);
		memcpy(sel[index][second][rssiTempIndex[index][second]].selMumac,Probedata.cmac2,sizeof(unsigned char)*6);
		sel[index][second][rssiTempIndex[index][second]].maxRssi=Probedata.crssi;
		rssiTempIndex[index][second]++;//����������ƽ�һλ
	}
}

void  Probe::rssiMissGet()//�����ҳ�����Ϊ�յļ��ϣ��������Ϻ���գ�Ӧ�û�ӵ������ļ��Ĺ��ܡ�
{
	//std::cout<<"rssiMissGET������ "<<std::endl;
	char timeFixed[16];
	time_t processGetIndexInit=processGetIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processGetIndexInit));
	int second=charTimeGetSecond(timeFixed);
	//�õ����ݵĲ���
	struct tempMac
	{
		unsigned char Mac[6];
	}tempmac[sameTimeMacNum];
	rssiMissIndex[second]=rssiTempIndex[0][second];//�Ȱѵ�һ��̽�������д��ȥ
	for(int i1=0;i1< rssiTempIndex[0][second];i1++)
	{
		if (rssiTempIndex[0][second]==0)//��û�д浽����������
			break;
		memcpy(tempmac[i1].Mac,sel[0][second][i1].selMumac,sizeof(unsigned char)*6);
	}
	for (int i = 1; i < ProbeNum; i++)//�ҵ�����̽��Ĳ���
	{
		if (rssiTempIndex[i][second]==0)//��û�д浽����������
			continue;
		for (int j = 0; j < rssiTempIndex[i][second]; j++)
		{
			bool sstoreflag=1;
			for (int k = 0; k <rssiMissIndex[second]; k++)
			{
				if (memcmp(sel[i][second][j].selMumac,tempmac[k].Mac,sizeof(unsigned char)*6)==0)//�������,��ʲô����������Ϊ�Ѿ��浽������
				{
					sstoreflag=0;
					break;
				}
			}
			if (sstoreflag)//��������ڣ�������벢����
			{
				memcpy(tempmac[rssiMissIndex[second]].Mac,sel[i][second][j].selMumac,sizeof(unsigned char)*6);
				rssiMissIndex[second]++;
			}
		}
	}
	//Ҫ�����Ƿ�Ϊ�յ��ж�
	if (rssiMissIndex[second]!=0)
	{
		//���ݲ�������������
		for (int i = 0; i < rssiMissIndex[second]; i++)//̽�������ڵ�Ԫ��
		{
			memcpy(seled[second][i].Timestamp,timeFixed,sizeof(char)*14);
			memcpy(seled[second][i].Mumac,tempmac[i].Mac,sizeof(unsigned char)*6);
			for (int j = 0; j < ProbeNum; j++)//�����̽��
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
				if (flagg)//���û�в鵽��ֻ�ܽ�����Ϊ0
				{
					seled[second][i].Rssi[j]=0;
				}
			}
		}
		//���ú�������,�Ժ����ɾ��
		//std::cout<<"Get���������"<<timeFixed<<std::endl;
		for (int index=0;index<rssiMissIndex[second];index++)
		{
			for (int ii=0;ii<14;ii++) 
			{
				fprintf(fpGet,"%c", seled[second][index].Timestamp[ii]);
			}
			fprintf(fpGet,",");
			fprintf(fpGet,"%02X:%02X:%02X:%02X:%02X:%02X,",seled[second][index].Mumac[0], seled[second][index].Mumac[1], seled[second][index].Mumac[2], \
				seled[second][index].Mumac[3],seled[second][index].Mumac[4], seled[second][index].Mumac[5]);
			//�������̽���RSSI��Ϣ����
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpGet,"%d", seled[second][index].Rssi[r]);
				if(r!=ProbeNum-1)//���һ�����������
				{
					fprintf(fpGet,",");
				}
			}
			fprintf(fpGet,"\n");
		}
	}
	//�ڲ�����ɺ󣬽�β�����գ������Ƿ��У�����Ҫ�ڸò����н�������գ�Ҫ��������
	for (int i=0;i<ProbeNum;i++)
	{
		memset(sel[i][second],0,sizeof(rssiTemp)*sameTimeMacNum);
		rssiTempIndex[i][second]=0;
	}
}

void  Probe::rssiMissNot()//���������Ϊ�յļ��� ��������������������ԭ��
{
	//std::cout<<"rssiMissNot������ "<<std::endl;
	char timeFixed[16];
	time_t processNotIndexInit=processNotIndex-6;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processNotIndexInit));
	int second=charTimeGetSecond(timeFixed);
	//Ҫ�����Ƿ�Ϊ�յ��жϣ������
	if (rssiMissIndex[second]!=0)
	{
		//������Ĳ���
		for (int i = 0; i < rssiMissIndex[second]; i++)//������Ϻ�Ľ��
		{
			for (int j = 0; j < ProbeNum; j++)//����RSSI
			{
				if (seled[second][i].Rssi[j]==0)//�ҳ�����RSSIΪ0�Ĳ���
				{
					//��ʼ���²��ң����ֵ,ȡ����ʱ����ͬMac��ַ��ֵ��RSSI�����ֵ��Ĭ�϶�����һ�أ�û�о�����
					for (int k = 0; k < rssiMissIndex[(second-1)%60]; k++)//��ǰһ�����
					{
						if (rssiMissIndex[(second-1)%60]==0)
							break;
						if (memcmp(seled[(second-1)%60][k].Mumac,seled[second][i].Mumac,sizeof(unsigned char)*6)==0)
						{
							seled[second][i].Rssi[j]=MaxRssi(seled[(second-1)%60][k].Rssi[j],seled[second][i].Rssi[j]);
							break;
						}
					}
					for (int k = 0; k < rssiMissIndex[(second+1)%60]; k++)//���һ�����
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
		//�ļ��������&���ú�������
		//std::cout<<"Not���������"<<timeFixed<<std::endl;
		for (int index=0;index<rssiMissIndex[second];index++)
		{
			for (int ii=0;ii<14;ii++) 
			{
				fprintf(fpNot,"%c", seled[second][index].Timestamp[ii]);
			}
			fprintf(fpNot,",");
			fprintf(fpNot,"%02X:%02X:%02X:%02X:%02X:%02X,",seled[second][index].Mumac[0], seled[second][index].Mumac[1], seled[second][index].Mumac[2], \
				seled[second][index].Mumac[3],seled[second][index].Mumac[4], seled[second][index].Mumac[5]);
			//������̽���RSSI��Ϣ����
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpNot,"%d", seled[second][index].Rssi[r]);
				if(r!=ProbeNum-1)//���һ�����������
				{
					fprintf(fpNot,",");
				}
			}
			fprintf(fpNot,"\n");
		}
	}	
	//��գ�����Ҫ�����ͺ����,��յ�Ӧ������һ������
	memset(seled[(second-1)%60],0,sizeof(rssiMiss)*sameTimeMacNum);
	rssiMissIndex[(second-1)%60]=0;
	//������������ ��Kinect������װ����д��kinect����Ӧ���������λ�ã���Ӧ�ð���ת���󼯳ɵ������С������������ꡣ���ҵķ�����֪�ܷ�ͨ������������д
	//rssiForMac();
	//rssiForMacAnalyticed();
}

void  Probe::rssiForMac()//�����ҳ���ӦMac��RSSI���м��ϣ�ϣ���ܹ��������ݿ��������
{

}

void  Probe::rssiForMacAnalyticed()//�����ض����еı任 �ݶ��������Ժ���Ժϲ�
{
	//��������Լ���Ȩֵ���㷨
}

char Probe::MaxRssi(char rssi1,char rssi2)//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
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

char Probe::NormRssi(char a[],int ccc)//ͨ����˹�������˲�
{
	double sum=0.0;//���
	double mean=0.0;
	double stdev=0.0;
	int num=0;
	for (int kk=0;kk<ccc;kk++)
	{
		num++;
		sum+=a[kk];
	}
	if (1==num)//Ҫ����������жϣ������������������жϡ�
	{
		return char(sum);
	}else
	{
		mean =sum/num;//���ֵ
		double accum=0.0;
		for (int kk=0;kk<ccc;kk++)
		{
			accum+=(a[kk]-mean)*(a[kk]-mean);
		}
		stdev=sqrt(accum/num);//���׼��
		if (stdev==0)
		{
			return char(mean);
		}
		int num2=0,sum2=0;
		for (int kk=0;kk<ccc;kk++)//�޳�ֵ
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

double Probe::myErf(double x)//�������ο���������̩�չ�ʽչ�������
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

double Probe::myNormCdf(double x)//��˹�ֲ�����
{
	return ( 1 + myErf( x / sqrt(2) ) ) / 2;
}

bool Probe::timeCompare(char time1[14],char time2[],int delta)//����ʱ���ǽ�����ģ�������򵥵ķ�����ֱ��ת������ȥ�жϼ��
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
	memcpy(year1,ttt,sizeof(char)*4);//��ʱ��1�ֿ�
	memcpy(mon1,ttt+4,sizeof(char)*2);
	memcpy(day1,ttt+6,sizeof(char)*2);
	memcpy(hour1,ttt+8,sizeof(char)*2);
	memcpy(min1,ttt+10,sizeof(char)*2);
	memcpy(second1,ttt+12,sizeof(char)*2);
	struct tm tt1;
	memset(&tt1,0,sizeof(tt1));
	tt1.tm_year=atoi(year1)-1900;    //atoi�����ǽ��ַ���ת�����������ͣ���֪����������Ͳ����Լ�д��
	tt1.tm_mon=atoi(mon1)-1;    
	tt1.tm_mday=atoi(day1);    
	tt1.tm_hour=atoi(hour1);    
	tt1.tm_min=atoi(min1);    
	tt1.tm_sec=atoi(second1);    
	timeInt1=mktime(&tt1); 
	return timeInt1;
}

int Probe::charTimeGetSecond(char ttt[14])//��õõ����ݵĺ���λ
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}
