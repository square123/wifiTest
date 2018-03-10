#include "wifi.h"
#include "matchingMySQL.h"

//探针多线程程序
struct dataForProbe
{
	string macName; //构建的数据表和MAC名字一样
	int macPort;
	int rssiThd;
	string user_Name;
	string tableName;
	dataForProbe(const char *src1, int a, int b, const char *src2,const char *src3) 
	{
		macName = src1;
		macPort = a;
		rssiThd = b;
		user_Name = src2;
		tableName = src3;
	}
};
 
DWORD WINAPI rssiGetFun(LPVOID lpParameter) //滤波不在该程序中使用
{
	dataForProbe *pmd = (dataForProbe *)lpParameter;
	//新建探针
	Wifi wifi(pmd->macName.c_str(), pmd->macPort, pmd->rssiThd);

	//新建数据库
	matchingMySQL tmpSQL("localhost",pmd->user_Name.c_str(),"1234","test");

	//创建表格
	tmpSQL.creSmallRssiTable(pmd->tableName.c_str());

	while (1) 
	{
		bool databaseFlag = false;
		mncatsWifi tmpVar;
		string tmpTime; //用数组传参不好处理 避过去
		wifi.wifiProcess(databaseFlag, tmpVar, tmpTime);
		if (databaseFlag)
		{
			tmpSQL.insertSmallRssiData(pmd->tableName.c_str(), tmpTime, tmpVar.cmac2, tmpVar.crssi);
		}
	}
	return 0;
}

int main() 
{
	int probeNumber = 1;
	//写成线程的东西 方便整理 还有存储到单独的数据库上，通过数据库的交集来完成任务
	vector<dataForProbe> probeSet;
	//设置探针的信息和数据库信息
	dataForProbe tmp1("C8:E7:D8:D4:A3:02",2222,-80,"rssi_2_user","rssi1");
	probeSet.push_back(tmp1);
	//创建读取探针的进程
	vector<HANDLE> myHandles(probeNumber);
	for (int i = 0; i < probeNumber; i++)
	{
		myHandles[i] = CreateThread(NULL, 0, rssiGetFun, &probeSet[i], 0, NULL);
		if (myHandles[i]==NULL)
		{
			ExitProcess(i);
		}
	}
	WaitForMultipleObjects(probeNumber, &myHandles[0], TRUE, INFINITE);
	for (int i = 0; i < probeNumber;i++)
	{
		CloseHandle(myHandles[i]);
	}
	//探针数据
}