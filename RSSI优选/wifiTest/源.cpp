#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
#include<time.h>//����ʱ��
#include <fstream>//ϣ������ʱ��
#include<math.h>
#pragma comment(lib,"WS2_32.lib")

using namespace std;
#define sameTimeMacNum 10 //��ʾͬһʱ����̽��Ĭ�����洢������������
#define ProbeNum 2 //��ʾһ���м���̽�뷵������
#define SERVER_PORT 2222 
#define BUFFER_SIZE 1024 
#define THD -80

class Probe
{
public:
	struct cliprobeData {
		unsigned char Apmac[6];          //Դ AP �� MAC ��ַ
		unsigned short int Vendorid;     //���� ID
		unsigned short int Reserved1;    //����λ
		unsigned char Bssid[6];          //AP �� BSSID
		unsigned char Radiotype;         //������Ϣ�����ߵ����� 0x01��802.11b
		//0x02��802.11g
		//0x03��802.11a
		//0x04��802.11n
		//0x05��802.11ac
		unsigned char Channel;           //AP����MU����ʱ���ڵ��ŵ�
		unsigned char Associated;        //MU�Ƿ����ӵ� AP��0x01���� 0x02����
		unsigned char Messagetype;       //�������� 0x00��Probe Request 0x01��Association Request
		char Timestamp[14];              //����MU��Ϣ��ʱ��㣬���뼶��ʱ���
		unsigned short int Reserved2;    //����λ
		unsigned char Mutype;            //MU�����ͣ�0x01��δ֪���� 0x02��MU
		unsigned short int Reserved3;    //����λ
		char Rssi;                       //����������dBmΪ��λ��RSSI ����ֵ ʮ���ƣ�-128 ��127
		unsigned short int Reserved4;    //����λ
		unsigned char Noisefloor;        //����������dBmΪ��λ�ĵ�������ֵ
		unsigned short int Reserved5;    //����λ
		unsigned char Datarate;          //������Ϣ�����ߵ����ͣ�0x01��802.11b
		//0x02��802.11g
		//0x03��802.11a
		//0x04��802.11n
		//0x05��802.11ac
		unsigned char MPDUflags;         //����MPDU��flag���ȷ��MU�����Ƿ�Я��Frame Control��Sequence Control�ֶ�
		//1������Я����Ӧ�ֶΣ�0������Я����Ӧ�ֶΣ�
		unsigned char Mumac[6];          //�����ź�MU��MAC��ַ
		unsigned short int Framecontrol;   //MPDU�С�FrameControl���ֶ�
		unsigned short int Sequencecontrol;//MPDU �С�SequenceControl���ֶ�
		unsigned short int Reserved6;      //����λ
	};
	struct cliprobeData *packageData;
	struct rssiTemp
	{
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][sameTimeMacNum];
	rssiTemp selBuffer[ProbeNum][sameTimeMacNum];//����һ�����ļĴ���
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp1,*fp2;
	//FILE	*fp3;
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; 
	unsigned char zeroMac[6];
	Probe();
	~Probe();
	void InitProbe();
	void probeProcess();
	void probeTimeFix(char src[],char dst[14] );//�����洢�޸����ʱ�����ݸ�ʽ������ϵͳʱ������������������Ҫ��ʹ��ʱҪͬ��
	void rssiPurify(char time[14],FILE *f,int index);//����ͬһʱ���RSSI��ѡ
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
	bool timeCompare(char time1[14],char time2[],int delta);//����ʱ������ʱ���Ƿ����delta��
	time_t charToTimeInt(char ttt[14]);//�ַ���ת����ʱ��int


private:
	char timeTemp[ProbeNum][14];//��������ʱ��仯�ı���
	char timeBuffer[ProbeNum][14];//ʱ��ļĴ����������洢���ϴε�ʱ��
	int indexForPure[ProbeNum];
	int indexForPureBuf[ProbeNum];//��Ϊ��һ�����ļĴ���
	bool flag[ProbeNum];
};

Probe::Probe()
{
	memset(&zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ�����
	for(int i=0;i<ProbeNum;i++)
	{
		memset(&sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//�ṹ���ʼ��
		memset(&timeTemp[i],0,sizeof(char)*14);//ʱ���ʼ��
		indexForPure[i]=0;//������ʼ��
		flag[i]=false;//��־λ��ʼ��
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
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	//fp3=fopen("prb2.csv","a+");
}


void Probe::probeTimeFix(char src[],char dst[14] ) //�����洢�޸����ʱ�����ݸ�ʽ������ϵͳʱ������������������Ҫ��ʹ��ʱҪͬ��
{
	time_t tt=time(NULL);
	tm* t=localtime(&tt);
	/*printf("%d-%02d-%02d %02d:%02d:%02d\n", t->tm_year + 1900,t->tm_mon + 1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);*/
	dst[0]=src[0];
	dst[1]=src[1];
	dst[2]=src[2];
	dst[3]=src[3];
	int i=4,j=4;
	if((t->tm_mon + 1)<10)//��
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
	if((t->tm_mday)<10) //��
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
	if((t->tm_hour)<10) //Сʱ
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
	if((t->tm_min)<10) //��
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
	if((t->tm_sec)<10) //��
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
	//����У�飬��Ϊ���ܴ����ڶ���ʱ�����Ա�̽���һ��
	//����ֻ�����һλ�����жϣ���Ϊʱ�������ֻ�п�������"��"����"��"��������
	if (dst[13]==0)
	{
		dst[13]=dst[12];
		dst[12]='0';
	}
	///////
	//�������
	//for (int i=0;i<14;i++) {
	//	printf("%c", dst[i]);
	//}
	//printf("\n");
}

void Probe::rssiPurify(char time[14],FILE *f,int index)//����ͬһʱ���RSSI��ѡ,���ϲ�ͬһʱ�䲢ѡ������RSSI
{
	if(memcmp(timeTemp[index],time,sizeof(char)*14)==0)//ʱ��һ��ʱ
	{
		for(int i=0;i<sameTimeMacNum;i++)
		{
			if(memcmp(sel[index][i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//����⵽����Ϊ��ʱ���Ѳ��ֵ��ֵ�������飬�������������Ĳ���
			{
				memcpy(sel[index][i].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
				sel[index][i].maxRssi=packageData->Rssi;
				indexForPure[index]++;
				break;//����ѭ��
			}
			else if(memcmp(packageData->Mumac,sel[index][i].selMumac,sizeof(unsigned char)*6)==0)//�Ƚ��Ƿ�������������,memcmp ��ͬ����0,��ͬ���ط���
			{
				sel[index][i].maxRssi=MaxRssi(sel[index][i].maxRssi,packageData->Rssi);
				break;//����ѭ��
			}
		}
	}
	else //��ʱ�䲻һ�µ�ʱ��Ҫ������ݣ�����Ҫ�ѵ�һ����ͬ�����ݴ����������ҽ�ʱ���¼����
	{//������ݵ��ļ�
		if(flag[index]==true)//�����ȴ浽���ݲ�ִ��д����
		{
			bool douflag=timeCompare(timeBuffer[index],timeTemp[index],1);//д���������������
			
			for(int j=0;j<indexForPure[index];j++)//ʹ����������ż�������
			{
				//�����ݼ���һ��Buffer�����Զ����ݽ���ɸѡ������ȫ���棬ֻҪ��MAC����Ӧ�þ���
				//���һ��������������ʱ��������Ϊһ�룬�Ž����Ƿ��ظ����ж�
				if (douflag)//�������Ѿ����ֵ�Mac��ֱ�Ӳ����ڵڶ����������������ݲ����ˣ���Ϊ�Ѿ������
				{
					//cout<<"ʱ����Ϊ1��"<<endl;
					bool skipflag=false;
					for (int k=0;k<indexForPureBuf[index];k++)
					{
						if (memcmp(sel[index][j].selMumac,selBuffer[index][k].selMumac,sizeof(unsigned char)*6)==0)//��Ҫ����ķֱ�����һ������ݽ��бȽ�
						{
							skipflag=true;//������ظ��ģ������flag,������forѭ��
							//cout<<"���ظ�"<<endl;
							break;
						}
					}
					if (skipflag==true)//������ظ��ģ�������������ڣ��Ƚ���һ��Mac��
					{
						//cout<<"Ӧ������һ��"<<endl;
						continue;
					}
				}
				//��ʾ���Բ���
				/*cout<<"Ҫ�����ʱ�䣺"<<charToTimeInt(timeTemp[index])<<",";
				for (int ii=0;ii<14;ii++) 
				{
					printf("%c", timeTemp[index][ii]);
				}
				printf(",");
				printf("%02X:%02X:%02X:%02X:%02X:%02X,", sel[index][j].selMumac[0], sel[index][j].selMumac[1], sel[index][j].selMumac[2], \
					sel[index][j].selMumac[3], sel[index][j].selMumac[4], sel[index][j].selMumac[5]);
				printf("%d", sel[index][j].maxRssi);
				printf("\n");*/
				//����ļ�����
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
			//���ϴε����ݴ���buffer�У�Ϊ���ܵ���һ�뱸�ã�ע����������������ģ����ԣ����������붼����ʱ����ʵֻ��¼��һ�γ��ֵ�ֵ
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//��¼�Ѿ�������ļ��»ؿ��Բο�����
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			//
			flag[index]=false;//������Ϊ��ֻ����һ��
			memset(&sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//���ṹ������Ϊ��
			indexForPure[index]=0;//����
		}
		//��������Ͱѵ�һ�����ݸ�©����,���ҵ����ݷ����仯������ļ�Ҳ�ᶪ������	
		memcpy(sel[index][0].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
		sel[index][0].maxRssi=packageData->Rssi;
		indexForPure[index]++;
		flag[index]=true;
		memcpy(timeTemp[index],time,sizeof(char)*14);//�ѱ仯��ʱ�丳ֵ��timeTemp
	}
}

void Probe::probeProcess()
{
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
	char * ipaddr=NULL;
	char addr[20];
	ipaddr= inet_ntoa(clientAddr.sin_addr);//��ȡIP��ַ
	strcpy(addr,ipaddr);  
	printf("%s\n",ipaddr);		//��ʾ�����źŵĵ�ַ
	packageData = (struct cliprobeData *) buffer;//��ʽ������ 
	char retime[14]="";//��ʼ������Ϊ��
	probeTimeFix(packageData->Timestamp,retime);//�޸�ʱ�����ʽ��ʹ����14λ
	if(int(packageData->Rssi)>THD){
		//Ӧ��������������ݵ�ͳһ�ϲ�
		if(addr[10]=='1')//������ǵ�ַ
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

bool Probe::timeCompare(char time1[14],char time2[],int delta)//����ʱ���ǽ�����ģ�������򵥵ķ�����ֱ��ת������ȥ�жϼ��
{
	time_t timeInt1,timeInt2;
	timeInt1=charToTimeInt(time1);
	//cout<<"��������ļ���ʱ��:"<<timeInt1<<endl;
	timeInt2=charToTimeInt(time2);
	//cout<<"���ڼ��������ʱ��:"<<timeInt2<<endl;
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
int main(int argc,char*argv[])//�Ժ�д����Ҫ����ķ�ʽȥ���������޸ĺ�����˼·��
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

