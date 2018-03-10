#include "wifi.h"

//wifi���캯������ɿͻ��˰������Ķ�ȡ��̽�����socket�Ĵ���
Wifi::Wifi(const char* setMac,int setPort, int setTHD)
{
	//���ò���
	wifiMac=setMac;
	wifiPort=setPort;
	wifiTHD=setTHD;

	isCorrected = false;

	//����������
	ifstream infile; 
	infile.open("deny.txt");   
	string sss;
	while(getline(infile,sss))//�ȶ�ȡһ�е�����
	{
		denyList.insert(sss);
	}
	infile.close();             //�ر��ļ������� 

	//��̽�����ͨ�ŵĳ�ʼ��������UDPЭ�飩
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	////����Ƕ����
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	////��Ƕ����
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(wifiPort);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 
	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
}

//�������� ���socket�Ĺر�
Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

//��¼Macֵ�Ͷ�Ӧ��RSSIֵ
void Wifi::wifiProcess(bool &frag,mncatsWifi &outData, string &correctTime)//Ϊ�������ݿ��wifi���� �����α�ɱ����
{
	//socket
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
	
	//�ѽ��պ�����ݸ�ʽ��
	outData =mncatsWifi(buffer);

	//����ʱ��
	recorrectTime(outData);

	//�޳�denyList�е�Mac��ַ���˲������ǲ��Ҳ���
	auto deIt=find(denyList.begin(),denyList.end(), outData.mac2); //����denyList�е�Ԫ�أ����û�ҵ�������end()

	//�޳�·������mac��ַ ����������֡ & �ź�ǿ�ȴ�����ֵ & �ź�ǿ�Ȳ�Ϊ0 & ���ڰ������� &  ��õ����ݱ�����̽���MAC��ַ
	if((outData.dtype!="80")&&(int(outData.crssi)>wifiTHD)&&(int(outData.crssi)!=0)&&(deIt==denyList.end())&&(outData.mac1==wifiMac)&&(int(outData.crssi)<-10)) //��Щ���ݻ���ֺ�С��ֵ�϶������ݶ���
	{
		//�����ݼ�¼�����ݿ���
		frag = true;
		//���µ�ʱ�����
		time_t timeFixInt = charToTimeInt(outData.Timestamp) + deltaTime;
		struct tm t;
		localtime_s(&t, &timeFixInt);
		char tmpTime[16] = { 0 };
		strftime(tmpTime, sizeof(char)*16, "%Y%m%d%H%M%S", &t);
		correctTime = timeToString(tmpTime);
	}
}

//����һ��ʱ����º��� ����Ҫ����
void Wifi::recorrectTime(mncatsWifi &probeData)
{
	if(!isCorrected)
	{
		time_t sysTime = time(0);
		deltaTime = sysTime - charToTimeInt(probeData.Timestamp);
		isCorrected = true;
	}
}

time_t Wifi::charToTimeInt(char ttt[14]) 
{
	time_t timeInt1;
	char year1[4], mon1[2], day1[2], hour1[2], min1[2], second1[2];
	memcpy(year1, ttt, sizeof(char) * 4);
	memcpy(mon1, ttt + 4, sizeof(char) * 2);
	memcpy(day1, ttt + 6, sizeof(char) * 2);
	memcpy(hour1, ttt + 8, sizeof(char) * 2);
	memcpy(min1, ttt + 10, sizeof(char) * 2);
	memcpy(second1, ttt + 12, sizeof(char) * 2);
	struct tm tt1;
	memset(&tt1, 0, sizeof(tt1));
	tt1.tm_year = atoi(year1) - 1900;
	tt1.tm_mon = atoi(mon1) - 1;
	tt1.tm_mday = atoi(day1);
	tt1.tm_hour = atoi(hour1);
	tt1.tm_min = atoi(min1);
	tt1.tm_sec = atoi(second1);
	timeInt1 = mktime(&tt1);
	return timeInt1;
}

string Wifi::timeToString(char timeData[14])//timeתstring ����
{
	char timeDataEd[15];
	memset(timeDataEd, 0, sizeof(char) * 15);
	memcpy(timeDataEd, timeData, sizeof(char) * 14);
	stringstream stream;
	stream << timeDataEd;
	return stream.str();
}

//�˲��㷨Ӧ�÷������ݿ⼯��֮����Ϊ��UDPЭ�� ʱ��˳��϶�����ֲ������Ŀ��ܣ����ֱ�ӽ������ݴ�����ܻ��ǻ�������⣬�����������ݿ��û��������
//���������ݿ�������Ӵ���󣬻���Ҫ����
//char Probe::NormRssi(char a[], int ccc)//ͨ����˹�������˲�
//{
//	double sum = 0.0;//���
//	double mean = 0.0;
//	double stdev = 0.0;
//	int num = 0;
//	for (int kk = 0; kk < ccc; kk++)
//	{
//		num++;
//		sum += a[kk];
//	}
//	if (1 == num)//Ҫ����������жϣ������������������жϡ�
//	{
//		return char(sum);
//	}
//	else
//	{
//		mean = sum / num;//���ֵ
//		double accum = 0.0;
//		for (int kk = 0; kk < ccc; kk++)
//		{
//			accum += (a[kk] - mean)*(a[kk] - mean);
//		}
//		stdev = sqrt(accum / num);//���׼��
//		if (stdev == 0)
//		{
//			return char(mean);
//		}
//		int num2 = 0, sum2 = 0;
//		for (int kk = 0; kk < ccc; kk++)//�޳�ֵ
//		{
//			if (myNormCdf((a[kk] - mean) / stdev) >= 0.5)
//			{
//				sum2 += a[kk];
//				++num2;
//			}
//		}
//		if (num2 == 0)
//			return char(mean);
//		return char(sum2 / num2);
//	}
//}
//
//double Probe::myErf(double x)//�������ο���������̩�չ�ʽչ�������
//{
//	double res = x;
//	double factorial = 1;	//n!
//	double x_pow = x;
//	int one = 1, n;
//	for (n = 1; n < 10; n++) {
//		factorial *= n;
//		one *= -1;
//		x_pow *= x*x;
//		res += one / factorial * x_pow / (2 * n + 1);
//	}
//	res *= 2 / sqrt(3.1416);
//	return res;
//}
//
//double Probe::myNormCdf(double x)//��˹�ֲ�����
//{
//	return (1 + myErf(x / sqrt(2))) / 2;
//}
//
//bool Probe::timeCompare(char time1[14], char time2[], int delta)//����ʱ���ǽ�����ģ�������򵥵ķ�����ֱ��ת������ȥ�жϼ��
//{
//	time_t timeInt1, timeInt2;
//	timeInt1 = charToTimeInt(time1);
//	timeInt2 = charToTimeInt(time2);
//	if ((abs(timeInt1 - timeInt2) <= delta))
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
