#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
#include<time.h>//����ʱ��
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
