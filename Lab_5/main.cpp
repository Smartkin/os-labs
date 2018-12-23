#include <iostream>
#include <fstream>
#include <string>
using namespace std;




int main(int argc, char* argv[])
{
	fstream proc_info("/proc/self/smaps");
	fstream mem_info("/proc/self/status");
	filebuf* pbuf = mem_info.rdbuf();
	char* buf = new char[65535];
	
	pbuf->sgetn(buf, 65535);
	string str_vmsize(buf);
	auto index = str_vmsize.find("VmSize:");
	if (index != string::npos)
	{
		for(int i=0;i<20;++i)
		{
			cout << str_vmsize[index+i];
		}
		cout << endl;
	}
	cout << proc_info.rdbuf();	
	cout << endl;

	mem_info.close();
	proc_info.close();

	delete[] buf;
	return 0;
}
