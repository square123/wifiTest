#include <winsock.h>  
#include <iostream>  
#include <string>
#include <vector>
#include <sstream> //����ת����char���͵�string��
#include <mysql.h>  
#include "syscMySQL.h"
using namespace std;  

//#pragma comment(lib, "ws2_32.lib")  
//#pragma comment(lib, "libmysql.lib")  

//Ҫ���в��ԵĻ��������߲������㷨��û�д�Ȼ����������ߴ������� ��������ɹ��󣬸�����͸�һ���䣬��������ɣ������ٲ��԰�


int main() {  
	syscMySQL test;//���԰���ÿ�������Ĳ���
	//test.camTabCre("camtest");
	//test.dropTable("camtest");
	//vector<vector<string>> xx;

	//test.camTabCre("xx");
	vector<syscMySQL::camData> xxx;
 	test.camDataGet("xx","0",xxx);

	//cout<<xx[5][1]<<endl;
// 	char Timestamp[14]={50,50,50,50,50,50,50,50,50,50,50,50,50,50};
// 	unsigned char Mumac[6]={176,226,53,43,218,224};
// 	char Rssi[4]={-20,-25,-26,-30}; 
// 	test.insertRssiData("rssitest",Timestamp,Mumac,Rssi);
	//test.insertCamData("camtest","11111111111111",1,1.0,2.0,3.0,4.0);



	system("pause");  
	return 0;  
}