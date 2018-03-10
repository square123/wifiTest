#include "wifi.h"

//wifi构造函数（完成客户端白名单的读取和探针接受socket的处理）
Wifi::Wifi(const char* setMac,int setPort, int setTHD)
{
	//设置参数
	wifiMac=setMac;
	wifiPort=setPort;
	wifiTHD=setTHD;

	isCorrected = false;

	//白名单机制
	ifstream infile; 
	infile.open("deny.txt");   
	string sss;
	while(getline(infile,sss))//先读取一行的数据
	{
		denyList.insert(sss);
	}
	infile.close();             //关闭文件输入流 

	//与探针进行通信的初始化操作（UDP协议）
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	////创建嵌套字
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	////绑定嵌套字
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

//析构函数 完成socket的关闭
Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

//记录Mac值和对应的RSSI值
void Wifi::wifiProcess(bool &frag,mncatsWifi &outData, string &correctTime)//为了让数据库和wifi独立 将传参变成别的数
{
	//socket
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//定义接收
	if (recvfrom(s,buffer,BUFFER_SIZE,0,(SOCKADDR *)&clientAddr,&clientAddrLength) == SOCKET_ERROR)
	{
		printf("recvfrom() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	
	//把接收后的数据格式化
	outData =mncatsWifi(buffer);

	//修正时间
	recorrectTime(outData);

	//剔除denyList中的Mac地址，此布操作是查找操作
	auto deIt=find(denyList.begin(),denyList.end(), outData.mac2); //查找denyList中的元素，如果没找到，返回end()

	//剔除路由器的mac地址 必须是数据帧 & 信号强度大于阈值 & 信号强度不为0 & 不在白名单中 &  获得的数据必须是探针的MAC地址
	if((outData.dtype!="80")&&(int(outData.crssi)>wifiTHD)&&(int(outData.crssi)!=0)&&(deIt==denyList.end())&&(outData.mac1==wifiMac)&&(int(outData.crssi)<-10)) //有些数据会出现很小的值肯定是数据丢了
	{
		//将数据记录到数据库中
		frag = true;
		//将新的时间更正
		time_t timeFixInt = charToTimeInt(outData.Timestamp) + deltaTime;
		struct tm t;
		localtime_s(&t, &timeFixInt);
		char tmpTime[16] = { 0 };
		strftime(tmpTime, sizeof(char)*16, "%Y%m%d%H%M%S", &t);
		correctTime = timeToString(tmpTime);
	}
}

//加入一个时间更新函数 开机要更新
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

string Wifi::timeToString(char timeData[14])//time转string 函数
{
	char timeDataEd[15];
	memset(timeDataEd, 0, sizeof(char) * 15);
	memcpy(timeDataEd, timeData, sizeof(char) * 14);
	stringstream stream;
	stream << timeDataEd;
	return stream.str();
}

//滤波算法应该放在数据库集合之后，因为是UDP协议 时间顺序肯定会出现不连续的可能，如果直接进行数据处理可能还是会出现问题，那样存入数据库就没有意义了
//所以在数据库进行连接处理后，还需要排序
//char Probe::NormRssi(char a[], int ccc)//通过高斯函数来滤波
//{
//	double sum = 0.0;//求和
//	double mean = 0.0;
//	double stdev = 0.0;
//	int num = 0;
//	for (int kk = 0; kk < ccc; kk++)
//	{
//		num++;
//		sum += a[kk];
//	}
//	if (1 == num)//要加入个数的判断，至少有两个数才能判断。
//	{
//		return char(sum);
//	}
//	else
//	{
//		mean = sum / num;//求均值
//		double accum = 0.0;
//		for (int kk = 0; kk < ccc; kk++)
//		{
//			accum += (a[kk] - mean)*(a[kk] - mean);
//		}
//		stdev = sqrt(accum / num);//求标准差
//		if (stdev == 0)
//		{
//			return char(mean);
//		}
//		int num2 = 0, sum2 = 0;
//		for (int kk = 0; kk < ccc; kk++)//剔除值
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
//double Probe::myErf(double x)//误差函数，参考网上利用泰勒公式展开求积分
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
//double Probe::myNormCdf(double x)//高斯分布函数
//{
//	return (1 + myErf(x / sqrt(2))) / 2;
//}
//
//bool Probe::timeCompare(char time1[14], char time2[], int delta)//由于时间是进制类的，所以最简单的方法是直接转换到秒去判断间隔
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
