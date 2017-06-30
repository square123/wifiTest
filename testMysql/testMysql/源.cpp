#include <winsock.h>  
#include <iostream>  
#include <string>
#include <vector>
#include <sstream> //用于转换的char类型到string型
#include <mysql.h>  
using namespace std;  

//#pragma comment(lib, "ws2_32.lib")  
//#pragma comment(lib, "libmysql.lib")  

//单步执行，不想单步执行就注释掉  
#define STEPBYSTEP  

string charTo02XStr(unsigned char input)//将char类型转换成02X字符串型
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

string macToString(unsigned char Mymac[6])//完成将char类型转换成字符串
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+":"+charTo02XStr(Mymac[i]);
	}
	return output;
}

int main() {  
	cout << "****************************************" << endl;  

#ifdef STEPBYSTEP     //条件编译，作用：如果在此之前已定义了这样的宏名，则编译语句段。
	system("pause");  
#endif  

	//必备的一个数据结构  
	MYSQL mydata;  

	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL)) {  
		cout << "mysql_library_init() succeed" << endl;  
	}  
	else {  
		cout << "mysql_library_init() failed" << endl;  
		return -1;  
	}  

#ifdef STEPBYSTEP  
	system("pause");  
#endif  

	//初始化数据结构  
	if (NULL != mysql_init(&mydata)) {  
		cout << "mysql_init() succeed" << endl;  
	}  
	else {  
		cout << "mysql_init() failed" << endl;  
		return -1;  
	}  

#ifdef STEPBYSTEP  
	system("pause");  
#endif  

	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {  
		cout << "mysql_options() succeed" << endl;  
	}  
	else {  
		cout << "mysql_options() failed" << endl;  
		return -1;  
	}  

#ifdef STEPBYSTEP  
	system("pause");  
#endif  

	//连接数据库  
	if (NULL!= mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3306, NULL, 0))  
		//这里的地址，用户名，密码，端口可以根据自己本地的情况更改  
	{  
		cout << "mysql_real_connect() succeed" << endl;  
	}  
	else {  
		cout << "mysql_real_connect() failed" << endl;  
		return -1;  
	}  

#ifdef STEPBYSTEP  
	system("pause");  
#endif  

	//sql字符串  
	string sqlstr;  

// 	//创建一个表  
// 	sqlstr = "CREATE TABLE IF NOT EXISTS user_info";  //表的名称
// 	sqlstr += "(";  
// 	sqlstr +=  
// 		"user_id INT UNSIGNED ,";  //第一列user_id
// 	sqlstr +=  
// 		"user_name VARCHAR(100),";  //第二列user_name
// 	sqlstr +=  
// 		"user_second_sum INT UNSIGNED ";  //第三列user_second_sum
// 	sqlstr += ");";  
// 	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
// 		cout << "mysql_query() create table succeed" << endl;  
// 	}  
// 	else {  
// 		cout << "mysql_query() create table failed" << endl;  
// 		mysql_close(&mydata);  
// 		return -1;  
// 	}  

	//要转换成什么样子才对
	//输入数据不变形
	//输出数据不变形

// 	char timeData[15]={49,49,49,49,49,49,49,49,49,49,49,49,49,49,0}; //要输出时间，要在char数组后加入0 表示结尾
// 	unsigned char mac[6]={176,226,53,43,218,224};
// 	char r1=-36,r2=-26,r3=-45,r4=-14;
// 	stringstream streamm;
// 	streamm<<int(r1);
// 
// 	cout<<macToString(mac)<<endl;
// 	cout<<streamm.str()<<endl;

#ifdef STEPBYSTEP  
	system("pause");  
#endif  

	//向表中插入数据  
// 	sqlstr =  
// 		"INSERT INTO tt VALUES ( 11111111111111 , 'ab:ab:ab:Ab:Aa:ab' ,-21,-24,-25,-26);";   
// 	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
// 		cout << "mysql_query() insert data succeed" << endl;  
// 	}  
// 	else {  
// 		cout << "mysql_query() insert data failed" << endl;  
// 		mysql_close(&mydata);  
// 		return -1;  
// 	}  

// #ifdef STEPBYSTEP  
// 	system("pause");  
// #endif  
// 
	vector<string> terms;
	//在控制台显示刚才插入的数据  
	sqlstr = "SELECT distinct macAddress FROM ttnew";  
	MYSQL_RES *result = NULL;  
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		/*cout << "mysql_query() select data succeed" << endl;  */
		//一次性取得数据集  
		result = mysql_store_result(&mydata);  
		//取得并打印行数  
		int rowcount = mysql_num_rows(result);  
		cout << "row count: " << rowcount << endl;  

		//取得并打印各字段的名称  
		unsigned int fieldcount = mysql_num_fields(result);  
		cout<<fieldcount<<endl;

// 		MYSQL_FIELD *field = NULL;  
// 		for (unsigned int i = 0; i < fieldcount; i++) {  
// 			field = mysql_fetch_field_direct(result, i);  
// 			cout << field->name << "\t\t";  
// 		}  
// 		cout << endl;  

		//打印各行  

  		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			for (int i = 0; i < fieldcount; i++) {  

				terms.push_back(row[i]);
 				cout << row[i] << "\t\t";   
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

	cout<<"_____________________________________________"<<endl;

	for(auto i:terms)
	{
		cout<<"xx"<<i<<endl;
	}
// 
// #ifdef STEPBYSTEP  
// 	system("pause");  
// #endif  
// 
// 	//删除刚才建的表  
// 	sqlstr = "DROP TABLE user_info";  
// 	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
// 	    cout << "mysql_query() drop table succeed" << endl;  
// 	}  
// 	else {  
// 	    cout << "mysql_query() drop table failed" << endl;  
// 	    mysql_close(&mydata);  
// 	    return -1;  
// 	} 


	mysql_free_result(result);  
	mysql_close(&mydata);  
	mysql_server_end();  

	system("pause");  
	return 0;  
}