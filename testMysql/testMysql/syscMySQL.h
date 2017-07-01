#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
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
		int score;//����
		int num; //��Ҫ���θ��·���
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
	void matchProcess(const char *srcCamTable,const char *srcRssiTable,const char *camName,const char *timeBegin,const char *timeEnd,vector<matchResult> &res);//��������Ĳ���
	void singleMatch(vector<camData> &src,vector<rssiData> &dst,matchResult &result);//�㷨һ��Ҫȡ������Ȼ�����
	void renewResult(vector<matchResult> &src);
private:

	//ѡȡһ��ʱ��εĺ��� �漰����Ĳ��� ��ЩӦ�ö�������mysql�ϵı���в��������Ŷ�ȡ����
	void selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd);
	//��������Ԫ�� �ƺ�ûɶ��,��Ϊ�ܻ�ȥ�����
	
	//�ҳ����ظ���Ԫ�� ����������string �����Ӧ����Mac��ַԪ��
	void uniqueMac(const char *srcTable,const string name,vector<string> &uniqueTerm);
	void rssiDataGet(const char *srcTable,const string name,vector<rssiData> &dst);//��ȡrssi���ݵĺ���
	void camDataGet(const char *srcTable,const string name,vector<camData> &dst);//��ȡcam���ݵĺ���

	string charTo02XStr(unsigned char input);//��char����ת����02X�ַ�����
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	string timeToStrng(char timeData[14]);//timeתstring ����
	string charToString(char x);//char ת string ����
	string intToString(int x);//int ת string ����
	string doubleToString(double x);//double תstring����
	int stringToInt(string x);//string תint����
	//�㷨������������
	bool resultSort(const matchResult &r1,const matchResult &r2);
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

void syscMySQL::readData(const char *tableName,vector<vector<string>>  &tempOut ) //��ȡ����ʹ�ã�һ���õĺ���
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
		while (NULL != row) {  
			vector<string> rowTemp;
			for (int i = 0; i < fieldcount; i++) {  
				rowTemp.push_back(row[i]);//Ӧ�����¶���һ�����ݽṹ
			}  
			tempOut.push_back(rowTemp);
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

int syscMySQL::stringToInt(string x)
{
	int temp;
	stringstream stream;
	stream<<x;
	stream>>temp;
	if (!stream.good()) 
	{ 
		cout<<"error transform strToInt";
	}
	return temp;
}

void syscMySQL::selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd)//��������mysql��Ĳ����������ڳ�����Ĳ���
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

void syscMySQL::uniqueMac(const char *srcTable,const string name,vector<string> &uniqueTerm) //д��ͨ�õ�
{
	string sqlstr="SELECT distinct ";
	sqlstr+=name;
	sqlstr+=" FROM ";
	sqlstr+=srcTable;  
	MYSQL_RES *result = NULL; 
	if (0 == mysql_query(&mydata, sqlstr.c_str())) 
	{
		result = mysql_store_result(&mydata); 
		unsigned int fieldcount = mysql_num_fields(result);  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			for (int i = 0; i < fieldcount; i++)
			{  
				uniqueTerm.push_back(row[i]);
			}  
			row = mysql_fetch_row(result);  
		}  
		mysql_free_result(result);  
	}
	else
	{
		cout << "mysql_query() select data failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::rssiDataGet(const char *srcTable,const string name,vector<rssiData> &dst)//��ȡrssi���ݵĺ���
{
	string sqlstr="SELECT * FROM ";
	sqlstr+=srcTable;
	sqlstr+=" WHERE macAddress='";
	sqlstr+=name;  
	sqlstr+="' order by macAddress asc;";
	MYSQL_RES *result = NULL; 
	if (0 == mysql_query(&mydata, sqlstr.c_str())) 
	{
		result = mysql_store_result(&mydata);  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			rssiData ttt;
			ttt.timeName=row[0];
			ttt.macName=row[1];
			ttt.rssi[0]=stringToInt(row[2]);
			ttt.rssi[1]=stringToInt(row[3]);
			ttt.rssi[2]=stringToInt(row[4]);
			ttt.rssi[3]=stringToInt(row[5]);
			dst.push_back(ttt);
			row = mysql_fetch_row(result);  
		}  
		mysql_free_result(result);  
	}
	else
	{
		cout << "mysql_query() select data failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::camDataGet(const char *srcTable,const string name,vector<syscMySQL::camData> &dst)//��ȡcam���ݵĺ���
{
	string sqlstr="SELECT * FROM ";
	sqlstr+=srcTable;
	sqlstr+=" WHERE name='";
	sqlstr+=name;  
	sqlstr+="' order by name asc;";
	MYSQL_RES *result = NULL; 
	if (0 == mysql_query(&mydata, sqlstr.c_str())) 
	{
		result = mysql_store_result(&mydata);  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			camData ttt;
			ttt.timeName=row[0];
			ttt.name=row[1];
			ttt.dis[0]=stringToInt(row[2]);
			ttt.dis[1]=stringToInt(row[3]);
			ttt.dis[2]=stringToInt(row[4]);
			ttt.dis[3]=stringToInt(row[5]);
			dst.push_back(ttt);
			row = mysql_fetch_row(result);  
		}  
		mysql_free_result(result);  
	}
	else
	{
		cout << "mysql_query() select data failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::singleMatch(vector<camData> &src,vector<rssiData> &dst,matchResult &result)
{

}

void syscMySQL::matchProcess(const char *srcCamTable,const char *srcRssiTable,const char *camName,const char *timeBegin,const char *timeEnd,vector<matchResult> &res)
{
	vector<camData> cam;
	//�������ݱ�
	selTimeBetween(srcCamTable,"tempCam",timeBegin,timeEnd);
	selTimeBetween(srcRssiTable,"tempRssi",timeBegin,timeEnd);
	//������ͷ���л�ȡcamName����Ϣ
	camDataGet("tempCam",camName,cam);
	//��ȡrssi�д��ڵ�mac��ַ
	vector<string> rssiName;
	uniqueMac("tempRssi","macAddress",rssiName);
	for (auto i:rssiName)
	{
		vector<rssiData> rssi;
		matchResult resSingle;
		rssiDataGet("tempRssi",i,rssi);
		singleMatch(cam,rssi,resSingle);
		res.push_back(resSingle);
	}
	//��ƥ��Ľ���������и���
	renewResult(res);
	//����
	sort(res.begin(),res.end(),resultSort);
	dropTable("tempCam");
	dropTable("tempRssi");
}

bool syscMySQL::resultSort(const matchResult &r1,const matchResult &r2)
{
	return r1.score<r2.score;
}

//��Ϊ�ڼ�¼�������Ѿ�����Ϣ�Ż�Ϊһ�룬����һ���Ӧ���������
void syscMySQL::singleMatch(vector<camData> &src1,vector<rssiData> &src2,matchResult &result)//ƥ����̵Ĺؼ���һ��Ҫȡ�������ݵĽ���
{
	//Ĭ���ڴ洢ʱ��Ԫ���Ѿ����ĺ����   ������ƥ���ٶ�ΪO(n)
	//Ĭ��camData��һֱ��ֵ�ģ�������ĳʱ����rssiֵû��camֵ���򽻼�����src2����
	//���ʵ���У�camDataû�����ݣ������ݿ�����Ҫ��ȡ������
	vector<camData> tempCam;//camData ��Ҫ��һ���洢�Ĺ��̣����Բ��ܸı������
	auto it1=src1.begin();
	auto it2=src2.begin();
	while (it1!=src1.end()&&it2!=src2.end())//��ôд��Ϊ���Է���һ����һĬ���������ﲻ��Ҳ��Ҫ�޸�
	{
		if (it1->time < it2->time)
		{
			it1++;
		}else if (it2->time < it1->time)
		{
			it2++;
		}else
		{
			tempCam.push_back(*it1);
			it1++;
			it2++;
		}
	}
	//��Ҫ����Ӧλ�ô洢������Ȼ�󹹳�һ����񣬴�ʱ����С����֪�ģ����Կ��Ը���src���ڵ�ֵ���ж�Ӧ
	//�������ŷţ�ÿ����һ��̽����̽�⵽����Ϣ 
	//�����ö�̬���鲻���ã����Ҵ��ںܶ�����ֱ࣬�Ӳ���vector���м���
// 	int *tableCam = new int [tempCam.size()*4]();
// 	int *tableRssi = new int [tempCam.size()*4]();
	vector<int> rssi4;
	vector<int> cam4;
	for (int ind=0;ind<tempCam.size();++ind) //ͨ��������������ʱ�Ѿ�һһ��Ӧ
	{
		//���ն�Ӧ���ڵ�λ�ý�camҲ�����
		if (src2[ind].rssi[0]!=0)
		{
			rssi4[0].push_back(src2[ind].rssi[0]);
			tableCam[ind*4+0]=src1[ind].dis[0];
		}
		if (src2[ind].rssi[1]!=0)
		{
			tableRssi[ind*4+1]=src2[ind].rssi[1];
			tableCam[ind*4+1]=src1[ind].dis[1];
		}
		if (src2[ind].rssi[2]!=0)
		{
			tableRssi[ind*4+2]=src2[ind].rssi[2];
			tableCam[ind*4+2]=src1[ind].dis[2];
		}
		if (src2[ind].rssi[3]!=0)
		{
			tableRssi[ind*4+3]=src2[ind].rssi[3];
			tableCam[ind*4+3]=src1[ind].dis[3];
		}	
	}
	//�����������ݵ�����ֵ,�ҳ����������


	//���бȽ�

	//��������������������

	//�ͷ��ڴ�
// 	delete[] tableCam;
// 	delete[] tableRssi;
// 	tableCam=NULL;
// 	tableRssi=NULL;
}

void syscMySQL::renewResult(vector<matchResult> &src)//���¸��������ٶԷ��������޸�
{
	//sigmoid����
}