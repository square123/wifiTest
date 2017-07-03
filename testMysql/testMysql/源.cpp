#include <winsock.h>  
#include <iostream>  
#include <string>
#include <vector>
#include <sstream> //用于转换的char类型到string型
#include <mysql.h>  
#include "syscMySQL.h"
using namespace std;  

//#pragma comment(lib, "ws2_32.lib")  
//#pragma comment(lib, "libmysql.lib")  

//要进行测试的话，先离线测试下算法有没有错，然后测试线在线处理的情况 测试如果成功后，该任务就告一段落，后续再完成，下午再测试吧


int main() {  
	syscMySQL test;//测试包括每个函数的测试
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