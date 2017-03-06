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
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp1,*fp2,*fpSysc;
	//FILE	*fp3;
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; 
	unsigned char zeroMac[6];
	char zeroTimestamp[14];
	char zeroRssi;
	Probe();
	~Probe();
	void InitProbe();
	void probeProcess();
	void probeTimeFix(char src[],char dst[14] );//�����洢�޸����ʱ�����ݸ�ʽ������ϵͳʱ������������������Ҫ��ʹ��ʱҪͬ��
	void rssiPurify(char time[14],FILE *f,int index);//����ͬһʱ���RSSI��ѡ
	void probeSysc(int baseIndex,FILE *f);//����ͬ���ṹ��ĺ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
	bool timeCompare(char time1[14],char time2[],int delta);//����ʱ������ʱ���Ƿ����delta��
	time_t charToTimeInt(char ttt[14]);//�ַ���ת����ʱ��int
	struct syscProbed
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi[ProbeNum];
	}syscResult[sameTimeMacNum];//�������ͬ����Ľ��

	//��ʵ�ֺ������̵�ȫ�ֱ�����װ��private��
private:	
	//��ѡ����
	char timeTemp[ProbeNum][14];//��������ʱ��仯�ı���
	char timeBuffer[ProbeNum][14];//ʱ��ļĴ����������洢���ϴε�ʱ��
	int indexForPure[ProbeNum];
	int indexForPureBuf[ProbeNum];//��Ϊ��һ�����ļĴ���
	bool flag[ProbeNum];

	struct rssiTemp
	{
		unsigned char selMumac[6];
		char maxRssi;
	}sel[ProbeNum][sameTimeMacNum];
	rssiTemp selBuffer[ProbeNum][sameTimeMacNum];//����һ�����ļĴ���
	//ͬ������
	time_t syscTime;	//ϵͳʱ��
	int storeIndex;//�洢���ݵ�����
	int processIndex;//����ͬ��������
	bool saveFinshFlag[ProbeNum];
	struct syscProbe	//ͬ����̽��Ľṹ��	
	{
		char Timestamp[14];
		unsigned char selMumac[6];
		char Rssi;
	}syscStr[5][ProbeNum][sameTimeMacNum];//��ά�ṹ�����飬Ҫ�Ĵ��ʱ�䡢̽��������ÿ���Ӵ洢�����ݣ�3������صķŵ�����
	time_t selectSysPrbTime(syscProbe sysc[sameTimeMacNum]);//����ṹ��ʱ��ĺ���
	void reduceSyscProbe(syscProbe sysc[sameTimeMacNum],syscProbe sysced[sameTimeMacNum]);//���ṹ�����ݽ���
	void AllreduceSyscProbe(syscProbe Allsysc[5][ProbeNum][sameTimeMacNum],syscProbe Allsysced[5][ProbeNum][sameTimeMacNum]);//�������ṹ����������
	syscProbe zeroSysc;//����洢ȫ��ΪsyscProbe��ʽ
	//char timePool[3][14];//������ʾҪ�����ʱ���
};

Probe::Probe()
{
	storeIndex=0;//Ҫ���µ����ݴ洢������ֱ�Ӵ�����0��ʼ�洢
	processIndex=3;//Ҫʹ��׼��������ţ���2��ʼ
	memset(zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ���mac����
	memset(zeroTimestamp,0,sizeof(char)*14);//����һ��ȫΪ���ʱ���
	//memset(&zeroRssi,0,sizeof(char));//����һ��ȫΪ���RSSI
	zeroRssi=0;
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//���������ʼ��
	memset(syscStr,0,sizeof(syscProbe)*sameTimeMacNum*5*ProbeNum);
	memset(&zeroSysc,0,sizeof(syscProbe));//����洢ȫ��ΪsyscProbe��ʽ
	//memset(timePool,0,sizeof(char)*3*14);
	for(int i=0;i<ProbeNum;i++)
	{
		memset(sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//�ṹ���ʼ��
		memset(timeTemp[i],0,sizeof(char)*14);//ʱ���ʼ��
		indexForPure[i]=0;//������ʼ��
		flag[i]=false;//��־λ��ʼ��
		saveFinshFlag[i]=0;//�洢��־λ��ʼ��
		//for (int j=0;j<3;j++)//ͬ���洢������ʼ��
		//{
		//	memset(&syscStr[j][i],0,sizeof(syscProbe)*sameTimeMacNum);
		//}
	}
	syscTime=0;
}

Probe::~Probe()
{
	closesocket(s);
	::WSACleanup();
	fclose(fp1);
	fclose(fp2);
	fclose(fpSysc);
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
	fpSysc=fopen("probeSysc.csv","a+");
}


void Probe::probeTimeFix(char src[],char dst[14] ) //�����洢�޸����ʱ�����ݸ�ʽ������ϵͳʱ������������������Ҫ��ʹ��ʱҪͬ��
{
	time_t tt=time(NULL);
	tm* t=localtime(&tt);
	char endPos=0;
	int endIndex=0;
	for (int k=0;k<14;k++)//������һλ
	{
		if (src[k]==0)
		{
			endPos=src[k-1];
			endIndex=k-1;
			break;
		}
	}
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
	if ((dst[13]==0))
	{
		dst[13]=dst[12];
		dst[12]='0';
	}
	//�ú���������
	//�ټ���һ��У�飬�������ʱ�䲻ͬʱ
	if (endPos!=dst[13])
	{
		dst[13]=src[endIndex];
		dst[12]=src[endIndex-1];
	}
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
	else //��ʱ�䲻һ�µ�ʱ��Ҫ������ݣ�����Ҫ�ѵ�һ����ͬ�����ݴ�����������ʱ���¼����
	{    //������ݵ��ļ�
		if(flag[index]==true)//�����ȴ浽���ݲ�ִ��д����
		{
			//���storeIndex��ͬ���ṹ�������
			memset(syscStr[storeIndex][index],0,sizeof(syscProbe)*sameTimeMacNum);//��ͬ���ṹ��index�����µ�������Ϊ��
			//
			bool douflag=timeCompare(timeBuffer[index],timeTemp[index],1);//д���������������
			for(int j=0;j<indexForPure[index];j++)//ʹ����������ż�������
			{
				//�����ݼ���һ��Buffer�����Զ����ݽ���ɸѡ������ȫ���棬ֻҪ��MAC����Ӧ�þ���
				//���һ��������������ʱ��������Ϊһ�룬�Ž����Ƿ��ظ����ж�
				//��������������жϲ���
				if (douflag)//�Ѿ����ֵ�Mac�벻���ڵڶ����������������ݲ����ˣ���Ϊ�Ѿ������
				{
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
						continue;
					}
				}
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
				//̽��ͬ���洢����
				memcpy(syscStr[storeIndex][index][j].Timestamp,timeTemp[index],sizeof(char)*14);
				memcpy(syscStr[storeIndex][index][j].selMumac,sel[index][j].selMumac,sizeof(unsigned char)*6);
				syscStr[storeIndex][index][j].Rssi=sel[index][j].maxRssi;
				//
			}
			//���ϴε����ݴ���buffer�У�Ϊ���ܵ���һ�뱸�ã�ע����������������ģ����ԣ����������붼����ʱ����ʵֻ��¼��һ�γ��ֵ�ֵ
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//��¼�Ѿ�������ļ��»ؿ��Բο�����
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			//
			flag[index]=false;//������Ϊ��ֻ����һ��
			memset(sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//���ṹ������Ϊ��
			indexForPure[index]=0;//����
			saveFinshFlag[index]=1;
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
	//socketͨ�Ű󶨲���
	bool enable=true;
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//bool syscZeroEnable=true;//��¼ϵͳʱ��ʹ�ܶ�
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
	//printf("%s\n",ipaddr);		//��ʾ�����źŵĵ�ַ
	packageData = (struct cliprobeData *) buffer;//��ʽ������ 
	char retime[14]="";//��ʼ������Ϊ��
	probeTimeFix(packageData->Timestamp,retime);//�޸�ʱ�����ʽ��ʹ����14λ
	//printf("�޸���ʽǰ��ʱ�䣺\n");
	//for (int ii=0;ii<14;ii++)
	//{
	//	printf("%c",packageData->Timestamp[ii]);
	//}
	//printf("\n");
	//printf("�޸���ʽ���ʱ�䣺\n");
	//for (int ii=0;ii<14;ii++)
	//{
	//	printf("%c",retime[ii]);
	//}
	//printf("\n");
	//��̽��ʱ����ͬ����ʱ���䷵�ص�ʱ����˳�����еģ����ᳬǰ���䣬��ˣ���ֻ��¼ÿ��ʱ��洢�ı仯����
	//�൱�ڴ洢ͬһʱ�����ļ��������ǣ�ֻ�Ƚ��м���Ǹ�������Ĳ����ǣ���ʱ��ıȽϷ��ڽṹ���У���Ϊ̽�뷵�ص�ʱ�䲢����������
	//��Ҫÿ��һ��ʱ��仯�ͼ�¼һ�Σ���ʱ������¼����������һ����������������ṹ�壬����������˭Ϊ��׼
	//����Ҫ�������㣬����Ҫ���ýṹ�壬����ֱ��ͨ��������ֱ��������
	//ʵ�ָù���Ϊ�����֣�һ������RSSI��ѡ�ϸò��ָ���洢���ݽṹ�壬��һ������process�����У�ͨ��ʱ��仯�������ݽ��в������ò�����Ҫ�ɻ�׼�ͱȽ����
	//����3��Ŀ�ģ���Ϊ�˿˷�̽��һ�������Ϊ�ֻ��Ƿ����壬��Ų��Ĵ����ٶȺܿ죬������������̽����ܵ����ź�һ����ͬ��

	if(int(packageData->Rssi)>THD)//��������ֵ�ȽϺ���
	{	
	/*	for (int ii=0;ii<14;ii++) 
		{
			printf("%c", retime[ii]);
		}
		printf("\n");*/
		cout<<charToTimeInt(retime)<<endl;
		//Ӧ��������������ݵ�ͳһ�ϲ�
		
		if(addr[10]=='1')//������ǵ�ַ
		{
			cout<<"̽��1������"<<endl;
			rssiPurify(retime,fp1,0);
		}
		else if(addr[10]=='2')
		{
			
			cout<<"̽��2������"<<endl;
			rssiPurify(retime,fp2,1);
		}
		//�ò�����ʱû��
		for (int mm=0;mm<ProbeNum;mm++)//����Ҫ���������洢��///��������������������������������������Ҫ��֤�����ٻ�����˵
		{
			enable=enable&&saveFinshFlag[mm];
		}
		if (enable)//���ʱ�䲻ͬʱ������־λ������(syscTime<charToTimeInt(retime))&&
		{
			syscTime=charToTimeInt(retime);
			cout<<"ͬ������Ϊ"<<syscTime<<endl;
			/*	printf("Ҫͬ����ʱ���: ");
			for (int man=0;man<3;man++)
			{
			for (int ii=0;ii<14;ii++) 
			{
			printf("%c", timePool[man][ii]);
			}
			printf("  ");
			}
			printf("\n");
			printf("Ҫ�����ʱ�䣺 ");
			for (int ii=0;ii<14;ii++) 
			{
			printf("%c", timePool[processIndex][ii]);
			}
			printf("\n");
			memcpy(timePool[storeIndex],retime,sizeof(char)*14);*/
			probeSysc(0,fpSysc);//ͬ��������
			for (int mm=0;mm<ProbeNum;mm++)//�������������0///�о�����������~~~~��������ݴ������ʧ�����⣬Ӧ�û���Ҫ�޸ģ��޸���break�ط��ϣ���������ʱ������ж���
			{
				saveFinshFlag[mm]=0;
			}
			enable=1;//��������
			storeIndex=(storeIndex+1)%5;//�洢������1
			processIndex=(processIndex+1)%5;//����������1
			
		}
	}
}
		
void Probe::probeSysc(int baseIndex,FILE *f)//̽��ͬ������,��ͬ��������ݴ洢��һ���µı��� �����ж�ʧ��ô���������
{
	//��ʼ������
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//���ڶ�̽�뼯�ϱ��ʽ�����

	syscProbe syscStrEd[5][ProbeNum][sameTimeMacNum];//�������ڴ洢���պ�ı���
	memset(syscStrEd,0,(sizeof(syscProbe)*5*ProbeNum*sameTimeMacNum));//���&��ʼ��

	AllreduceSyscProbe(syscStr,syscStrEd);//ͨ���ú��������ݽ���һЩ
	int jianshao=0;
	//�жϲ���
	for (int m=0;m<sameTimeMacNum;m++)//��ѭ���ǽ�baseIndex̽����processIndexʱ���µĽṹ�������������������бȽ�
	{
		if (memcmp(&syscStrEd[processIndex][baseIndex][m],&zeroSysc,sizeof(syscProbe))==0)//Ϊ����������������
		{
			break;
		}	
		jianshao++;
		//�Ȱѱ���������
		memcpy(syscResult[m].Timestamp,syscStrEd[processIndex][baseIndex][m].Timestamp,sizeof(char)*14);
		memcpy(syscResult[m].selMumac,syscStrEd[processIndex][baseIndex][m].selMumac,sizeof(unsigned char)*6);
		syscResult[m].Rssi[baseIndex]=syscStrEd[processIndex][baseIndex][m].Rssi;
		for(int n=0;n<ProbeNum;n++)//��ѭ�����޳�baseIndex������������������̽��Ƚ�,n��ʾ̽������
		{
			if(n==baseIndex)//������׼����
			{
				continue;
			}	
			for (int k=0;k<5;k++)//����ʱ��,��������һ��С�Ķ���ϣ���������ʱ�俪ʼ��¼���ͽ�k���((processIndex+2+k)%3)��Ĭ�����ȵ�����á�������������������Ҫ��֤
			{	
				bool skip=false;
				if (!(timeCompare(syscStrEd[((processIndex+2+k)%5)][n][0].Timestamp,syscStrEd[processIndex][baseIndex][0].Timestamp,1)))//��ʱ�䲻�ϸ�ʱ��ֱ������,��Ϊһ���ʱ������ͬ�ģ�������0�Ϳ���
				{
					continue;//ͨ�����ж�ȥ��ʱ�䲻�ϸ�ı���
				}
				for (int l=0;l<sameTimeMacNum;l++)//����һ��ʱ�����, l��ʾ���ڴ洢����
				{	
					if (memcmp(&syscStrEd[((processIndex+2+k)%5)][n][l],&zeroSysc,sizeof(syscProbe))==0)
					{
						break;//��Ϊ�Ѿ������ݽ����ˣ����Կ���ֱ��ʹ��break
					}
					if(memcmp(syscStrEd[processIndex][baseIndex][m].selMumac,syscStrEd[((processIndex+2+k)%5)][n][l].selMumac,sizeof(unsigned char)*6)==0)//���ƥ��Ͱ�RSSIֵ����
					{
						syscResult[m].Rssi[n]=syscStrEd[((processIndex+2+k)%5)][n][l].Rssi;
						skip=true;
						break;
					}
				}
				if (skip)//������������Ҫ�Ǽ�⵽һ��ֵ����ֱ���������ʱ��
				{
					break;
				}
			}
		}
	}
	//������֣�������ֵ�����
	for (int q=0;q<jianshao;q++)
	{
		bool outflag=1;
		outflag=outflag&&memcmp(syscResult[q].selMumac,zeroMac,sizeof(unsigned char)*6);//��һ���ͺ�
		for (int r=0;r<ProbeNum;r++)
		{
			outflag=outflag&&memcmp(&syscResult[q].Rssi[r],&zeroRssi,sizeof(char));
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
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpSysc,"%d", syscResult[q].Rssi[r]);
				if(r!=ProbeNum-1)//���һ�����������
				{
					fprintf(fpSysc,",");
				}
			}
			fprintf(fpSysc,"\n");
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

time_t Probe::selectSysPrbTime(syscProbe sysc[sameTimeMacNum])//���syscProbe�ṹ���ʱ�䣬����Ū�ıȽϸ�����
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
	memset(sysced,0,sizeof(syscProbe)*sameTimeMacNum);//������ṹ����������
	int j=0;
	for (int i=0;i<sameTimeMacNum;i++)
	{
		if (memcmp(&zeroSysc,&sysc[i],sizeof(syscProbe))!=0)//�����Ϊ�գ��ͱ������µ����ݼ�����
		{
			//memcpy(&sysced[j],&sysc[i],sizeof(syscProbe));//���д�Ĳ��ԣ�����ȡ��ַ
			memcpy(sysced[j].selMumac,sysc[i].selMumac,sizeof(unsigned char)*6);
			memcpy(sysced[j].Timestamp,sysc[i].Timestamp,sizeof(char)*14);
			sysced[j].Rssi=sysc[i].Rssi;
			j++;
		}
	}
}

void Probe::AllreduceSyscProbe(syscProbe Allsysc[5][ProbeNum][sameTimeMacNum],syscProbe Allsysced[5][ProbeNum][sameTimeMacNum])
{
	for (int i=0;i<5;i++)
	{
		for (int j=0;j<ProbeNum;j++)
		{
			reduceSyscProbe(Allsysc[i][j],Allsysced[i][j]);
		}
	}
}

int main(int argc,char*argv[])
{
	Probe probe;
	probe.InitProbe();
	while(1)
	{
		probe.probeProcess();
		if( GetKeyState( VK_ESCAPE ) < 0 )
		{
			probe.~Probe();
		}
	}  
}

//memset,memcpy.memcmp,bug�ܽᣬ������Ŀ��Բ�ȡ��ַ����Ϊ���鱾�����ָ�룬���Դ��ݣ��������࣬�ṹ�����Ҫȡ��ַ�����ܽ��е�ַ����


//���������պ���󣬲��Ұ�C++���¹�һ���һ��Ҫ��ʱ��ѳ�������дһ�飬���������ʹ�á�