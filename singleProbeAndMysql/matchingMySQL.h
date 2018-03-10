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
	MYSQL mydata; //数据库的操作数据

public:

	//数据库的初始操作
	matchingMySQL();//3308(需要根据情况修改)
	matchingMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~matchingMySQL();

	//删除表格
	void dropTable(const char *tableName);

	//主表部分
	//Rssi部分
	void creSmallRssiTable(const char *tableName); 
	void insertSmallRssiData(const char *tableName, string &strTime, unsigned char Mumac[6], char rssi);//单个的插入操作，整个操作可以用数据库的连接查询得到
	//视觉跟踪部分（暂时先不写）
	//void  creObjectTrackingTable(const char *tableName);
	//void insertTrackingData();

	//处理的辅助函数（待跟踪模块完成后补充，数据库测试 之前已经写好了）

	//读取数据
	void readData(const char * tableName,vector<vector<string>> &tempOut);//读取数据都从这里来找
private:
	//其他格式转换函数
	string charTo02XStr(unsigned char input);//将char类型转换成02X字符串型
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	string timeToString(char timeData[14]);//time转string 函数
	string charToString(char x);//char 转 string 函数
	string intToString(int x);//char 转 string 函数
	string doubleToString(double x);//char 转 string 函数
	int stringToInt(string x);

};



