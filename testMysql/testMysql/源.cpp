#include <winsock.h>  
#include <iostream>  
#include <string>  
#include <mysql.h>  
using namespace std;  

 
int main() {  

//数据库初始化部分
	//必备的一个数据结构  
	MYSQL mydata;  
	//初始化数据库  
	if (0 != mysql_library_init(0, NULL, NULL)) 
	{  
		cout << "mysql_library_init() failed" << endl;  
		return -1;  
	}  
	//初始化数据结构  
	if (NULL == mysql_init(&mydata))
	{  
		cout << "mysql_init() failed" << endl;  
		return -1;  
	}  
	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
	{  
		cout << "mysql_options() failed" << endl;  
		return -1;  
	}  

//连接数据库  
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3306, NULL, 0))   //这里的地址，用户名，密码，端口可以根据自己本地的情况更改     
	{  
		cout << "mysql_real_connect() failed" << endl;  
		return -1;  
	}  

//操作数据库
	//sql字符串  
	string sqlstr;  
	//创建一个表  
	sqlstr = "CREATE TABLE IF NOT EXISTS probedata ";  //表的名称
	sqlstr += "(";  
	sqlstr +=  
		"time  varchar(16)  null,";  //第一列
	sqlstr +=  
		"mac  varchar(20)  null,";  //第二列
	sqlstr +=  
		"rssi1  int(11)  null,";  //第三列
	sqlstr +=  
		"rssi2  int(11)  null,";  //第四列
	sqlstr +=  
		"rssi3  int(11)  null,";  //第五列
	sqlstr +=  
		"rssi4  int(11)  null,";  //第六列
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
		return -1;  
	}  


	//向表中插入数据  
	sqlstr =  
		"INSERT INTO user_info(user_name) VALUES('公司名称'),('一级部门'),('二级部门'),('开发小组'),('姓名');";  
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() insert data succeed" << endl;  
	}  
	else {  
		cout << "mysql_query() insert data failed" << endl;  
		mysql_close(&mydata);  
		return -1;  
	}  
 

	//在控制台显示刚才插入的数据  
	sqlstr = "SELECT user_id,user_name,user_second_sum FROM user_info";  
	MYSQL_RES *result = NULL;  
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() select data succeed" << endl;  

		//一次性取得数据集  
		result = mysql_store_result(&mydata);  
		//取得并打印行数  
		int rowcount = mysql_num_rows(result);  
		cout << "row count: " << rowcount << endl;  

		//取得并打印各字段的名称  
		unsigned int fieldcount = mysql_num_fields(result);  
		MYSQL_FIELD *field = NULL;  
		for (unsigned int i = 0; i < fieldcount; i++) {  
			field = mysql_fetch_field_direct(result, i);  
			cout << field->name << "\t\t";  
		}  
		cout << endl;  

		//打印各行  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			for (int i = 0; i < fieldcount; i++) {  
				cout << row[i] << "!!\t\t";  
			}  
			cout << endl;  
			row = mysql_fetch_row(result);  
		}  

	}  
	else {  
		cout << "mysql_query() select data failed" << endl;  
		mysql_close(&mydata);  
		return -1;  
	}  



	////删除刚才建的表  
	//sqlstr = "DROP TABLE user_info";  
	//if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
	//	cout << "mysql_query() drop table succeed" << endl;  
	//}  
	//else {  
	//	cout << "mysql_query() drop table failed" << endl;  
	//	mysql_close(&mydata);  
	//	return -1;  
	//} 


	mysql_free_result(result);  
	mysql_close(&mydata);  
	mysql_server_end();  

	system("pause");  
	return 0;  
}