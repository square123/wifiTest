//�����򼯳���ͳ�Ƴ���Ƶ�Σ�������̣����ɸѡӦ���޳�������
//��Ϊͬһ���ﲻ�����ڶ�γ��ϳ������ݣ��ҷ�Χ������ѡȡ�ܴ�
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>  
#include "myCsv.h"
#define pathDef "E://7��14����Ŀ���//projectTest//dingProcess//"  //�����ļ���·��
using namespace std;

class easyManu //���mac��ַ���̵ļ���
{
public:
	std::map<std::string,std::string> mobileManu;//Mac��ӳ���ϵ��
	easyManu()
	{
		Csv csv("������ֻ�Ʒ�ƶ�Ӧ��.csv");//��Ʒ�ƶ�Ӧ���Ǻ�ȫ����ʱ��ɾ���Ĳ��֣��´ο���ֻ����Ҫ�ֻ���Mac��ַ��¼�������ɡ�
		for (auto i=csv.table.begin();i!=csv.table.end();i++)
		{
			auto i1=i->begin();
			auto i2=i->begin()+1;
			mobileManu[*i1]=*i2;
		}
	}

	void mobileManuOutput(const string &srcStr)//����ֻ������Ķ�Ӧ����
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
			std::cout<<"����Ʒ��"<<endl;
		}
	}
};

int main()
{
	ofstream outflie("DingTichu.txt",ios::app); //����ļ���
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
		while(getline(infile,s))//�ȶ�ȡһ�е�����
		{
			istringstream xxx(s);//������ת��
			string a,b,c;
			xxx>>a>>b>>c;
			//cout<<a<<endl;//ֻ��ȡǰ������ݣ���ΪҪ���ǵ��ǲ�ͬʱ��ε����࣬������������
			statData[a]++;
		}
		infile.close();             //�ر��ļ������� 
	}
	vector<string> tichu;
	for (auto const &x:statData)
	{
		if (x.second>8)//�޳�Ԫ��֮ǰ���ݽϺõ�Ԫ�� ��֮ǰ��Ԫ�ر�������һ���ļ��У� Ӧ����һ�����������ʱȥ���£����洢�����ݿ���������������޳����ݡ�
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
	//�½�һ���ļ���������ִ����ܶ��Mac��ַ���Ժ���Խ���Щ�����޳������Ժ�������ʱҲ��Ҫ��Mac��ַ�޳� //Ӧ����Ҫ�������ֻ���

	//����Ӧ�ð����ݽ����˲����޳�һЩ����
	outflie.close();
	system("pause");
	return 0;
}