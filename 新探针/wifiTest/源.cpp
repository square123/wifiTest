#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
//#include<time.h>//����ʱ��
#include <fstream>

#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 
#define BUFFER_SIZE 100
class mncatsWifi 
{
public :
	string initData;//�����洢charת������ַ���
	string mac1;//�洢̽���mac�� 0-11
	string mac2;//�洢̽���mac�� 13-24
	string rssi;//�洢Rssiֵ 26-28
	string dtype;//�ź����� 30-31 //�Ժ��ٷ���
	string wifidate;//���� 33-42
	string wifitime;//ʱ�� 44-51
	string timeStamp;//�ۺϵ��ַ���
	mncatsWifi(string &s):mac1(s.substr(0,12)),mac2(s.substr(13,12)),rssi(s.substr(26,3)),dtype(s.substr(30,2)),\
		wifidate(s.substr(33,10)),wifitime(s.substr(44,8)),timeStamp(0){wifiReform(*this);}
	mncatsWifi(char ss[]):initData(ss),mac1(initData.substr(0,12)),mac2(initData.substr(13,12)),\
		rssi(initData.substr(26,3)),dtype(initData.substr(30,2)),wifidate(initData.substr(33,10)),\
		wifitime(initData.substr(44,8)){
			wifiReform(*this);
	}
private:
	mncatsWifi& wifiReform(mncatsWifi & temp1);
};
mncatsWifi& mncatsWifi::wifiReform(mncatsWifi & temp1)
{
	temp1.mac1=temp1.mac1.substr(0,2)+":"+temp1.mac1.substr(2,2)+":"+temp1.mac1.substr(4,2)+":"+temp1.mac1.substr(6,2)+":"+temp1.mac1.substr(8,2)+":"+temp1.mac1.substr(10,2);
	temp1.mac2=temp1.mac2.substr(0,2)+":"+temp1.mac2.substr(2,2)+":"+temp1.mac2.substr(4,2)+":"+temp1.mac2.substr(6,2)+":"+temp1.mac2.substr(8,2)+":"+temp1.mac2.substr(10,2);
	for (auto &i:temp1.mac2)
	{
		i=toupper(i);
	}
	temp1.timeStamp=temp1.wifidate.substr(0,4)+temp1.wifidate.substr(5,2)+temp1.wifidate.substr(8,2)+temp1.wifitime.substr(0,2)+\
		temp1.wifitime.substr(3,2)+temp1.wifitime.substr(6,2);
	return temp1;
}

int main(int argc,char*argv[])
{

	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	ofstream outfile("1.csv",ios::app);

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
		mncatsWifi datatemp=mncatsWifi(buffer);
		outfile<<datatemp.timeStamp<<","<<datatemp.mac2<<","<<datatemp.rssi<<","<<datatemp.dtype<<endl;
		cout<<datatemp.timeStamp<<","<<datatemp.mac2<<","<<datatemp.rssi<<endl;

		if( GetKeyState( VK_ESCAPE ) < 0 ){
			closesocket(s);
			::WSACleanup();
	    outfile.close();
			exit(0);
		}
	}

}
