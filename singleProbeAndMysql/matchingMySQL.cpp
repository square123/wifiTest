#include "matchingMySQL.h" 

//(ÿ�δ���ʱһ����Ҫ�ȴ�����񣬸�����ܻ�����ޱ�ɲ������)

//Ĭ�ϵ����ݿ�test
matchingMySQL::matchingMySQL() 
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
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "project",3306, NULL, 0))   //����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������
		cout << "mysql_real_connect() failed" << endl;  
}

//��Ĭ�ϵ����ݿ�
matchingMySQL::matchingMySQL(const char *host,const char *user,const char *passwd,const char *db) 
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

//��������
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
	//�������
	sqlstr += tableName;
	sqlstr += " ";
	sqlstr += "(";
	sqlstr +=
		"time  varchar(16)  null,";  //ʱ��
	sqlstr +=
		"mac_name varchar(20) null COMMENT 'Mac��ַ',"; 
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

void matchingMySQL::readData(const char * tableName, vector<vector<string>> &tempOut)//��ȡ���ݶ�����������
{
	string sqlstr = "SELECT * FROM ";
	sqlstr += tableName;
	sqlstr += ";";
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

string matchingMySQL::charTo02XStr(unsigned char input)//��char����ת����02X�ַ�����
{
	int high, low;
	char out[2] = { 0 };

	high = (input & 240) >> 4;//����λ
	low = input & 15;		//����λ
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
	string output(out, 2);//�ڶ���������������string�ĳ��ȣ���Ȼ���������
	return output;
}

string matchingMySQL::macToString(unsigned char Mymac[6])//��ɽ�char����ת�����ַ���
{
	string output, temp;
	output = charTo02XStr(Mymac[0]);
	for (int i = 1; i < 6; i++)
	{
		output = output + ":" + charTo02XStr(Mymac[i]);
	}
	return output;
}

string matchingMySQL::timeToString(char timeData[14])//timeתstring ����
{
	char timeDataEd[15];
	memset(timeDataEd, 0, sizeof(char) * 15);
	memcpy(timeDataEd, timeData, sizeof(char) * 14);
	stringstream stream;
	stream << timeDataEd;
	return stream.str();
}

string matchingMySQL::charToString(char x)//char ת string ����
{
	stringstream stream;
	stream << int(x);
	return stream.str();
}

string matchingMySQL::intToString(int x)//char ת string ����
{
	stringstream stream;
	stream << x;
	return stream.str();
}

string matchingMySQL::doubleToString(double x)//char ת string ����
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
