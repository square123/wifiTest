#include "wifi.h"
#include "matchingMySQL.h"

//̽����̳߳���
struct dataForProbe
{
	string macName; //���������ݱ��MAC����һ��
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
 
DWORD WINAPI rssiGetFun(LPVOID lpParameter) //�˲����ڸó�����ʹ��
{
	dataForProbe *pmd = (dataForProbe *)lpParameter;
	//�½�̽��
	Wifi wifi(pmd->macName.c_str(), pmd->macPort, pmd->rssiThd);

	//�½����ݿ�
	matchingMySQL tmpSQL("localhost",pmd->user_Name.c_str(),"1234","test");

	//�������
	tmpSQL.creSmallRssiTable(pmd->tableName.c_str());

	while (1) 
	{
		bool databaseFlag = false;
		mncatsWifi tmpVar;
		string tmpTime; //�����鴫�β��ô��� �ܹ�ȥ
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
	//д���̵߳Ķ��� �������� ���д洢�����������ݿ��ϣ�ͨ�����ݿ�Ľ������������
	vector<dataForProbe> probeSet;
	//����̽�����Ϣ�����ݿ���Ϣ
	dataForProbe tmp1("C8:E7:D8:D4:A3:02",2222,-80,"rssi_2_user","rssi1");
	probeSet.push_back(tmp1);
	//������ȡ̽��Ľ���
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
	//̽������
}