//本程序集成了统计出现频次，输出厂商，输出筛选应该剔除的数据
//因为同一人物不可能在多次场合出现数据，且范围不可能选取很大
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>  
#include "myCsv.h"
#define pathDef "E://7月14日项目相关//projectTest//dingProcess//"  //输入文件的路径
using namespace std;

class easyManu //输出mac地址厂商的简单类
{
public:
	std::map<std::string,std::string> mobileManu;//Mac码映射关系表
	easyManu()
	{
		Csv csv("精简版手机品牌对应表.csv");//该品牌对应表不是很全，当时有删掉的部分，下次可以只把主要手机的Mac地址记录下来即可。
		for (auto i=csv.table.begin();i!=csv.table.end();i++)
		{
			auto i1=i->begin();
			auto i2=i->begin()+1;
			mobileManu[*i1]=*i2;
		}
	}

	void mobileManuOutput(const string &srcStr)//输出手机网卡的对应厂商
	{
		std::string mac;
		mac=srcStr.substr(0,2)+srcStr.substr(3,2)+srcStr.substr(6,2);
		auto itt = mobileManu.find(mac);
		if (itt != mobileManu.end())
		{
			std::cout<<itt->second;
		}
		else
		{
			std::cout<<"其他品牌"<<endl;
		}
	}
};

int main()
{
	ofstream outflie("DingTichu.txt",ios::app); //输出文件名
	easyManu ManuOutput;
	map<string,int> statData;
	char str[100];
	for (int i=1;i<52;i++)
	{
		string pathStr=pathDef;
		sprintf_s(str,sizeof(str),"%d",i); 
		pathStr=pathStr+str;
		pathStr=pathStr+".txt";
		ifstream infile; 
		infile.open(pathStr);   
		string s;
		while(getline(infile,s))//先读取一行的数据
		{
			istringstream xxx(s);//将数据转换
			string a,b,c;
			xxx>>a>>b>>c;
			//cout<<a<<endl;//只读取前面的数据，因为要考虑的是不同时间段的种类，而不是其他的
			statData[a]++;
		}
		infile.close();             //关闭文件输入流 
	}
	vector<string> tichu;
	for (auto const &x:statData)
	{
		if (x.second>8)//剔除元素之前数据较好的元素 将之前的元素保存在于一个文件中， 应该有一个程序可以随时去更新，将存储的数据库进行修正。便于剔除数据。
		{
			tichu.push_back(x.first);
		}
		//cout<<x.first<<" "<<x.second<<endl;
		 //ManuOutput.mobileManuOutput(x.first);
	}
	for (auto & i:tichu)
	{
		outflie<<i<<endl;
	}
	//新建一个文件，保存出现次数很多的Mac地址，以后可以将这些数据剔除，在以后处理数据时也需要将Mac地址剔除 //应该需要建立这种机制

	//后面应该把数据进行滤波。剔除一些数据
	outflie.close();
	system("pause");
	return 0;
}