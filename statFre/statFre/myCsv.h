#ifndef  MYCSV_H
#define  MYCSV_H
#include <vector>
#include <string>
#include <fstream>
class Csv  //加入这个是为了识别品牌
{
public:
	std::vector <std::vector <std::string>> table;    //存储表格结构的二维vector
	int RowsCount;
	int ColumnsCount;
	Csv(char* filename)
	{
		FILE *fp; 
		char StrLine[1024];             //每行最大读取的字符数
		if((fp = fopen(filename,"r")) == NULL) //判断文件是否存在及可读
		{ 
			std::cout<<"文件不存在！"<<std::endl;
		} 
		std::vector <std::vector<std::string>> map;
		while (!feof(fp)) 
		{ 
			std::vector <std::string> row;
			fgets(StrLine,1024,fp);  //读取一行
			//每一行根据逗号再进行分割
			const char * split = ","; 
			char * p; 
			p = strtok (StrLine,split); 
			while(p!=NULL) { 
				row.push_back(p);    //每行每个元素加入行row中
				p = strtok(NULL,split); 
			} 
			ColumnsCount = row.size();
			map.push_back(row);    //每一行row加入map中
		} 	
		RowsCount = map.size();
		table = map;
	};
};
#endif