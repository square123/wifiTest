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
//关于读写问题，考虑分时间段处理，现在先不考虑，可以将数据先存一部分到本地，让每个数据都能在存储后再处理。

//要认清一个事实，该类是在Mysql数据库之上的操作，所以其读与写是不考虑其中的，该类只是比较方便的将一些复杂的操作封装到类中，方便处理。整个类实际
//就是对一个现有的数据库操作，彼此是不会干扰的。

//最终建立的各种类，实际上就是为了简便对数据库进行操作，而数据库的读写问题是在线程中和多台机器中需要处理的。

//该类是针对同步时使用的，需要将匹配算法也放在里面，先假设标号是int型，以后再考虑出一个唯一的码 hesh

class syscMySQL
{
private:
	MYSQL mydata; //数据库的操作

public:
	struct rssiData  //主要是有关数的结构需要转换下，便于后续去比较
	{
		string macName;
		string time;
		int rssi[4];
	};
	struct camData   //主要是有关数的结构需要转换下，便于后续去比较
	{
		string name;
		string time;
		int dis[4];//应该最后要修改类型 将所有的double都乘以1000转换成int，或者以后再转换接口
	};
	struct matchResult //算法最后的输出结果用 在匹配算法中，不需要考虑其他的，只考虑数就行，结构采用之前的就行，定义该结构就是方便存储
	{
		string macName;//mac地址
		double score;//分数
		int num; //需要两次更新分数
	};
	syscMySQL();
	syscMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~syscMySQL();
	void camTabCre(const char *tableName);//创建camera table
	void rssiTabCre(const char *tableName);//创建rssi table
	void dropTable(const char *tableName);//删除 table
	void insertRssiData(const char *tableName,char Timestamp[14],unsigned char Mumac[6],char Rssi[4]);//插入rssi数据 输入的应该是正常的结构
	void insertCamData(const char *tableName,const char *time,int name,double d1,double d2,double d3,double d4 );//插入cam数据  输入的应该是正常的结构


	void readData(const char *tableName,vector<vector<string>> &tempData );//读取table的元素 

	//匹配算法 输出的应该是一个向量  vector<matchResult> 包括Mac地址和分数 包含很多元素 输出算法应该有排序的成分在里面
	//最后的输入应该是一个人物的数据和多个探针的数据，在函数中应该还需要处理，最后的输出应该是排序后的输出
	void matchProcess(const char *srcCamTable,const char *srcRssiTable,const char *camName,const char *timeBegin,const char *timeEnd,vector<matchResult> &res);//单独排序的操作
	void singleMatch(vector<camData> &src,vector<rssiData> &dst,matchResult &result);//算法一定要取交集不然会出错
	vector<int> rssiFeatureTran(vector<int> &src,int thd);
	vector<int> camFeatureTran(vector<int> &src,int thd);
	vector<int> minusFeatureTran(vector<int> &src1,vector<int> &src2);
	void renewResult(vector<matchResult> &src);
	void uniqueMac(const char *srcTable,const char *name,vector<string> &uniqueTerm);//找出不重复的元素 这个输出的是string 输出的应该是Mac地址元素


private:
	
	void selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd);//选取一个时间段的函数 涉及到表的操作 这些应该都是先在mysql上的表进行操作，最后才读取数据	
	void rssiDataGet(const char *srcTable,const string name,vector<rssiData> &dst);//读取rssi数据的函数
	void camDataGet(const char *srcTable,const char* name,vector<camData> &dst);//读取cam数据的函数
	
	string charTo02XStr(unsigned char input);//将char类型转换成02X字符串型
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	string timeToStrng(char timeData[14]);//time转string 函数
	string charToString(char x);//char 转 string 函数
	string intToString(int x);//int 转 string 函数
	string doubleToString(double x);//double 转string函数
	int stringToInt(string x);//string 转int函数

	//算法中最后的排序函数
/*	bool resultSort(const matchResult &r1,const matchResult &r2);//可以采用lamda表达式来处理*/
};



