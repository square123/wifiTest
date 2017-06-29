#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
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
	struct rssiData  //��Ҫ���й����Ľṹ��Ҫת���£����ں���ȥ�Ƚ�
	{
		string macName;
		string timeName;
		int rssi[4];
	};
	struct camData   //��Ҫ���й����Ľṹ��Ҫת���£����ں���ȥ�Ƚ�
	{
		string name;
		string timeName;
		int dis[4];//Ӧ�����Ҫ�޸����� �����е�double������1000ת����int�������Ժ���ת���ӿ�
	};
	struct matchResult //�㷨������������ ��ƥ���㷨�У�����Ҫ���������ģ�ֻ���������У��ṹ����֮ǰ�ľ��У�����ýṹ���Ƿ���洢
	{
		string macName;//mac��ַ
		int score;//����
	};
public:
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
	//�㷨һ��Ҫȡ������Ȼ�����
	vector<matchResult> matchProcess(vector<camData> &cam,vector<rssiData> &rssi);
private:

	//ѡȡһ��ʱ��εĺ��� �漰����Ĳ��� ��ЩӦ�ö�������mysql�ϵı���в��������Ŷ�ȡ����
	void selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd);
	//��������Ԫ�� �ƺ�ûɶ��,��Ϊ�ܻ�ȥ�����
	
	//�ҳ����ظ���Ԫ�� ����������string �����Ӧ����Mac��ַԪ��
	vector<string> uniqueMac(const char *srcTable,const char *dstTable);
	vector<rssiData> rssiDataGet(vector<vector<string>> &tempData);//��ȡrssi���ݵĺ���
	vector<camData> camDataGet(vector<vector<string>> &tempData);//��ȡcam���ݵĺ���

	string charTo02XStr(unsigned char input);//��char����ת����02X�ַ�����
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	string timeToStrng(char timeData[14]);//timeתstring ����
	string charToString(char x);//char ת string ����
	string intToString(int x);//int ת string ����
	string doubleToString(double x);//double תstring����
	//�㷨������������
	
};

//�����ݵ�ʱ��Ӧ�õ���д�����������úܸ���

syscMySQL::syscMySQL() //Ĭ�ϵ����ݿ�
{
	//��ʼ�����ݿ�  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//��ʼ�����ݽṹ  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//���������ݿ�֮ǰ�����ö��������ѡ��  
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3306, NULL, 0))   //����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������     
		cout << "mysql_real_connect() failed" << endl;  
}

syscMySQL::syscMySQL(const char *host,const char *user,const char *passwd,const char *db) //��Ĭ�ϵ����ݿ�
{
	//��ʼ�����ݿ�  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//��ʼ�����ݽṹ  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//���������ݿ�֮ǰ�����ö��������ѡ��  
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������  
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
	//�������
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"time  varchar(14)  null,";  //ʱ��
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
	//�������
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"time  varchar(14)  null,";  //ʱ��
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

void syscMySQL::insertRssiData(const char *tableName,char Timestamp[14],unsigned char Mumac[6],char Rssi[4] )//�޸�
{
	string sqlstr="INSERT INTO ";   //����Ҫ������
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

void syscMySQL::insertCamData(const char *tableName,const char *time,int name,double d1,double d2,double d3,double d4 )//�޸�
{
	string sqlstr="INSERT INTO ";   //����Ҫ������
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
		//һ����ȡ�����ݼ�  
		result = mysql_store_result(&mydata);  
		//ȡ�ò���ӡ����  
		int rowcount = mysql_num_rows(result);  
		//ȡ�ò���ӡ���ֶε�����  
		unsigned int fieldcount = mysql_num_fields(result);    
		//��ӡ����  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		vector<string> rowTemp;
		while (NULL != row) {  
			for (int i = 0; i < fieldcount; i++) {  
				rowTemp.push_back(row[i]);//Ӧ�����¶���һ�����ݽṹ
			}  
			row = mysql_fetch_row(result);  
		}  
	}  
	else {  
		cout << "mysql_query() select data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

string syscMySQL::charTo02XStr(unsigned char input)//��char����ת����02X�ַ�����
{
	int high,low;
	char out[2]={0};

	high=(input&240)>>4;//����λ
	low=input&15;		//����λ
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
	string output(out,2);//�ڶ���������������string�ĳ��ȣ���Ȼ���������
	return output;
}

string syscMySQL::macToString(unsigned char Mymac[6])//��ɽ�char����ת�����ַ���
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+"_"+charTo02XStr(Mymac[i]);
	}
	return output;
}

string syscMySQL::timeToStrng(char timeData[14])//timeתstring ����
{
	char timeDataEd[15];
	memset(timeDataEd,0,sizeof(char)*15);
	memcpy(timeDataEd,timeData,sizeof(char)*14);
	stringstream stream;
	stream<<timeDataEd;
	return stream.str();
}

string syscMySQL::charToString(char x)//char ת string ����
{
	stringstream stream;
	stream<<int(x);
	return stream.str();
}

string syscMySQL::intToString(int x)//char ת string ����
{
	stringstream stream;
	stream<<x;
	return stream.str();
}

string syscMySQL::doubleToString(double x)//char ת string ����
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

vector<syscMySQL::rssiData> syscMySQL::rssiDataGet(vector<vector<string>> &tempData)//��ȡrssi���ݵĺ���
{

}

vector<syscMySQL::camData> syscMySQL::camDataGet(vector<vector<string>> &tempData)//��ȡcam���ݵĺ���
{

}

vector<syscMySQL::matchResult> syscMySQL::matchProcess(vector<camData> &cam,vector<rssiData> &rssi)
{

}

void syscMySQL::readData(const char *tableName,vector<vector<string>> &tempData )//��ȡtable��Ԫ�� 
{

}