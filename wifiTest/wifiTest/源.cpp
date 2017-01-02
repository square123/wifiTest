
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

#define SERVER_PORT 2222 
#define BUFFER_SIZE 1024 
#define THD -30

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

int main(int argc,char*argv[])
{
	struct selMacRssi
	{
		unsigned char selMumac[6];          //�����ź�MU��MAC��ַ
		char selRssi;                       //����������dBmΪ��λ��RSSI ����ֵ ʮ���ƣ�-128 ��127
	}sel[100];//����ṹ������ һ����100Ӧ���㹻����

	//�ı���ʹ��getchar(),openCVʹ��waitkey,����ȡ���̰���ʱ����ͨ�õ�GetKeyState(), system("pause")Ҳ��һ���ܺõ�ѡ��
	/*����winsock�ļ�*/
	struct cliprobeData *packageData;
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp;

	//���������ض�mac��ַ�Ĵ洢������
	char myRssi=0;
	//unsigned char Mymac1[6]={176,226,53,43,218,224};//�ҵ�mac��ַ
	//unsigned char Mymac2[6]={240,37,183,193,7,151};//������mac��ַ
	//getSpecialMac(Mymac1);//�õ�mac�����ݣ���ѡ
	//getSpecialRssi(myRssi);
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
		return 1;
	}
	//����Ƕ����
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//��Ƕ����
	sockaddr_in servAddr;//��������ַ
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 

	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	//�ļ��洢���Ե�ʱ��ȡ��
	fp=fopen("text.csv","a+");
	//��������
	while(1){

		/* ����һ����ַ�����ڲ���ͻ��˵�ַ */
		sockaddr_in clientAddr; 
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
		packageData = (struct cliprobeData *) buffer;//��ʽ������ 
		//ֻ��¼��Ӧ��mac��
		//kinect ��ȡ��ȵõ�һ���ж�����,���򲻶�mac����м���
		//�������ȡ���˵�����������С��1��ʱ����ʼ�洢��ֵ�ڵ�̽�����ݣ���ʼ��������Ƭ��
		//�����뿪ʱ����ͳ�����ݵĽ������Ļ������п��ܵ�Mac�룬�Լ���ѡmac�룬��ѡmac����п����䣬��Ļ���һ��ƥ������
		//���ݲ��ٴ洢�����¼��һ���ˡ�
		//ʵ��ʱ���Բ���bodyIndex ����ȣ���ѡ���ض���������ֵ��

		//�����������

		//Ҫ���̽���ض�������ȵĹ��ܣ��Ӷ���̽���ṩʹ�ܶ�

		if(1){
		///
		if(int(packageData->Rssi)>THD){	//������ֵ
			printf("ƥ�䵽�����п��ܵ�mac�룺\n");
			printf("%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			cout<<int(packageData->Rssi)<<endl;
			
            //�浽�ļ���
			//�ļ��洢���Ե�ʱ��ȡ��
			for (int i=0;i<14;i++) 
			{
				fprintf(fp,"%c", packageData->Timestamp[i]);
			}
			fprintf(fp,",");
			fprintf(fp,"%d,", packageData->Rssi);
			fprintf(fp,"%02X:%02X:%02X:%02X:%02X:%02X", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
				packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
			fprintf(fp,"\n");

			//�浽�ṹ������	


			//��̽�������޷������Ҫ�����ݽ��д�����ͳ�Ƽ������ݽ��
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
