#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
#include<time.h>//加入时间
#include <fstream>
#include "WifiProbe.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

int main(int argc,char*argv[])
{
	Probe probe;
	probe.InitProbe();
	while(1)
	{
		probe.probeProcess();
		if( GetKeyState( VK_ESCAPE ) < 0 )
		{
			probe.~Probe();
		}
	}  
}
