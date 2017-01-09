//#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
//#include<time.h>//����ʱ��
#include <fstream>//ϣ������ʱ��

#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP�󶨶˿ں�
#define BUFFER_SIZE 1024 
#define RSSITHD -30   //wifi̽��̽�����ֵ
#define REC_RSSI_COUNT 10 //ϵͳĬ�ϴ洢rssi���������

struct cliprobeData { //̽�뷵�����ݸ�ʽ
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

//��char����ת����02X�ַ�����
string charTo02XStr(unsigned char input)
{
	int high,low;
	char out[2]={0};
	
	high=(input&240)>>4;//����λ
	low=input&15;		//����λ
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
	string output(out,2);//�ڶ���������������string�ĳ��ȣ���Ȼ���������
	return output;
}

string macToString(unsigned char Mymac[6])//��ɽ�char����ת�����ַ���
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+":"+charTo02XStr(Mymac[i]);
	}
	return output;
}

void getSpecialMac(unsigned char Mymac[6])//ת���ַ�������ʽ�ĺ���
{
	//unsigned char Mymac[6]={0};//b0,e2,35,2b,da,e0
	cout<<"����Ҫ�ض�ʶ����ַ�����ʽ��������ð�Ÿ�ʽ���룬��ΪСд��"<<endl;
	scanf("%02x:%02x:%02x:%02x:%02x:%02x",&Mymac[0],&Mymac[1],&Mymac[2],&Mymac[3],&Mymac[4],&Mymac[5]);
	cout<<"���ת������ʽ��"<<endl;
	printf("%hhu,%hhu,%hhu,%hhu,%hhu,%hhu\n",unsigned char(Mymac[0]),Mymac[1],Mymac[2],Mymac[3],Mymac[4],Mymac[5]);
	//system("pause");
}

void getSpecialRssi(char MyRssi)//ת���ַ�������ʽ�ĺ���
{
	cout<<"������С��dB��"<<endl;
	scanf("%d",MyRssi);
	cout<<"���ת������ʽ��"<<endl;
	printf("%d\n",MyRssi);
	//system("pause");
}

float meanRssiSpecial(float lastOutput,float input,int num)//ֱ�Ӽ���ƽ��ֵ,��ʱ�任ȡ�ռ䣬���ϴ���֪��ֵ��ͳ�ƵĴ�����ֱ����ü���ֵ��ƽ����
{														   //�������øú������ã���float����̫�鷳�ˣ�ͨ��ͳ�ƺ�ֱ��ȥ���ƺ�����Ҳ���Щ
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
	printf("��ֵ�������ص�floatֵ��%d\n",avgNum);
	return avgNum;
}

char MaxRssi(char rssi1,char rssi2)//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
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

//wifi����
class Wifi
{
public:

	struct selMacRssi//����ýṹ��������ͳ�Ƴ���RSSIֵ�����������ʹ�ñ䳤�Ĵ洢�����ڶ�ʱ���ڲ�����ֺܶ�Mac��ַС��ĳ��ֵ����������ֻ����10��ֵ
	{
		unsigned char selMumac[6];      //�����ź�MU��MAC��ַ
		int sumRssi;                    //����RSSI���ܺ�
		char maxRssi;                   //����RSSI�����ֵ
		int num;						//ͳ�Ƹ�RSSI���ֵĴ���
	}sel[REC_RSSI_COUNT];//����Ҫ�洢�Ľṹ������
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp;
	sockaddr_in servAddr;//��������ַ
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
	memset(&zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ�����
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

void Wifi::InitWifi()//��ʼ��server��������
{
	//�ı���ʹ��getchar(),openCVʹ��waitkey,����ȡ���̰���ʱ����ͨ�õ�GetKeyState(), system("pause")Ҳ��һ���ܺõ�ѡ��
	/*����winsock�ļ�*/
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
}
void Wifi::wifiProcess()//��¼Macֵ�Ͷ�Ӧ��RSSIֵ
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
	packageData = (struct cliprobeData *) buffer;
	if(int(packageData->Rssi)>RSSITHD)
	{
		cout<<"�Ѽ�⵽�ź�:\n"<<endl;
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",packageData->Mumac[0],packageData->Mumac[1],\
		packageData->Mumac[2],packageData->Mumac[3],packageData->Mumac[4],packageData->Mumac[5]);
		printf( "%d\n",packageData->Rssi);
		for(int i=0;i<REC_RSSI_COUNT;i++)
		{
			if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//����⵽����Ϊ��ʱ���Ѳ��ֵ��ֵ��������
			{
				memcpy(sel[i].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
				sel[i].num=1;
				sel[i].maxRssi=packageData->Rssi;
				sel[i].sumRssi=int(packageData->Rssi);
				break;//����ѭ��
			}
			else if(memcmp(packageData->Mumac,sel[i].selMumac,sizeof(unsigned char)*6)==0)//�Ƚ��Ƿ�������������,memcmp ��ͬ����0,��ͬ���ط���
			{
				sel[i].num=sel[i].num+1;
				sel[i].maxRssi=MaxRssi(sel[i].maxRssi,packageData->Rssi);
				sel[i].sumRssi=sel[i].sumRssi+int(packageData->Rssi);
				//cout<<sel[i].sumRssi<<endl;//�������
				//cout<<sel[i].maxRssi<<endl;
				break;//����ѭ��
			}
		}
	}
}

void Wifi::reSelMacRssi()//����¼�Ľṹ��������������
{
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

void Wifi::wifiProcessed()//�жϺ��������
{
	int rssiTemp=-100;
	int index=0;
	int del;
	string str;
	for(int i=0;i<REC_RSSI_COUNT;i++)
		{
			if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//����⵽����Ϊ��ʱ����������
			{
				break;
			}
			else //��¼���ֵ�ĳ���
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
		printf("�Բ���,ϵͳδƥ�䵽Mac��ַ\n");
	}
	else
	{
	//������ֵ�Ĳ���
	printf("ƥ�䵽������ܵ�mac�룺\n");
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n",sel[index].selMumac[0],sel[index].selMumac[1],\
		sel[index].selMumac[2],sel[index].selMumac[3],sel[index].selMumac[4],sel[index].selMumac[5]);
	printf("��Ӧ��ƽ���ź�ֵ��%d\n",rssiTemp);
	printf("��Ӧ������ź�ֵ��%d\n",sel[index].maxRssi);
	str=sel[index].selMumac[0]+':'+sel[index].selMumac[1]+':'+sel[index].selMumac[2]+':'+sel[index].selMumac[3]+':'+sel[index].selMumac[4]+':'+sel[index].selMumac[5];
	cout<<str<<endl;
	//printf("��Ӧ����ͳ�ƴ���:%d",sel[index].num);
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