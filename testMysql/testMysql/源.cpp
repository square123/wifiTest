#include <winsock.h>  
#include <iostream>  
#include <string>
#include <vector>
#include <sstream> //����ת����char���͵�string��
#include <mysql.h>  
using namespace std;  

//#pragma comment(lib, "ws2_32.lib")  
//#pragma comment(lib, "libmysql.lib")  

//����ִ�У����뵥��ִ�о�ע�͵�  
#define STEPBYSTEP  

string charTo02XStr(unsigned char input)//��char����ת����02X�ַ�����
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

string macToString(unsigned char Mymac[6])//��ɽ�char����ת�����ַ���
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

#ifdef STEPBYSTEP     //�������룬���ã�����ڴ�֮ǰ�Ѷ����������ĺ�������������Ρ�
	system("pause");  
#endif  

	//�ر���һ�����ݽṹ  
	MYSQL mydata;  

	//��ʼ�����ݿ�  
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

	//��ʼ�����ݽṹ  
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

	//���������ݿ�֮ǰ�����ö��������ѡ��  
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������  
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

	//�������ݿ�  
	if (NULL!= mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3306, NULL, 0))  
		//����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������  
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

	//sql�ַ���  
	string sqlstr;  

// 	//����һ����  
// 	sqlstr = "CREATE TABLE IF NOT EXISTS user_info";  //�������
// 	sqlstr += "(";  
// 	sqlstr +=  
// 		"user_id INT UNSIGNED ,";  //��һ��user_id
// 	sqlstr +=  
// 		"user_name VARCHAR(100),";  //�ڶ���user_name
// 	sqlstr +=  
// 		"user_second_sum INT UNSIGNED ";  //������user_second_sum
// 	sqlstr += ");";  
// 	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
// 		cout << "mysql_query() create table succeed" << endl;  
// 	}  
// 	else {  
// 		cout << "mysql_query() create table failed" << endl;  
// 		mysql_close(&mydata);  
// 		return -1;  
// 	}  

	//Ҫת����ʲô���ӲŶ�
	//�������ݲ�����
	//������ݲ�����

// 	char timeData[15]={49,49,49,49,49,49,49,49,49,49,49,49,49,49,0}; //Ҫ���ʱ�䣬Ҫ��char��������0 ��ʾ��β
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

	//����в�������  
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
	//�ڿ���̨��ʾ�ղŲ��������  
	sqlstr = "SELECT distinct macAddress FROM ttnew";  
	MYSQL_RES *result = NULL;  
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {  
		/*cout << "mysql_query() select data succeed" << endl;  */
		//һ����ȡ�����ݼ�  
		result = mysql_store_result(&mydata);  
		//ȡ�ò���ӡ����  
		int rowcount = mysql_num_rows(result);  
		cout << "row count: " << rowcount << endl;  

		//ȡ�ò���ӡ���ֶε�����  
		unsigned int fieldcount = mysql_num_fields(result);  
		cout<<fieldcount<<endl;

// 		MYSQL_FIELD *field = NULL;  
// 		for (unsigned int i = 0; i < fieldcount; i++) {  
// 			field = mysql_fetch_field_direct(result, i);  
// 			cout << field->name << "\t\t";  
// 		}  
// 		cout << endl;  

		//��ӡ����  

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
// 	//ɾ���ղŽ��ı�  
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