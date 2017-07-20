//以后数据要存到数据库中
//程序是将形如1.txt格式的文件合并成一个新的文件
#include <iostream> 
#include <set>
#include <fstream>
#include <string>
#include <sstream>  
#include <string>

#define pathDef "E://7月14日项目相关//projectTest//zong//"  //输入文件的路径
#define num 2 //输入文件的个数

using namespace std;

int main()
{
	set<string> uniqueSet;
	char str[10]; //输入文件的最大位数
	for (int i=1;i<num+1;i++)
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
			//cout<<a<<endl;//只读取前面的数据，因为要考虑的是不同时间段的种类，而不是其他的
			uniqueSet.insert(s);
		}
		infile.close();             //关闭文件输入流 
	}
	ofstream outfile("zong.txt",ios::app);
	for (auto &i:uniqueSet)
	{
		outfile<<i<<endl;
	}
	outfile.close();
	system("pause");
	return 0;
}