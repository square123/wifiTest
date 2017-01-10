#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
#include<time.h>//����ʱ��
#include <fstream>//ϣ������ʱ��
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
	char Probe::MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0

private:
	char timeTemp[ProbeNum][14];//��������ʱ��仯�ı���
	int indexForPure[ProbeNum];
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
		//�������
		//for (int i=0;i<14;i++) {
		//	printf("%c", dst[i]);
		//}
		//printf("\n");
}

void Probe::rssiPurify(char time[14],FILE *f,int index)//����ͬһʱ���RSSI��ѡ,���ϲ�ͬһʱ�䲢ѡ������RSSI����һ�������õ�ȫ�ֱ���ʱ
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
		//���һ��������������ʱ��������Ϊһ��

		//
		if(flag[index]==true)//�����ȴ浽���ݲ�ִ��д����
		{
			//�����ݼ���һ��Buffer�����Զ����ݽ���ɸѡ������ȫ���棬ֻҪ��MAC����Ӧ�þ���

			//
			for(int j=0;j<indexForPure[index];j++)//ʹ����������ż�������
			{
				//����Ϊ�յ��ж�

				//
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
			//�����ݴ���buffer��

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
		printf("%s\n",ipaddr);		
		packageData = (struct cliprobeData *) buffer;//��ʽ������ 
		char retime[14];
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
