#include <vector>
#include <string>
#include <fstream>
using std::string;
using std::vector;
class Csv
{
public:
	vector <vector <string>> table;    //�洢���ṹ�Ķ�άvector
	int RowsCount;
	int ColumnsCount;
	Csv(char* filename)
	{
		FILE *fp; 
		char StrLine[1024];             //ÿ������ȡ���ַ���
		if((fp = fopen(filename,"r")) == NULL) //�ж��ļ��Ƿ���ڼ��ɶ�
		{ 

		} 
		vector <vector<string>> map;
		while (!feof(fp)) 
		{ 
			vector <string> row;
			fgets(StrLine,1024,fp);  //��ȡһ��
			//ÿһ�и��ݶ����ٽ��зָ�
			const char * split = ","; 
			char * p; 
			p = strtok (StrLine,split); 
			while(p!=NULL) { 
				row.push_back(p);    //ÿ��ÿ��Ԫ�ؼ�����row��
				p = strtok(NULL,split); 
			} 
			ColumnsCount = row.size();
			map.push_back(row);    //ÿһ��row����map��
		} 	
		RowsCount = map.size();
		table = map;

	};

};