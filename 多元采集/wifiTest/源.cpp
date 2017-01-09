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

#define SERVER_PORT 2222 

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

int main(int argc,char*argv[])
{
	/*����winsock�ļ�*/
	struct cliprobeData *packageData;
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp1,*fp2;


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
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	//��������
	while(1){
#define BUFFER_SIZE 1024 
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
		//�������
		//time_t t = time( 0 );
		//char tmp[64];//����ʱ��
		//strftime( tmp, sizeof(tmp), "%Y/%m/%d %X \n\t", localtime(&t) );//��ȡϵͳʱ��
		
		char * ipaddr=NULL;
		char addr[20];
		ipaddr= inet_ntoa(clientAddr.sin_addr);//��ȡIP��ַ
		strcpy(addr,ipaddr);  
		printf("%s\n",ipaddr);		
		packageData = (struct cliprobeData *) buffer;//��ʽ������ 
		//printf("�õ���̽�����ݣ�\n");
		//printf("ԴAP��MAC��ַ:%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Apmac[0], packageData->Apmac[1], packageData->Apmac[2], \
		//	packageData->Apmac[3], packageData->Apmac[4], packageData->Apmac[5]);
		//////printf("���ߵ�����:%02X\n", packageData->Radiotype);
		//printf("�ŵ�:%d\n", packageData->Channel);
		//printf("ʱ���:");
		///*   for (int i=0;i<14;i++) {
		//printf("%c", packageData->Timestamp[i]);
		//}*/
		//printf("%c%c%c%c-%c%c-%c %c:%c%c:%c%c",packageData->Timestamp[0],packageData->Timestamp[1],packageData->Timestamp[2],packageData->Timestamp[3],\
		//	packageData->Timestamp[4],packageData->Timestamp[5],packageData->Timestamp[6],packageData->Timestamp[7],\
		//	packageData->Timestamp[8],packageData->Timestamp[9],packageData->Timestamp[10],packageData->Timestamp[11]);
		//printf("\n");
		//printf("�ͻ���MAC��ַ:%02X:%02X:%02X:%02X:%02X:%02X\n", packageData->Mumac[0], packageData->Mumac[1], packageData->Mumac[2], \
		//	packageData->Mumac[3], packageData->Mumac[4], packageData->Mumac[5]);
		//printf("Rssiֵ:%d\n", packageData->Rssi);
	/*	printf("Noiseֵ��%d\n",packageData->Noisefloor);*/

		//printf("\n");
		//ofstream myfile("time.csv",ios::app);  //�ļ����
		//if(!myfile)
		//{
		//	cout<<"error !";
		//}
		//else
		//{
		//	//myfile<<tmp<<endl;//ȥ��һЩû�õ�ʱ��
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
