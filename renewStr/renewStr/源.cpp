//�Ժ�����Ҫ�浽���ݿ���
//�����ǽ�����1.txt��ʽ���ļ��ϲ���һ���µ��ļ�
#include <iostream> 
#include <set>
#include <fstream>
#include <string>
#include <sstream>  
#include <string>

#define pathDef "E://7��14����Ŀ���//projectTest//zong//"  //�����ļ���·��
#define num 2 //�����ļ��ĸ���

using namespace std;

int main()
{
	set<string> uniqueSet;
	char str[10]; //�����ļ������λ��
	for (int i=1;i<num+1;i++)
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
			//cout<<a<<endl;//ֻ��ȡǰ������ݣ���ΪҪ���ǵ��ǲ�ͬʱ��ε����࣬������������
			uniqueSet.insert(s);
		}
		infile.close();             //�ر��ļ������� 
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