#include "syscMySQL.h"

//存数据的时候应该单独写个函数，不用很复杂

syscMySQL::syscMySQL() //默认的数据库test
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
		"rssi4  int(11)  null";  //rssi4
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
		"name  varchar(20)  null,";  //index不能做关键词
	sqlstr +=  
		"dis1  int(11)  null,";  //dis1
	sqlstr +=  
		"dis2  int(11)  null,";  //dis2
	sqlstr +=  
		"dis3  int(11)  null,";  //dis3
	sqlstr +=  
		"dis4  int(11)  null";  //dis4
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
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {  
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
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {   
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
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {   
		cout << "mysql_query() insert data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

void syscMySQL::readData(const char *tableName,vector<vector<string>>  &tempOut ) //读取数据使用，一般用的很少
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
		output=output+":"+charTo02XStr(Mymac[i]);
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

int syscMySQL::stringToInt(string x)
{
	int temp;
	stringstream stream;
	stream<<x;
	stream>>temp;
// 	if (!stream.good()) 
// 	{ 
// 		cout<<"error transform strToInt";
// 	}
	return temp;
}

void syscMySQL::selTimeBetween(const char *srcTable,const char *dstTable,const char *timeBegin,const char *timeEnd)//尽量减少mysql表的操作，减少在程序里的操作
{
	string sqlstr="create table ";
	sqlstr+=dstTable;
	sqlstr+=" as select * from ";
	sqlstr+=srcTable;
	sqlstr+=" where time between '";
	sqlstr+=timeBegin;
	sqlstr+="' and '";
	sqlstr+=timeEnd;
	sqlstr+="';";
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() selTimeBetween failed" << endl;  
		mysql_close(&mydata);  
	}  
}

void syscMySQL::uniqueMac(const char *srcTable,const char *name,vector<string> &uniqueTerm) //写成通用的
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
		cout << "mysql_query() unique failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::rssiDataGet(const char *srcTable,const string name,vector<syscMySQL::rssiData> &dst)//读取rssi数据的函数
{
	string sqlstr="SELECT * FROM ";
	sqlstr+=srcTable;
	sqlstr+=" WHERE mac='";
	sqlstr+=name;  
	sqlstr+="' order by time asc;";
	MYSQL_RES *result = NULL; 
	if (0 == mysql_query(&mydata, sqlstr.c_str())) 
	{
		result = mysql_store_result(&mydata);  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			rssiData ttt;
			ttt.time=row[0];
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
		cout << "mysql_query() rssiDataGet failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::camDataGet(const char *srcTable,const char* name,vector<syscMySQL::camData> &dst)//读取cam数据的函数
{
	string sqlstr="SELECT * FROM ";
	sqlstr+=srcTable;
	sqlstr+=" WHERE name='";
	sqlstr+=name;  
	sqlstr+="' order by time asc;";
	MYSQL_RES *result = NULL; 
	if (0 == mysql_query(&mydata, sqlstr.c_str())) 
	{
		result = mysql_store_result(&mydata);  
		MYSQL_ROW row = NULL;  
		row = mysql_fetch_row(result);  
		while (NULL != row) {  
			camData ttt;
			ttt.time=row[0];
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
		cout << "mysql_query() camDataGet failed" << endl;  
		mysql_free_result(result);  
		mysql_close(&mydata); 
	}  
}

void syscMySQL::matchProcess(const char *srcCamTable,const char *srcRssiTable,const char *camName,const char *timeBegin,const char *timeEnd,vector<matchResult> &res)
{
	vector<camData> cam;
	//构建数据表
	selTimeBetween(srcCamTable,"tempCam",timeBegin,timeEnd);
	selTimeBetween(srcRssiTable,"tempRssi",timeBegin,timeEnd);
	//从摄像头表中获取camName的信息
	camDataGet("tempCam",camName,cam);
	//获取rssi中存在的mac地址
	vector<string> rssiName;
	uniqueMac("tempRssi","mac",rssiName);
	for (auto i:rssiName)
	{
		vector<rssiData> rssi;
		matchResult resSingle;
		//cout<<i<<endl;
		rssiDataGet("tempRssi",i,rssi);
		//cout<<rssi.size()<<endl;
		singleMatch(cam,rssi,resSingle);
		res.push_back(resSingle);
	}
	//对匹配的结果分数进行更新
	renewResult(res); //需要进行测试
	//排序
	sort(res.begin(),res.end(),[](const matchResult &r1,const matchResult &r2){return r2.score<r1.score;});
	dropTable("tempCam");
	dropTable("tempRssi");
}


// bool syscMySQL::resultSort(const matchResult &r1,const matchResult &r2)
// {
// 	return r1.score<r2.score;
// }

//因为在记录过程中已经将信息优化为一秒，所以一秒对应多个的问题
void syscMySQL::singleMatch(vector<camData> &src1,vector<rssiData> &src2,matchResult &result)//匹配过程的关键，一定要取两个数据的交集
{
	//cout<<"camsize"<<src1.size()<<endl; //这个部分要好好修改下
	//cout<<"rssisize"<<src2.size()<<endl; 
	//默认在存储时，元素已经是拍好序的   所以其匹配速度为O(n)
	//默认camData是一直有值的，不存在某时刻有rssi值没有cam值，则交集就是src2本身
	//如果实际中，camData没有数据，则数据可能需要先取交集。
	vector<camData> tempCam;//camData 需要有一个存储的过程，所以不能改变其变量
	vector<rssiData> tempRssi;
	auto it1=src1.begin();
	auto it2=src2.begin();
	while (it2!=src2.end()&&it1!=src1.end())//这么写是为了以防万一，万一默认数据这里不对也需要修改 //为了稳定性，将RSSI也定义为中间变量
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
			tempRssi.push_back(*it2); //保证两个变量大小是一样的
			it1++;
			it2++;
		}
	}
	//cout<<tempCam.size()<<endl;
	//觉得用动态数组不够好，而且存在很多的冗余，直接采用vector进行计算
	vector<int> rssi1,rssi2,rssi3,rssi0; 
	vector<int> cam1,cam2,cam3,cam0;
	for (int ind=0;ind<tempCam.size();++ind) //通过该操作，此时已经一一对应，无需再考虑时间的问题
	{
		//按照rssi对应存在的位置将cam也存进来
		if (src2[ind].rssi[0]!=0)
		{
			rssi0.push_back(tempRssi[ind].rssi[0]);
			cam0.push_back(tempCam[ind].dis[0]);
		}
		if (src2[ind].rssi[1]!=0)
		{
			rssi1.push_back(tempRssi[ind].rssi[1]);
			cam1.push_back(tempCam[ind].dis[1]);
		}
		if (src2[ind].rssi[2]!=0)
		{
			rssi2.push_back(tempRssi[ind].rssi[2]);
			cam2.push_back(tempCam[ind].dis[2]);
		}
		if (src2[ind].rssi[3]!=0)
		{
			rssi3.push_back(tempRssi[ind].rssi[3]);
			cam3.push_back(tempCam[ind].dis[3]);
		}	
	}
	//上述的的四个特征已经被筛选出来了，计算表格中数据的特征值,找出非零的特征 此时的输出值已经是0 1 0.3
	vector<int> diff0=minusFeatureTran(rssiFeatureTran(rssi0,3),camFeatureTran(cam0,50));
	vector<int> diff1=minusFeatureTran(rssiFeatureTran(rssi1,3),camFeatureTran(cam1,50));
	vector<int> diff2=minusFeatureTran(rssiFeatureTran(rssi2,3),camFeatureTran(cam2,50));
	vector<int> diff3=minusFeatureTran(rssiFeatureTran(rssi3,3),camFeatureTran(cam3,50));
	//最后输出分数和特征个数
	result.macName=src2[0].macName;
	result.num=diff0.size()+diff1.size()+diff2.size()+diff3.size();
	result.score=result.num==0?0:double(accumulate(diff0.begin(),diff0.end(),accumulate(diff2.begin(),diff2.end(),0))+accumulate(diff1.begin(),diff1.end(),accumulate(diff3.begin(),diff3.end(),0)))/result.num*100;
}

vector<int> syscMySQL::rssiFeatureTran(vector<int> &src,int thd)
{
	vector<int> result;//应该加入判断
	if (src.size()!=0)
	{
		for (auto it=src.begin();it!=src.end()-1;it++)
		{
			if ((*(it+1)-(*it))>thd)
			{
				result.push_back(1);//rssi值变大记为1
			}
			else if (((*it)-(*it+1))>thd)
			{
				result.push_back(-1);//rssi值变小记为-1
			}
			else
			{
				result.push_back(0);//不变记为0
			}
		}
	}
	return result;
}

vector<int> syscMySQL::camFeatureTran(vector<int> &src,int thd)
{
	vector<int> result;
	if (src.size()!=0)
	{
		for (auto it=src.begin();it!=src.end()-1;it++)
		{
			if ((*(it+1)-(*it))>thd)
			{
				result.push_back(-1);//距离变大记为-1
			}
			else if (((*it)-(*it+1))>thd)
			{
				result.push_back(1);//距离变小记为1
			}
			else
			{
				result.push_back(0);//不变记为0
			}
		}
	}
	return result;
}

vector<int> syscMySQL::minusFeatureTran(vector<int> &src1,vector<int> &src2)
{
	vector<int> result;
	if (src1.size()==src2.size())//尺寸必须要一样
	{
		for (auto it1=src1.begin(),it2=src2.begin();it1!=src1.end();it1++,it2++)
		{
			switch (abs(*it1-*it2))
			{
			case 0://距离相同则记为1
				result.push_back(1);
				break;
			case 1://距离相似则记为0.3
				result.push_back(0.3);
				break;
			case 2://距离不同则记为0
				result.push_back(0);
				break;
			default:
				break;
			}
		}
	}
	return result;
}

void syscMySQL::renewResult(vector<matchResult> &src)//重新根据数量再对分数进行修改
{
	//返回最大数的位置
	auto maxEle=max_element(src.begin(),src.end(),[](const matchResult &r1,const matchResult &r2){return r1.num<r2.num;});
	//sigmoid函数
	double b=0.5*((*maxEle).num);
	double a=15.3170/((*maxEle).num);
	for_each(src.begin(),src.end(),[a,b](matchResult &r1){r1.score=r1.score/(1+exp(-1*a*(r1.num-b)));});
}