// Lab_2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

void OutputModuleInformation(char* modPath);
void OutputModuleInformation(long handle);

void OutputCurrentProcessInformation();

void OutputProcessList(HANDLE snapshot);
void OutputThreadList(HANDLE snapshot);
void OutputModuleList(HANDLE snapshot);
void OutputDriverList();

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: Program requires at least 1 parameter to start!" << std::endl;
		exit(1);
	}
	else
	{
		//Parse given param to see if handler number or module name was passed
		auto handle = strtol(argv[1], nullptr, 16);
		std::string param_str(argv[1]);

		if (handle == 0L || param_str.find_first_of('.') != std::string::npos || param_str.find_first_of(':') != std::string::npos)
		{
			OutputModuleInformation(argv[1]);
		}
		else
		{
			OutputModuleInformation(handle);
		}

		OutputCurrentProcessInformation();

		HANDLE th_snap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, GetCurrentProcessId());
		OutputProcessList(th_snap);
		OutputThreadList(th_snap);
		OutputModuleList(th_snap);
		OutputDriverList();
		CloseHandle(th_snap);
	}

	system("pause");

	return 0;
}

void OutputModuleInformation(char* modPath)
{
	bool fname_flag = false;
	std::wstring mod_name_str(1, '#');
	mod_name_str.resize(strlen(modPath) + 1, '\0');
	mbstowcs(&mod_name_str[0], modPath, strlen(modPath));

	std::wcout << L"Passed argument: " << mod_name_str.c_str() << std::endl;

	auto find_slash = mod_name_str.find_first_of(L'\\');
	fname_flag = find_slash == std::wstring::npos;

	HMODULE hModule = GetModuleHandleA(modPath);
	if (hModule == 0)
	{
		std::cout << "No module found!" << std::endl;
		return;
	}

	TCHAR* f_name_path_buf = new TCHAR[512]{ '\0' };
	TCHAR* f_name_buf = new TCHAR[512]{ '\0' };
	GetModuleFileName(hModule, f_name_path_buf, 512);
	std::wstring str = std::wstring(f_name_path_buf);

	str = str.substr(str.find_last_of(L'\\') + 1, str.length() - str.find_last_of(L'\\'));
	str.copy(f_name_buf, str.length());
	f_name_buf[str.length()] = '\0';
	std::wcout << L"Handle: " << hModule << std::endl;
	if (fname_flag) std::wcout << L"FName: " << f_name_path_buf << std::endl;
	else			std::wcout << L"Name: " << f_name_buf << std::endl;
}

void OutputModuleInformation(long handle)
{
	TCHAR* f_name_path_buf = new TCHAR[512]{ '\0' };
	TCHAR* f_name_buf = new TCHAR[512]{ '\0' };
	HMODULE hModule = (HMODULE)handle;

	std::cout << "Passed argument: " << std::hex << handle << std::dec << std::endl;

	int res = GetModuleFileName(hModule, f_name_path_buf, 512);
	if (res == 0)
	{
		std::cout << "No module found!" << std::endl;
		return;
	}
	std::wstring str = std::wstring(f_name_path_buf);

	str = str.substr(str.find_last_of(L'\\') + 1, str.length() - str.find_last_of(L'\\'));
	str.copy(f_name_buf, str.length());
	f_name_buf[str.length()] = '\0';

	std::wcout << L"FName: " << f_name_path_buf << std::endl;
	std::wcout << L"Name: " << f_name_buf << std::endl;
}

void OutputCurrentProcessInformation()
{
	DWORD proc_id = GetCurrentProcessId();
	std::cout << "Process ID: " << std::hex << proc_id << std::endl;

	HANDLE proc_pseudo_handle = GetCurrentProcess();
	std::cout << "Process pseudohandler: " << std::dec << proc_pseudo_handle << std::endl;

	HANDLE proc_handle;
	DuplicateHandle(proc_pseudo_handle, proc_pseudo_handle, proc_pseudo_handle, &proc_handle, PROCESS_ALL_ACCESS, TRUE, DUPLICATE_SAME_ACCESS);
	std::cout << "Process true handler: " << std::hex << proc_handle << std::endl;

	HANDLE copy_handle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, proc_id);
	std::cout << "Process copy handler: " << std::hex << copy_handle << std::endl;

	CloseHandle(copy_handle);
	CloseHandle(proc_handle);
}

void PrintProcessInfo(PROCESSENTRY32 pe);
void OutputProcessList(HANDLE snapshot)
{
	
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	Process32First(snapshot, &pe);
	PrintProcessInfo(pe);
	while (Process32Next(snapshot, &pe)) PrintProcessInfo(pe);

}

void PrintProcessInfo(PROCESSENTRY32 pe)
{
	std::cout << "NEW PROCESS OUTPUT" << std::endl;
	std::cout << "Process ID: " << pe.th32ProcessID << std::endl;
	std::cout << "Module ID: " << pe.th32ModuleID << std::endl;
	std::cout << "Parent process ID: " << pe.th32ParentProcessID << std::endl;
	std::cout << "Default heap ID: " << pe.th32DefaultHeapID << std::endl;
	std::cout << std::dec;
	std::cout << "Threads amount: " << pe.cntThreads << std::endl;
	std::cout << "Usage count: " << pe.cntUsage << std::endl;
	std::cout << "Base priority class: " << pe.pcPriClassBase << std::endl;
	std::cout << std::hex;
	std::wcout << L"Exe file name: " << pe.szExeFile << std::endl;
	std::cout << "FINISHED OUTPUTTING PROCESS INFO" << std::endl;
	std::cout << std::endl;
}

void PrintThreadInfo(THREADENTRY32 te);
void OutputThreadList(HANDLE snapshot)
{
	THREADENTRY32 te;
	auto counter = 0;
	const unsigned int MAX_OUT = 100; //Limit amout of thread output to 100
	te.dwSize = sizeof(te);

	Thread32First(snapshot, &te);
	PrintThreadInfo(te);
	while (Thread32Next(snapshot, &te) && counter < MAX_OUT) { PrintThreadInfo(te); counter++; };
}

void PrintThreadInfo(THREADENTRY32 te)
{
	std::cout << "NEW THREAD OUTPUT" << std::endl;
	std::cout << std::hex;
	std::cout << "Thread ID: " << te.th32ThreadID << std::endl;
	std::cout << "Owner process ID: " << te.th32OwnerProcessID << std::endl;
	std::cout << std::dec;
	std::cout << "Base priority: " << te.tpBasePri << std::endl;
	std::cout << "Delta priority: " << te.tpDeltaPri << std::endl;
	std::cout << "Usage count: " << te.cntUsage << std::endl;
	std::cout << "FINISHED OUTPUTTING THREAD INFO" << std::endl;
	std::cout << std::endl;
}

void PrintModuleInfo(MODULEENTRY32 me);
void OutputModuleList(HANDLE snapshot)
{
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	Module32First(snapshot, &me);
	PrintModuleInfo(me);
	while (Module32Next(snapshot, &me)) PrintModuleInfo(me);
}

void PrintModuleInfo(MODULEENTRY32 me)
{
	std::cout << "NEW MODULE OUPUT" << std::endl;
	std::cout << std::hex;
	std::cout << "Module ID: " << me.th32ModuleID << std::endl;
	std::cout << "Process ID: " << me.th32ProcessID << std::endl;
	std::cout << "Module base address: " << &me.modBaseAddr << std::endl;
	std::cout << std::dec;
	std::cout << "Module base size: " << me.modBaseSize << std::endl;
	std::wcout << L"Module handle: " << me.hModule << std::endl;
	std::wcout << L"Module path: " << me.szExePath << std::endl;
	std::cout << "FINISHED OUTPUTTING MODULE INFO" << std::endl;
	std::cout << std::endl;
}

void OutputDriverList()
{
	LPVOID drivers[1024];
	DWORD cbNeeded;
	int cDrivers, i;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
	{
		TCHAR szDriver[1024];

		cDrivers = cbNeeded / sizeof(drivers[0]);

		std::cout << "There are " << cDrivers << " drivers:" << std::endl;
		for (i = 0; i < cDrivers; i++)
		{
			if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
			{
				std::wcout << i + 1 << L": " << szDriver << std::endl;
			}
		}
	}
	else
	{
		std::cout << "EnumDeviceDrivers failed; array size needed is " << cbNeeded / sizeof(LPVOID) << std::endl;
		return;
	}
}