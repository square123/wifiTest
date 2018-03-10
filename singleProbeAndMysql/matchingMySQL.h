#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <map>
#include <cmath>
#include <set>
using namespace std;  

class matchingMySQL
{
private:
	MYSQL mydata; //���ݿ�Ĳ�������

public:

	//���ݿ�ĳ�ʼ����
	matchingMySQL();//3308(��Ҫ��������޸�)
	matchingMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~matchingMySQL();

	//ɾ�����
	void dropTable(const char *tableName);

	//������
	//Rssi����
	void creSmallRssiTable(const char *tableName); 
	void insertSmallRssiData(const char *tableName, string &strTime, unsigned char Mumac[6], char rssi);//�����Ĳ�������������������������ݿ�����Ӳ�ѯ�õ�
	//�Ӿ����ٲ��֣���ʱ�Ȳ�д��
	//void  creObjectTrackingTable(const char *tableName);
	//void insertTrackingData();

	//����ĸ���������������ģ����ɺ󲹳䣬���ݿ���� ֮ǰ�Ѿ�д���ˣ�

	//��ȡ����
	void readData(const char * tableName,vector<vector<string>> &tempOut);//��ȡ���ݶ�����������
private:
	//������ʽת������
	string charTo02XStr(unsigned char input);//��char����ת����02X�ַ�����
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	string timeToString(char timeData[14]);//timeתstring ����
	string charToString(char x);//char ת string ����
	string intToString(int x);//char ת string ����
	string doubleToString(double x);//char ת string ����
	int stringToInt(string x);

};



