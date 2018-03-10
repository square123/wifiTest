#include "matchingMySQL.h" 

//(每次处理时一定都要先创建表格，负责可能会出现无表可插的问题)

//默认的数据库test
matchingMySQL::matchingMySQL() 
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
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "project",3306, NULL, 0))   //这里的地址，用户名，密码，端口可以根据自己本地的情况更改
		cout << "mysql_real_connect() failed" << endl;  
}

//非默认的数据库
matchingMySQL::matchingMySQL(const char *host,const char *user,const char *passwd,const char *db) 
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

//析构函数
matchingMySQL::~matchingMySQL()
{
	mysql_close(&mydata);  
	mysql_server_end();  
}

void matchingMySQL::dropTable(const char *tableName)
{
	string sqlstr;
	sqlstr = "DROP TABLE ";
	sqlstr += tableName;
	sqlstr += ";";
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() drop table failed" << endl;
		mysql_close(&mydata);
	}
}


void matchingMySQL::creSmallRssiTable(const char *tableName)
{
	string sqlstr;
	sqlstr = "CREATE TABLE IF NOT EXISTS ";
	//表的名称
	sqlstr += tableName;
	sqlstr += " ";
	sqlstr += "(";
	sqlstr +=
		"time  varchar(16)  null,";  //时间
	sqlstr +=
		"mac_name varchar(20) null COMMENT 'Mac地址',"; 
	sqlstr +=
		"rssi  int(11)  null";
	sqlstr += ");";
	if (0 != mysql_query(&mydata, sqlstr.c_str()))
	{
		cout << "mysql_query() create table failed" << endl;
		mysql_close(&mydata);
	}
}

void matchingMySQL::insertSmallRssiData(const char *tableName, string & strTime, unsigned char Mumac[6], char rssi)
{
	string sqlstr = "INSERT INTO "; 
	sqlstr += tableName;
	sqlstr += " VALUES('";
	sqlstr += strTime;
	sqlstr += "', '";
	sqlstr += macToString(Mumac);
	sqlstr += "' ,";
	sqlstr += charToString(rssi);
	sqlstr += ");";
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() insert data failed" << endl;
		mysql_close(&mydata);
	}
}

void matchingMySQL::readData(const char * tableName, vector<vector<string>> &tempOut)//读取数据都从这里来找
{
	string sqlstr = "SELECT * FROM ";
	sqlstr += tableName;
	sqlstr += ";";
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
		while (NULL != row) {
			vector<string> rowTemp;
			for (int i = 0; i < fieldcount; i++) {
				rowTemp.push_back(row[i]);//应该重新定义一个数据结构
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

string matchingMySQL::charTo02XStr(unsigned char input)//将char类型转换成02X字符串型
{
	int high, low;
	char out[2] = { 0 };

	high = (input & 240) >> 4;//高四位
	low = input & 15;		//低四位
	if (high >= 10)
	{
		out[0] = (high - 10) + 'A';
	}
	else
	{
		out[0] = (high)+'0';
	}
	if (low >= 10)
	{
		out[1] = (low - 10) + 'A';
	}
	else
	{
		out[1] = (low)+'0';
	}
	string output(out, 2);//第二个参数用来控制string的长度，不然会出现乱码
	return output;
}

string matchingMySQL::macToString(unsigned char Mymac[6])//完成将char类型转换成字符串
{
	string output, temp;
	output = charTo02XStr(Mymac[0]);
	for (int i = 1; i < 6; i++)
	{
		output = output + ":" + charTo02XStr(Mymac[i]);
	}
	return output;
}

string matchingMySQL::timeToString(char timeData[14])//time转string 函数
{
	char timeDataEd[15];
	memset(timeDataEd, 0, sizeof(char) * 15);
	memcpy(timeDataEd, timeData, sizeof(char) * 14);
	stringstream stream;
	stream << timeDataEd;
	return stream.str();
}

string matchingMySQL::charToString(char x)//char 转 string 函数
{
	stringstream stream;
	stream << int(x);
	return stream.str();
}

string matchingMySQL::intToString(int x)//char 转 string 函数
{
	stringstream stream;
	stream << x;
	return stream.str();
}

string matchingMySQL::doubleToString(double x)//char 转 string 函数
{
	stringstream stream;
	stream << int(x * 1000);
	return stream.str();
}

int matchingMySQL::stringToInt(string x)
{
	int temp;
	stringstream stream;
	stream << x;
	stream >> temp;
	return temp;
}
