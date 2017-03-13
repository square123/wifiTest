#include<stdio.h>
#include<string>
#include<iostream>
#include "MyProbe.h"

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