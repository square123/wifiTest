#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
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
	struct rssiData  //主要是有关数的结构需要转换下，便于后续去比较
	{
		string macName;
		string timeName;
		int rssi[4];
	};
	struct camData   //主要是有关数的结构需要转换下，便于后续去比较
	{
		string name;
		string timeName;
		int dis[4];//应该最后要修改类型 将所有的double都乘以1000转换成int，或者以后再转换接口
	};
	struct matchResult //算法最后的输出结果用 在匹配算法中，不需要考虑其他的，只考虑数就行，结构采用之前的就行，定义该结构就是方便存储
	{
		string macName;//mac地址
		int score;//分数
	};
public:
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
	//算法一定要取交集不然会出错
	vector<matchResult> matchProcess(vector<camData> &cam,vector<rssiData> &rssi);
private:

	//选取一个时间段的函数 涉及到表的操作 这些应该都是先在mysql上的表进行操作，最后才读取数据
	void selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd);
	//按行排序元素 似乎没啥用,因为总会去排序的
	
	//找出不重复的元素 这个输出的是string 输出的应该是Mac地址元素
	vector<string> uniqueMac(const char *srcTable,const char *dstTable);
	vector<rssiData> rssiDataGet(vector<vector<string>> &tempData);//读取rssi数据的函数
	vector<camData> camDataGet(vector<vector<string>> &tempData);//读取cam数据的函数

	string charTo02XStr(unsigned char input);//将char类型转换成02X字符串型
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	string timeToStrng(char timeData[14]);//time转string 函数
	string charToString(char x);//char 转 string 函数
	string intToString(int x);//int 转 string 函数
	string doubleToString(double x);//double 转string函数
	//算法中最后的排序函数
	
};

//存数据的时候应该单独写个函数，不用很复杂

syscMySQL::syscMySQL() //默认的数据库
{
	//初始化数据库  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//初始化数据结构  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3306, NULL, 0))   //这里的地址，用户名，密码，端口可以根据自己本地的情况更改     
		cout << "mysql_real_connect() failed" << endl;  
}

syscMySQL::syscMySQL(const char *host,const char *user,const char *passwd,const char *db) //非默认的数据库
{
	//初始化数据库  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//初始化数据结构  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, host, user, passwd, db,3306, NULL, 0))       
		cout << "mysql_real_connect() failed" << endl;  
}

syscMySQL::~syscMySQL()
{
	mysql_close(&mydata);  
	mysql_server_end();  
}

void syscMySQL::rssiTabCre(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//表的名称
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"time  varchar(14)  null,";  //时间
	sqlstr +=  
		"mac  varchar(20)  null,";  //Mac
	sqlstr +=  
		"rssi1  int(11)  null,";  //rssi1
	sqlstr +=  
		"rssi2  int(11)  null,";  //rssi2
	sqlstr +=  
		"rssi3  int(11)  null,";  //rssi3
	sqlstr +=  
		"rssi4  int(11)  null,";  //rssi4
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

void syscMySQL::camTabCre(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//表的名称
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"time  varchar(14)  null,";  //时间
	sqlstr +=  
		"index  varchar(20)  null,";  //index
	sqlstr +=  
		"dis1  int(11)  null,";  //dis1
	sqlstr +=  
		"dis2  int(11)  null,";  //dis2
	sqlstr +=  
		"dis3  int(11)  null,";  //dis3
	sqlstr +=  
		"dis4  int(11)  null,";  //dis4
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

void syscMySQL::dropTable(const char *tableName)
{
	string sqlstr;
	sqlstr = "DROP TABLE ";  
	sqlstr+=tableName;
	sqlstr+=";";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() drop table succeed" << endl;  
	}  
	else {  
		cout << "mysql_query() drop table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

void syscMySQL::insertRssiData(const char *tableName,char Timestamp[14],unsigned char Mumac[6],char Rssi[4] )//修改
{
	string sqlstr="INSERT INTO ";   //还需要测试下
	sqlstr+=tableName;
	sqlstr+=" VALUES(";
	sqlstr+=timeToStrng(Timestamp);
	sqlstr+=", '";
	sqlstr+=macToString(Mumac);
	sqlstr+="' ,";
	sqlstr+=charToString(Rssi[0]);
	sqlstr+=" ,";
	sqlstr+=charToString(Rssi[1]);
	sqlstr+=" ,";
	sqlstr+=charToString(Rssi[2]);
	sqlstr+=" ,";
	sqlstr+=charToString(Rssi[3]);
	sqlstr+=");";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() insert data succeed" << endl;  
	}  
	else {  
		cout << "mysql_query() insert data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

void syscMySQL::insertCamData(const char *tableName,const char *time,int name,double d1,double d2,double d3,double d4 )//修改
{
	string sqlstr="INSERT INTO ";   //还需要测试下
	sqlstr+=tableName;
	sqlstr+=" VALUES(";
	sqlstr+=time;
	sqlstr+=", ";
	sqlstr+=intToString(name);
	sqlstr+=" ,";
	sqlstr+=doubleToString(d1);
	sqlstr+=" ,";
	sqlstr+=doubleToString(d2);
	sqlstr+=" ,";
	sqlstr+=doubleToString(d3);
	sqlstr+=" ,";
	sqlstr+=doubleToString(d4);
	sqlstr+=");";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() insert data succeed" << endl;  
	}  
	else {  
		cout << "mysql_query() insert data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

void syscMySQL::readData(const char *tableName,vector<vector<string>>  &tempOut )
{
	string sqlstr="SELECT * FROM ";
	sqlstr+=tableName;
	sqlstr+=";";
	MYSQL_RES *result = NULL;  
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		//一次性取得数据集  
		result = mysql_store_result(&mydata);  
		//取得并打印行数  
		int rowcount = mysql_num_rows(result);  
		//取得并打印各字段的名称  
		unsigned int fieldcount = mysql_num_fields(result);    
		//打印各行  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		vector<string> rowTemp;
		while (NULL != row) {  
			for (int i = 0; i < fieldcount; i++) {  
				rowTemp.push_back(row[i]);//应该重新定义一个数据结构
			}  
			row = mysql_fetch_row(result);  
		}  
	}  
	else {  
		cout << "mysql_query() select data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

string syscMySQL::charTo02XStr(unsigned char input)//将char类型转换成02X字符串型
{
	int high,low;
	char out[2]={0};

	high=(input&240)>>4;//高四位
	low=input&15;		//低四位
	if (high>=10)
	{
		out[0]=(high-10)+'A';
	}
	else
	{
		out[0]=(high)+'0';
	}
	if (low>=10)
	{
		out[1]=(low-10)+'A';
	}
	else
	{
		out[1]=(low)+'0';
	}
	string output(out,2);//第二个参数用来控制string的长度，不然会出现乱码
	return output;
}

string syscMySQL::macToString(unsigned char Mymac[6])//完成将char类型转换成字符串
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+"_"+charTo02XStr(Mymac[i]);
	}
	return output;
}

string syscMySQL::timeToStrng(char timeData[14])//time转string 函数
{
	char timeDataEd[15];
	memset(timeDataEd,0,sizeof(char)*15);
	memcpy(timeDataEd,timeData,sizeof(char)*14);
	stringstream stream;
	stream<<timeDataEd;
	return stream.str();
}

string syscMySQL::charToString(char x)//char 转 string 函数
{
	stringstream stream;
	stream<<int(x);
	return stream.str();
}

string syscMySQL::intToString(int x)//char 转 string 函数
{
	stringstream stream;
	stream<<x;
	return stream.str();
}

string syscMySQL::doubleToString(double x)//char 转 string 函数
{
	stringstream stream;
	stream<<int(x*1000);
	return stream.str();
}

void syscMySQL::selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd)
{
	string sqlstr="create table ";
	sqlstr+=dstTable;
	sqlstr+=" as select * from ";
	sqlstr+=srcTable;
	sqlstr+=" where time between ";
	sqlstr+=timeBegin;
	sqlstr+=" and ";
	sqlstr+=timeEnd;
	sqlstr+=";";
}

vector<string> syscMySQL::uniqueMac(const char *srcTable,const char *dstTable)
{
	vector<string> uniqueTerm;

}

vector<syscMySQL::rssiData> syscMySQL::rssiDataGet(vector<vector<string>> &tempData)//读取rssi数据的函数
{

}

vector<syscMySQL::camData> syscMySQL::camDataGet(vector<vector<string>> &tempData)//读取cam数据的函数
{

}

vector<syscMySQL::matchResult> syscMySQL::matchProcess(vector<camData> &cam,vector<rssiData> &rssi)
{

}

void syscMySQL::readData(const char *tableName,vector<vector<string>> &tempData )//读取table的元素 
{

}