#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#define sameTimeMacNum 5 //��ʾͬһʱ����̽��Ĭ�����洢������������
#define ProbeNum 1 //��ʾһ���м���̽�뷵������
#define SERVER_PORT 2222 
#define BUFFER_SIZE 1024 

#define buffNum 3
using namespace std;

struct syscProbe	//ͬ����̽��Ľṹ��	
{
	int Timestamp[14];
	int selMumac[6];
	int  Rssi;
}syscStr[buffNum][ProbeNum][sameTimeMacNum];//��ά�ṹ�����飬Ҫ�Ĵ��ʱ�䡢̽��������ÿ���Ӵ洢�����ݣ�3������صķŵ�����

syscProbe zeroSysc;//����洢ȫ��ΪsyscProbe��ʽ
	
void reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum])
{
	memset(sysced,0,sizeof(syscProbe)*sameTimeMacNum);//������ṹ����������
	int j=0;
	for (int i=0;i<sameTimeMacNum;i++)
	{
		if (memcmp(&zeroSysc,&sysc[i],sizeof(syscProbe))!=0)//�����Ϊ�գ��ͱ������µ����ݼ�����
		{
			memcpy(&sysced[j],&sysc[i],sizeof(syscProbe));//���д�Ĳ��ԣ�����ȡ��ַ
			//memcpy(sysced[j].selMumac,sysc[i].selMumac,sizeof(int)*6);
			//memcpy(sysced[j].Timestamp,sysc[i].Timestamp,sizeof(int)*14);
			//sysced[j].Rssi=sysc[i].Rssi;
			j++;
		}
	}
}

void AllreduceSyscProbe(syscProbe Allsysc[buffNum][ProbeNum][sameTimeMacNum],syscProbe Allsysced[buffNum][ProbeNum][sameTimeMacNum])
{
	for (int i=0;i<buffNum;i++)
	{
		for (int j=0;j<ProbeNum;j++)
		{
			reduceSyscProbe(Allsysc[i][j],Allsysced[i][j]);
		}
	}
}

int main()
{
	memset(&zeroSysc,0,sizeof(syscProbe));//����洢ȫ��ΪsyscProbe��ʽ
	syscProbe syscStrEd[buffNum][ProbeNum][sameTimeMacNum];//�������ڴ洢���պ�ı���
	memset(syscStrEd,0,(sizeof(syscProbe)*buffNum*ProbeNum*sameTimeMacNum));//���&��ʼ��
	int  temp=-30;
	int  temp1[6]={1,2,3,4,5,6};
	int temp2[14]={1,2,3,4,5,6,7,8,9,0,1,2,3,4};
	syscStr[0][0][0].Rssi=temp;
	memcpy(syscStr[0][0][0].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[0][0][0].Timestamp,temp2,sizeof(int )*14);
	syscStr[0][0][2].Rssi=temp;
	memcpy(syscStr[0][0][2].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[0][0][2].Timestamp,temp2,sizeof(int)*14);
	syscStr[0][0][4].Rssi=temp;
	memcpy(syscStr[0][0][4].selMumac,temp1,sizeof(int)*6);
	memcpy(syscStr[0][0][4].Timestamp,temp2,sizeof(int )*14);

	syscStr[1][0][0].Rssi=temp;
	memcpy(syscStr[1][0][0].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[1][0][0].Timestamp,temp2,sizeof(int )*14);
	syscStr[1][0][2].Rssi=temp;
	memcpy(syscStr[1][0][2].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[1][0][2].Timestamp,temp2,sizeof(int)*14);
	syscStr[1][0][4].Rssi=temp;
	memcpy(syscStr[1][0][4].selMumac,temp1,sizeof(int)*6);
	memcpy(syscStr[1][0][4].Timestamp,temp2,sizeof(int )*14);

	syscStr[2][0][0].Rssi=temp;
	memcpy(syscStr[2][0][0].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[2][0][0].Timestamp,temp2,sizeof(int )*14);
	syscStr[2][0][2].Rssi=temp;
	memcpy(syscStr[2][0][2].selMumac,temp1,sizeof(int )*6);
	memcpy(syscStr[2][0][2].Timestamp,temp2,sizeof(int)*14);
	syscStr[2][0][4].Rssi=temp;
	memcpy(syscStr[2][0][4].selMumac,temp1,sizeof(int)*6);
	memcpy(syscStr[2][0][4].Timestamp,temp2,sizeof(int )*14);
	for (int i = 0; i < buffNum; i++)
	{
		for (int j = 0; j < ProbeNum; j++)
		{
			for (int k = 0; k < sameTimeMacNum; k++)
			{
				cout<<syscStr[i][j][k].Rssi<<" "<<syscStr[i][j][k].selMumac[0]<<" "<<syscStr[i][j][k].selMumac[1]<<" "
					<<syscStr[i][j][k].selMumac[2]<<" "<<syscStr[i][j][k].selMumac[3]<<" "<<syscStr[i][j][k].selMumac[4]<<" "
					<<syscStr[i][j][k].selMumac[5]<<" "<<syscStr[i][j][k].Timestamp[0]
				<<" "<<syscStr[i][j][k].Timestamp[1]<<" "<<syscStr[i][j][k].Timestamp[2]<<" "<<syscStr[i][j][k].Timestamp[3]
				<<" "<<syscStr[i][j][k].Timestamp[4]<<" "<<syscStr[i][j][k].Timestamp[5]<<" "<<syscStr[i][j][k].Timestamp[6]
				<<" "<<syscStr[i][j][k].Timestamp[7]<<" "<<syscStr[i][j][k].Timestamp[8]<<" "<<syscStr[i][j][k].Timestamp[9]
				<<" "<<syscStr[i][j][k].Timestamp[10]<<" "<<syscStr[i][j][k].Timestamp[11]<<" "<<syscStr[i][j][k].Timestamp[12]
				<<" "<<syscStr[i][j][k].Timestamp[13]<<" "<<syscStr[i][j][k].Timestamp[14]<<endl;
			}
			cout<<endl<<endl;
		}
		cout<<endl<<endl;
	}
	cout<<endl;
	AllreduceSyscProbe(syscStr,syscStrEd);
	for (int i = 0; i < buffNum; i++)
	{
		for (int j = 0; j < ProbeNum; j++)
		{
			for (int k = 0; k < sameTimeMacNum; k++)
			{
				cout<<syscStrEd[i][j][k].Rssi<<" "<<syscStrEd[i][j][k].selMumac[0]<<" "<<syscStrEd[i][j][k].selMumac[1]<<" "
					<<syscStrEd[i][j][k].selMumac[2]<<" "<<syscStrEd[i][j][k].selMumac[3]<<" "<<syscStrEd[i][j][k].selMumac[4]<<" "
					<<syscStrEd[i][j][k].selMumac[5]<<" "<<syscStrEd[i][j][k].Timestamp[0]
				<<" "<<syscStrEd[i][j][k].Timestamp[1]<<" "<<syscStrEd[i][j][k].Timestamp[2]<<" "<<syscStrEd[i][j][k].Timestamp[3]
				<<" "<<syscStrEd[i][j][k].Timestamp[4]<<" "<<syscStrEd[i][j][k].Timestamp[5]<<" "<<syscStrEd[i][j][k].Timestamp[6]
				<<" "<<syscStrEd[i][j][k].Timestamp[7]<<" "<<syscStrEd[i][j][k].Timestamp[8]<<" "<<syscStrEd[i][j][k].Timestamp[9]
				<<" "<<syscStrEd[i][j][k].Timestamp[10]<<" "<<syscStrEd[i][j][k].Timestamp[11]<<" "<<syscStrEd[i][j][k].Timestamp[12]
				<<" "<<syscStrEd[i][j][k].Timestamp[13]<<" "<<syscStrEd[i][j][k].Timestamp[14]<<endl;
			}
			cout<<endl<<endl;
		}
		cout<<endl<<endl;
	}
	getchar();
}