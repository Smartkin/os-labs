#include"Utils.h"
#include<sstream>

std::string GetColorString(const DWORD& color)
{
	std::stringstream ss;
	ss << (color & 255) << ":" << (color >> 8 & 255) << ":" << (color >> 16 & 255);

	return ss.str();
}

std::string GetTimeString(const SYSTEMTIME& time)
{
	std::stringstream ss;
	ss << "Date: " << time.wDay << "\\" << time.wMonth << "\\" << time.wYear << " Time: " << time.wHour << ":" << time.wMinute << ":"
		<< time.wSecond << "." << time.wMilliseconds;

	return ss.str();
}