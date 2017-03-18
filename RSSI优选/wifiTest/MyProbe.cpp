#include "MyProbe.h"

Probe::Probe()//���ݳ�ʼ��
{
	storeIndex=0;//Ҫ���µ����ݴ洢������ֱ�Ӵ�����0��ʼ�洢
	processIndex=(storeIndex+1)%buffNum;//Ҫʹ��׼��������ţ���2��ʼ storeIndexֻ�����ʾbaseIndex������
	storeIndexBuffer=storeIndex;
	memset(zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ���mac����
	memset(zeroTimestamp,0,sizeof(char)*14);//����һ��ȫΪ���ʱ���
	zeroRssi=0;
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//���������ʼ��
	memset(syscStr,0,sizeof(syscProbe)*sameTimeMacNum*60*ProbeNum);//��baseIndex̽��ĳ�ʼ��
	memset(syscStrForIndex,0,sizeof(syscProbe)*buffNum*sameTimeMacNum);//baseIndex̽��ĳ�ʼ��
	memset(&zeroSysc,0,sizeof(syscProbe));//����洢ȫ��ΪsyscProbe��ʽ
	memset(rssiData,0,sizeof(char)*ProbeNum*sameTimeMacNum*rssiCapacity);//��ʼ��
	memset(rssiIndex,0,sizeof(int)*ProbeNum*sameTimeMacNum);
	for(int i=0;i<ProbeNum;i++)
	{
		memset(sel[i],0,sizeof(rssiTemp)*sameTimeMacNum);//�ṹ���ʼ��
		memset(timeTemp[i],0,sizeof(char)*14);//ʱ���ʼ��
		indexForPure[i]=0;//������ʼ��
		flag[i]=0;//��־λ��ʼ��
	}
	syscTime=0;
}

Probe::~Probe()//�����ر��ļ���socket�ӿ�
{
	closesocket(s);
	::WSACleanup();
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fpSysc);
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
	//�ļ���������
	fp1=fopen("probe1.csv","a+");
	fp2=fopen("probe2.csv","a+");
	fp3=fopen("probe3.csv","a+");
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
	//�ټ���һ��У�飬�������ʱ�䲻ͬʱ�ٴ��޸�
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
				rssiData[index][i][rssiIndex[index][i]]=packageData->Rssi;//norm��
				rssiIndex[index][i]++;
				indexForPure[index]++;
				break;//����ѭ��
			}
			else if(memcmp(packageData->Mumac,sel[index][i].selMumac,sizeof(unsigned char)*6)==0)//�Ƚ��Ƿ�������������,memcmp ��ͬ����0,��ͬ���ط���
			{
				rssiData[index][i][rssiIndex[index][i]]=packageData->Rssi;//norm��
				rssiIndex[index][i]++;
				//sel[index][i].maxRssi=MaxRssi(sel[index][i].maxRssi,packageData->Rssi);//ѡȡ����RSSIֵ
				sel[index][i].maxRssi=NormRssi(rssiData[index][i],rssiIndex[index][i]);//ѡȡ����RSSIֵ
				break;//����ѭ��
			}
		}
	}
	else //��ʱ�䲻һ�µ�ʱ��Ҫ������ݣ�����Ҫ�ѵ�һ����ͬ�����ݴ�����������ʱ���¼����
	{    //������ݵ��ļ�
		if(flag[index]==true)//�����ȴ浽���ݲ�ִ��д����
		{
			//���storeIndex��ͬ���ṹ�������
			if (index==baseIndex)//�����baseIndex��������
			{
				memset(syscStrForIndex[storeIndex],0,sizeof(syscProbe)*sameTimeMacNum);//��ͬ���ṹ��index�����µ�������Ϊ��
			}else//����ط�д�Ĳ����ã���ʵֻ��Ĭ��baseIndexΪ0����Ϊ����ֵ�����������Ȳ�����ô����
			{
				memset(syscStr[charTimeGetSecond(timeTemp[index])][index-1],0,sizeof(syscProbe)*sameTimeMacNum);//�����ݶ�Ӧλ������
			}
			//���沿����RSSI�Ż�Ҫ��ɵĹ���
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
				//̽��ͬ���洢����-------��Ϊ�����������baseIndex�ͷ�baseIndex
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
			//���ϴε����ݴ���buffer�У�Ϊ���ܵ���һ�뱸�ã�ע����������������ģ����ԣ����������붼����ʱ����ʵֻ��¼��һ�γ��ֵ�ֵ
			memcpy(timeBuffer[index],timeTemp[index],sizeof(char)*14);//��¼�Ѿ�������ļ��»ؿ��Բο�����
			memcpy(selBuffer[index],sel[index],sizeof(rssiTemp)*sameTimeMacNum);
			indexForPureBuf[index]=indexForPure[index];
			flag[index]=false;//������Ϊ��ֻ����һ��
			memset(sel[index],0,sizeof(rssiTemp)*sameTimeMacNum);//���ṹ������Ϊ��
			memset(rssiData[index],0,sizeof(char)*sameTimeMacNum*rssiCapacity);//��RSSIֵ�Ĵ洢����
			indexForPure[index]=0;//����
			memset(rssiIndex[index],0,sizeof(int)*sameTimeMacNum);//���洢RSSI����������
			//̽��ͬ�����ղ���
			syscProbe temp[sameTimeMacNum];//��ʹ�������յĲ��ָ�������
			if (index==baseIndex)
			{
				reduceSyscProbe(syscStrForIndex[storeIndex],temp);
				memcpy(syscStrForIndex[storeIndex],temp,sizeof(syscProbe)*sameTimeMacNum);
				//�ƽ�ͬ������ķ�չ
				storeIndex=(storeIndex+1)%buffNum;
				processIndex=(storeIndex+1)%buffNum;
			}else
			{
				reduceSyscProbe(syscStr[charTimeGetSecond(timeTemp[index])][index-1],temp);
				memcpy(syscStr[charTimeGetSecond(timeTemp[index])][index-1],temp,sizeof(syscProbe)*sameTimeMacNum);
			}
		}
		//��������Ͱѵ�һ�����ݸ�©����,���ҵ����ݷ����仯������ļ�Ҳ�ᶪ������	
		memcpy(sel[index][0].selMumac,packageData->Mumac,sizeof(unsigned char)*6);
		sel[index][0].maxRssi=packageData->Rssi;
		rssiData[index][0][rssiIndex[index][0]]=packageData->Rssi;//norm��
		rssiIndex[index][0]++;
		indexForPure[index]++;
		flag[index]=true;
		memcpy(timeTemp[index],time,sizeof(char)*14);//�ѱ仯��ʱ�丳ֵ��timeTemp
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
	char * ipaddr=NULL;
	char addr[20];
	ipaddr= inet_ntoa(clientAddr.sin_addr);//��ȡIP��ַ
	strcpy(addr,ipaddr);  
	//printf("%s\n",ipaddr);		//��ʾ�����źŵĵ�ַ
	packageData = (struct cliprobeData *) buffer;//��ʽ������ 
	char retime[14]="";//��ʼ������Ϊ��
	probeTimeFix(packageData->Timestamp,retime);//�޸�ʱ�����ʽ��ʹ����14λ

	//��̽��ʱ����ͬ����ʱ���䷵�ص�ʱ����˳�����еģ����ᳬǰ���䣬��ˣ���ֻ��¼ÿ��ʱ��洢�ı仯����
	//�൱�ڴ洢ͬһʱ�����ļ��������ǣ�ֻ�Ƚ��м���Ǹ�������Ĳ����ǣ���ʱ��ıȽϷ��ڽṹ���У���Ϊ̽�뷵�ص�ʱ�䲢����������
	//��Ҫÿ��һ��ʱ��仯�ͼ�¼һ�Σ���ʱ������¼����������һ����������������ṹ�壬����������˭Ϊ��׼
	//����Ҫ�������㣬����Ҫ���ýṹ�壬����ֱ��ͨ��������ֱ��������
	//ʵ�ָù���Ϊ�����֣�һ������RSSI��ѡ�ϸò��ָ���洢���ݽṹ�壬��һ������process�����У�ͨ��ʱ��仯�������ݽ��в������ò�����Ҫ�ɻ�׼�ͱȽ����
	//����3��Ŀ�ģ���Ϊ�˿˷�̽��һ�������Ϊ�ֻ��Ƿ����壬��Ų��Ĵ����ٶȺܿ죬������������̽����ܵ����ź�һ����ͬ��

	if(int(packageData->Rssi)>THD)//��������ֵ�ȽϺ���
	{	

		std::cout<<charToTimeInt(retime)<<std::endl;
		//Ӧ��������������ݵ�ͳһ�ϲ�

		if(addr[10]=='1')//������ǵ�ַ
		{
			std::cout<<"̽��1������"<<std::endl;
			rssiPurify(retime,fp1,0);
		}
		else if(addr[10]=='2')
		{		
			std::cout<<"̽��2������"<<std::endl;
			rssiPurify(retime,fp2,1);
		}else if(addr[10]=='3')
		{
			std::cout<<"̽��3������"<<std::endl;
			rssiPurify(retime,fp3,2);
		}

		if (storeIndex!=storeIndexBuffer)//ֻҪbaseIndex�洢��Ϻ��ȥ��������
		{
			storeIndexBuffer=storeIndex;
			std::cout<<"ͬ����������"<<std::endl;
			probeSysc(fpSysc);//ͬ��������
		}
	}
}

void Probe::probeSysc(FILE *f)//̽��ͬ������,��ͬ��������ݴ洢��һ���µı��� �����ж�ʧ��ô���������
{
	//��ʼ������
	memset(syscResult,0,sizeof(syscProbed)*sameTimeMacNum);//���ڶ�̽�뼯�ϱ��ʽ�����
	int jianshao=0;
	int timePoint=charTimeGetSecond(syscStrForIndex[processIndex][0].Timestamp);//����ʱ��
	std::cout<<"ͬ��ʱ�䣺"<<timePoint<<std::endl;
	//�жϲ���
	for (int m=0;m<sameTimeMacNum;m++)//��ѭ���ǽ�baseIndex̽����processIndexʱ���µĽṹ�������������������бȽ�
	{
		if (memcmp(&syscStrForIndex[processIndex][m],&zeroSysc,sizeof(syscProbe))==0)//Ϊ����������������
		{
			break;
		}	
		++jianshao;
		//�Ȱѱ���������
		memcpy(syscResult[m].Timestamp,syscStrForIndex[processIndex][m].Timestamp,sizeof(char)*14);
		memcpy(syscResult[m].selMumac,syscStrForIndex[processIndex][m].selMumac,sizeof(unsigned char)*6);
		syscResult[m].Rssi[baseIndex]=syscStrForIndex[processIndex][m].Rssi;

		for(int n=0;n<ProbeNum-1;n++)//��ѭ���޳�baseIndex��ֻ�Ƚ���������̽�������
		{	
			for (int k=(59+timePoint)%60;k!=(timePoint+62)%60;k=(k+61)%60)//���������ʱ�������������һ����
			{
				bool skip=0;
				for (int l=0;l<sameTimeMacNum;l++)//����һ��ʱ�����, l��ʾ���ڴ洢����
				{	
					if (memcmp(&syscStr[k][n][l],&zeroSysc,sizeof(syscProbe))==0)
					{
						break;//��Ϊ�Ѿ������ݽ����ˣ����Կ���ֱ��ʹ��break
					}
					if(timeCompare(syscStr[k][n][l].Timestamp,syscStrForIndex[processIndex][m].Timestamp,1))//���ƥ��Ͱ�RSSIֵ����
					{
						if (memcmp(syscStrForIndex[processIndex][m].selMumac,syscStr[k][n][l].selMumac,sizeof(unsigned char)*6)==0)
						{
							syscResult[m].Rssi[n+1]=syscStr[k][n][l].Rssi;//��Ϊ��һλ�Ѿ�ռ���ˣ�ֻ�ܴӵڶ�λ��ʼ
							skip=1;
							break;
						}
					}
				}
				if (skip)//������������Ҫ�Ǽ�⵽һ��ֵ����ֱ���������ʱ��
				{
					break;
				}
			}
		}
	}
	//����ļ����֣�������ֵ�����
	for (int q=0;q<jianshao;++q)
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
			//�������̽���RSSI��Ϣ����
			for (int r=0;r<ProbeNum;r++)
			{
				fprintf(fpSysc,"%d", syscResult[q].Rssi[r]);
				if(r!=ProbeNum-1)//���һ�����������
				{
					fprintf(fpSysc,",");
				}
			}
			//�������̽��Ĳ�ֵ��Ϣ����
			fprintf(fpSysc,",%d,%d,%d",syscResult[q].Rssi[0]-syscResult[q].Rssi[1],syscResult[q].Rssi[1]-syscResult[q].Rssi[2],syscResult[q].Rssi[2]-syscResult[q].Rssi[0]);
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

int Probe::charTimeGetSecond(char ttt[14])
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
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
//�����ú�������ʹ��
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