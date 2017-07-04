#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
using namespace std;  
//���ڶ�д���⣬���Ƿ�ʱ��δ��������Ȳ����ǣ����Խ������ȴ�һ���ֵ����أ���ÿ�����ݶ����ڴ洢���ٴ���

//Ҫ����һ����ʵ����������Mysql���ݿ�֮�ϵĲ��������������д�ǲ��������еģ�����ֻ�ǱȽϷ���Ľ�һЩ���ӵĲ�����װ�����У����㴦��������ʵ��
//���Ƕ�һ�����е����ݿ�������˴��ǲ�����ŵġ�

//���ս����ĸ����࣬ʵ���Ͼ���Ϊ�˼������ݿ���в����������ݿ�Ķ�д���������߳��кͶ�̨��������Ҫ����ġ�

//���������ͬ��ʱʹ�õģ���Ҫ��ƥ���㷨Ҳ�������棬�ȼ�������int�ͣ��Ժ��ٿ��ǳ�һ��Ψһ���� hesh

class syscMySQL
{
private:
	MYSQL mydata; //���ݿ�Ĳ���

public:
	struct rssiData  //��Ҫ���й����Ľṹ��Ҫת���£����ں���ȥ�Ƚ�
	{
		string macName;
		string time;
		int rssi[4];
	};
	struct camData   //��Ҫ���й����Ľṹ��Ҫת���£����ں���ȥ�Ƚ�
	{
		string name;
		string time;
		int dis[4];//Ӧ�����Ҫ�޸����� �����е�double������1000ת����int�������Ժ���ת���ӿ�
	};
	struct matchResult //�㷨������������ ��ƥ���㷨�У�����Ҫ���������ģ�ֻ���������У��ṹ����֮ǰ�ľ��У�����ýṹ���Ƿ���洢
	{
		string macName;//mac��ַ
		double score;//����
		int num; //��Ҫ���θ��·���
	};
	syscMySQL();
	syscMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~syscMySQL();
	void camTabCre(const char *tableName);//����camera table
	void rssiTabCre(const char *tableName);//����rssi table
	void dropTable(const char *tableName);//ɾ�� table
	void insertRssiData(const char *tableName,char Timestamp[14],unsigned char Mumac[6],char Rssi[4]);//����rssi���� �����Ӧ���������Ľṹ
	void insertCamData(const char *tableName,const char *time,int name,double d1,double d2,double d3,double d4 );//����cam����  �����Ӧ���������Ľṹ


	void readData(const char *tableName,vector<vector<string>> &tempData );//��ȡtable��Ԫ�� 

	//ƥ���㷨 �����Ӧ����һ������  vector<matchResult> ����Mac��ַ�ͷ��� �����ܶ�Ԫ�� ����㷨Ӧ��������ĳɷ�������
	//��������Ӧ����һ����������ݺͶ��̽������ݣ��ں�����Ӧ�û���Ҫ�����������Ӧ�������������
	void matchProcess(const char *srcCamTable,const char *srcRssiTable,const char *camName,const char *timeBegin,const char *timeEnd,vector<matchResult> &res);//��������Ĳ���
	void singleMatch(vector<camData> &src,vector<rssiData> &dst,matchResult &result);//�㷨һ��Ҫȡ������Ȼ�����
	vector<int> rssiFeatureTran(vector<int> &src,int thd);
	vector<int> camFeatureTran(vector<int> &src,int thd);
	vector<int> minusFeatureTran(vector<int> &src1,vector<int> &src2);
	void renewResult(vector<matchResult> &src);
	void uniqueMac(const char *srcTable,const char *name,vector<string> &uniqueTerm);//�ҳ����ظ���Ԫ�� ����������string �����Ӧ����Mac��ַԪ��


private:
	
	void selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd);//ѡȡһ��ʱ��εĺ��� �漰����Ĳ��� ��ЩӦ�ö�������mysql�ϵı���в��������Ŷ�ȡ����	
	void rssiDataGet(const char *srcTable,const string name,vector<rssiData> &dst);//��ȡrssi���ݵĺ���
	void camDataGet(const char *srcTable,const char* name,vector<camData> &dst);//��ȡcam���ݵĺ���
	
	string charTo02XStr(unsigned char input);//��char����ת����02X�ַ�����
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	string timeToStrng(char timeData[14]);//timeתstring ����
	string charToString(char x);//char ת string ����
	string intToString(int x);//int ת string ����
	string doubleToString(double x);//double תstring����
	int stringToInt(string x);//string תint����

	//�㷨������������
/*	bool resultSort(const matchResult &r1,const matchResult &r2);//���Բ���lamda���ʽ������*/
};



