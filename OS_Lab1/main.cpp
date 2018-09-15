#pragma warning(disable : 4996)

#include<iostream>
#include<locale>
#include"Utils.h"

void OutputMetrics();
void OutputParameters();
void OutputColors();
void OutputTime();


int main(int argc, const char** argv)
{

	DWORD OS_Name_Buf = MAX_COMPUTERNAME_LENGTH + 1;
	DWORD User_Name_Buf = MAX_COMPUTERNAME_LENGTH + 1;
	DWORD Dir_Buf = 256;

	TCHAR* OS_Name = new TCHAR[OS_Name_Buf];
	TCHAR* User_Name = new TCHAR[User_Name_Buf];
	TCHAR* Sys_Dir = new TCHAR[Dir_Buf];
	TCHAR* Win_Dir = new TCHAR[Dir_Buf];
	TCHAR* Temp_Dir = new TCHAR[Dir_Buf];

	GetComputerName(OS_Name, &OS_Name_Buf);
	GetUserName(User_Name, &User_Name_Buf);
	GetSystemDirectory(Sys_Dir, Dir_Buf);
	GetWindowsDirectory(Win_Dir, Dir_Buf);
	GetTempPath(Dir_Buf, Temp_Dir);

	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&info);

	std::cout << "Hello computer, " << OS_Name << std::endl;
	std::cout << "Welcome user, " << User_Name << std::endl << std::endl;
	std::cout << "Here is some system information!\n\n\n";
	std::cout << "System directory: " << Sys_Dir << std::endl;
	std::cout << "Windows directory: " << Win_Dir << std::endl;
	std::cout << "Temporary directory: " << Temp_Dir << std::endl;
	std::cout << "Windows version: " << info.dwMajorVersion << "." << info.dwMinorVersion << "." << info.dwBuildNumber <<
		" PlatformID: " << info.dwPlatformId << std::endl;
	std::cout << std::endl;

	std::cout << "\tSome system metrics" << std::endl;
	OutputMetrics();
	std::cout << std::endl;

	std::cout << "\tSome system features and parameters" << std::endl;
	OutputParameters();
	std::cout << std::endl;

	std::cout << "\tSome color information" << std::endl;
	OutputColors();
	std::cout << std::endl;

	std::cout << "\tSome time information" << std::endl;
	OutputTime();
	std::cout << std::endl;
	

	//Clean up the buffers
	delete[] OS_Name;
	delete[] User_Name;
	delete[] Sys_Dir;
	delete[] Win_Dir;
	delete[] Temp_Dir;

	system("pause");
	return 0;
}


void OutputParameters()
{
	ANIMATIONINFO anim_info;
	BOOL font_smooth;
	anim_info.cbSize = sizeof(ANIMATIONINFO);
	
	SystemParametersInfo(SPI_GETANIMATION, sizeof(ANIMATIONINFO), &anim_info, 0);
	SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &font_smooth, 0);

	if (anim_info.iMinAnimate != 0)
	{
		std::cout << "Minimize and restore animation is enabled!" << std::endl;
	}
	else
	{
		std::cout << "Minimize and restore animation is disabled!" << std::endl;
	}

	if (font_smooth)
	{
		std::cout << "Font smoothing is enabled!" << std::endl;
	}
	else
	{
		std::cout << "Font smoothing is disabled!" << std::endl;
	}
}

void OutputMetrics()
{
	std::string boot_type = "";

	switch (GetSystemMetrics(SM_CLEANBOOT))
	{
	case 0:
		boot_type = "Normal boot";
		break;
	case 1:
		boot_type = "Fail-safe boot";
		break;
	case 2:
		boot_type = "Fail-safe with network boot";
		break;
	default:
		boot_type = "Unknown boot type";
		break;
	}

	std::cout << "Boot type: " << boot_type << std::endl;

	std::cout << "Monitor count: " << GetSystemMetrics(SM_CMONITORS) << std::endl;
	std::cout << "Mouse buttons: " << GetSystemMetrics(SM_CMOUSEBUTTONS) << std::endl;
	std::cout << "Border Width and Height: " << GetSystemMetrics(SM_CXBORDER) << "x" << GetSystemMetrics(SM_CYBORDER) << std::endl;
	std::cout << "Screen Width and Height: " << GetSystemMetrics(SM_CXSCREEN) << "x" << GetSystemMetrics(SM_CYSCREEN) << std::endl;
}

void OutputColors()
{
	DWORD act_caption_col = GetSysColor(COLOR_ACTIVEBORDER);
	std::cout << "Activate border color RGB: " << GetColorString(act_caption_col) << std::endl;

	DWORD bg_col = GetSysColor(COLOR_BACKGROUND);
	std::cout << "Background color RGB: " << GetColorString(bg_col) << std::endl;

	DWORD btn_col = GetSysColor(COLOR_BTNHIGHLIGHT);
	std::cout << "Button highlight color RGB: " << GetColorString(btn_col) << std::endl;

	DWORD frame_col = GetSysColor(COLOR_WINDOWFRAME);
	std::cout << "Window frame color RGB: " << GetColorString(frame_col) << std::endl;

	const int elem_amt = 4;
	INT col_elements[elem_amt] = { COLOR_ACTIVEBORDER, COLOR_BACKGROUND, COLOR_BTNHIGHLIGHT, COLOR_WINDOWFRAME };
	COLORREF colors[elem_amt] = { RGB(100,0,125), RGB(10,255,255), RGB(0,0,0), RGB(100,20,100) };
	SetSysColors(elem_amt, col_elements, colors);

	std::cout << "Changed colors!" << std::endl;

	act_caption_col = GetSysColor(COLOR_ACTIVEBORDER);
	std::cout << "Activate border color RGB: " << (act_caption_col & 255) << ":" << (act_caption_col >> 8 & 255) << ":" << (act_caption_col >> 16 & 255) << std::endl;

	bg_col = GetSysColor(COLOR_BACKGROUND);
	std::cout << "Background color RGB: " << GetColorString(bg_col) << std::endl;

	btn_col = GetSysColor(COLOR_BTNHIGHLIGHT);
	std::cout << "Button highlight color RGB: " << GetColorString(btn_col) << std::endl;

	frame_col = GetSysColor(COLOR_WINDOWFRAME);
	std::cout << "Window frame color RGB: " << GetColorString(frame_col) << std::endl;
}

void OutputTime()
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	std::cout << "Local time" << std::endl << GetTimeString(sys_time) << std::endl;

	GetSystemTime(&sys_time);
	std::cout << "System time" << std::endl << GetTimeString(sys_time) << std::endl;

	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	std::cout << "Time zone information" << std::endl;
	std::wcout << tzi.DaylightName;
	std::cout << " " << GetTimeString(tzi.DaylightDate) << std::endl;
}