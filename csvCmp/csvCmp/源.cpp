#include <iostream>
#include<fstream> 
#include<time.h>//加入时间
#include "myCsv.h"
using namespace std;

time_t charToTimeInt(const char ttt[14])
{
	time_t timeInt1;
	char year1[4],mon1[2],day1[2],hour1[2],min1[2],second1[2];
	memcpy(year1,ttt,sizeof(char)*4);//把时间1分开
	memcpy(mon1,ttt+4,sizeof(char)*2);
	memcpy(day1,ttt+6,sizeof(char)*2);
	memcpy(hour1,ttt+8,sizeof(char)*2);
	memcpy(min1,ttt+10,sizeof(char)*2);
	memcpy(second1,ttt+12,sizeof(char)*2);
	struct tm tt1;
	memset(&tt1,0,sizeof(tt1));
	tt1.tm_year=atoi(year1)-1900;    //atoi函数是将字符串转换成数字类型，早知有这个函数就不用自己写了
	tt1.tm_mon=atoi(mon1)-1;    
	tt1.tm_mday=atoi(day1);    
	tt1.tm_hour=atoi(hour1);    
	tt1.tm_min=atoi(min1);    
	tt1.tm_sec=atoi(second1);    
	timeInt1=mktime(&tt1); 
	return timeInt1;
}
bool timeCompare(const char time1[14],const char time2[],int delta)//由于时间是进制类的，所以最简单的方法是直接转换到秒去判断间隔
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

int main()
{
	char filename1[] = "C://Users//Administrator//Desktop//RSSI优选//wifiTest//probe1.csv"; //文件名
	Csv csv1(filename1);//元素显示测试：
	char filename2[] = "C://Users//Administrator//Desktop//RSSI优选//wifiTest//probe2.csv"; //文件名
	Csv csv2(filename2);//元素显示测试：
	char filename3[] = "C://Users//Administrator//Desktop//RSSI优选//wifiTest//probeSysc.csv"; //文件名
	Csv csv3(filename3);//元素显示测试：
	vector <vector <string>> result;    //存储表格结构的二维vector
	string buffer1;
	string buffer2;
	string buffer3;
	for(auto it1 = csv1.table.begin(); it1 < csv1.table.end()-1; ++it1)
	{    
		auto it2 = it1 -> begin();
		string sTemp1=it2[0];	
		const char* sTchar1=sTemp1.c_str();//得到第一个数据
		string mac1=it2[1];
		string rssi1=it2[2];
		for (auto itt1=csv2.table.begin();itt1<csv2.table.end()-1;++itt1)
		{
			auto itt2 = itt1 -> begin();
			string sTemp2=itt2[0];
			const char* sTchar2=sTemp2.c_str();//得到第2个数据
			string mac2=itt2[1];
			string rssi2=itt2[2];

			if((charToTimeInt(sTchar2)-charToTimeInt(sTchar1))>5)
				break;//大于时间跳出
			if(timeCompare(sTchar2,sTchar1,1))
			{
				if(mac1==mac2)
				{
					if((buffer1==sTemp1)&&(buffer2==mac1)&&(buffer3==rssi1))
					{
					
					}
					else{
						vector <string> rsows;
						buffer1=sTemp1;
						buffer2=mac1;
						buffer3=rssi1;
						rsows.push_back(sTemp1);
						rsows.push_back(mac1);
						rsows.push_back(rssi1);
						rsows.push_back(rssi2);
						result.push_back(rsows);
					}
				}
			}
		}
	}	
	cout<<"result!"<<endl;
	//int count=0;
	////比较部分
	//for(auto resIt1=result.begin();resIt1 < result.end(); ++resIt1)
	//{	
	//	bool outflag=1;
	//	auto resIt2=resIt1->begin();
	//	string time1=resIt2[0];
	//	string mac1=resIt2[1];
	//	string rssi11=resIt2[2];
	//	string rssi12=resIt2[3];
	//	for (auto csv3It1=csv3.table.begin();csv3It1 < csv3.table.end()-1; ++csv3It1)
	//	{
	//		auto csv3It2=csv3It1->begin();
	//		string time2=csv3It2[0];
	//		string mac2=csv3It2[1];
	//		string rssi21=csv3It2[2];
	//		string rssi22=csv3It2[3];
	//		if ((time1==time2)&&(mac1==mac2)&&(rssi11==rssi21)&&(rssi12==rssi22))
	//		{
	//			outflag=0;
	//			break;
	//		}
	//	}
	//	if (outflag)
	//	{
	//		cout<<resIt2[0]<<" "<<resIt2[1]<<" "<<resIt2[2]<<" "<<resIt2[3]<<endl;
	//		++count;
	//	}
	//}
	//cout<<count<<endl;
	cout<<"finish"<<endl;
	//ofstream outfile;  
	//outfile.open("C://Users//Administrator//Desktop//RSSI优选//wifiTest//computerSysn.csv",ios::out);  
	////标准流方式输出文件
	//for(auto t1 = result.begin(); t1 != result.end(); ++t1)
	//{
	//	auto t2 = t1 -> begin();
	//	outfile<<t2[0]+","+t2[1]+","+t2[2]+","+t2[3];
	//}
	//outfile.close(); 

	//C风格输出文件方式
	FILE *fpSysc;
	fpSysc=fopen("C://Users//Administrator//Desktop//RSSI优选//wifiTest//computerSysn.csv","a+");
	for(auto t1 = result.begin(); t1 != result.end(); ++t1)
	{
		auto t2 = t1 -> begin();
		const char* a=(t2[0]).c_str();
		for (int i=0;i<14;i++) 
		{
			fprintf(fpSysc,"%c",a[i]);
		}
		fprintf(fpSysc,",");
		const char* b=(t2[1]).c_str();
		for (int i=0;i<17;i++) 
		{
			fprintf(fpSysc,"%c",b[i]);
		}
		fprintf(fpSysc,",");
		const char* c=(t2[2]).c_str();
		for (int i=0;i<3;i++) 
		{
			fprintf(fpSysc,"%c",c[i]);
		}
		fprintf(fpSysc,",");
		const char* d=(t2[3]).c_str();
		for (int i=0;i<3;i++) 
		{
			fprintf(fpSysc,"%c",d[i]);
		}
		fprintf(fpSysc,"\n");
	}
	cout<<"finish"<<endl;
	fclose(fpSysc);

	getchar();
	return 0;
}