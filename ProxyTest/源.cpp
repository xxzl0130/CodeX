#include <iostream>
#include <string>
#include "SharpBridge/SharpBridge.h"
using namespace std;

int main()
{
	if (SharpBridge::Proxy::Instance().Start())
	{
		cout << "success!" << endl;
	}
	else
	{
		cout << "failed!" << endl;
	}
}