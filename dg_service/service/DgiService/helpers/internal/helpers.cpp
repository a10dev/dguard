

#define _WIN32_WINNT 0x0600

#include "helpers.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <TlHelp32.h>
#include <algorithm>
#include <Psapi.h>
#include <ostream>
#include <unordered_map>
#include <iphlpapi.h>
#include <Sddl.h>
#include <chrono>
#include <ctime>
#include <iomanip>

// #include "user manager/userManagerHelpers.h"

#pragma comment(lib, "psapi")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")

namespace process
{
	std::string getFilePath(DWORD _pid)
	{
		std::string path;
		char process_image[1024] = { 0 };

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ /*PROCESS_ALL_ACCESS*/, TRUE, _pid);
		if (hProcess != NULL)
		{
			size_t buffer_lenght = GetModuleFileNameExA(hProcess, NULL, process_image, sizeof(process_image));
			if (GetLastError() == ERROR_SUCCESS)
			{
				path = process_image;
			}
			else
			{
				ZeroMemory(process_image, sizeof(process_image));
				buffer_lenght = GetProcessImageFileNameA(hProcess, process_image, sizeof process_image);
				if (buffer_lenght != 0 /*GetLastError() == ERROR_SUCCESS*/)
				{
					path = process_image;
				}
			}
			CloseHandle(hProcess);
		}

		return path;
	}

//#ifndef UNICODE

	bool find(std::string _name,
		bool _fullMatch,
		std::string& _procImageFilePath,
		DWORD& _pid)
	{
		//DWORD pid = 0;
		bool result = false;
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);
			return false;
		}

		// use only same-case.
		strings::toLower(_name);
		std::wstring name = strings::s_ws(_name);

		do
		{
			std::wstring procname = pe32.szExeFile;
			strings::toLower(procname);

			if (_fullMatch)
			{
				if (strings::equalStrings(name, procname))
				{
					result = true;
				}
			}
			else if (procname.find(name) != std::string::npos)
			{
				result = true;
			}

			if (result)
			{
				_pid = pe32.th32ProcessID;
				_procImageFilePath = getFilePath(_pid);
				// 				if (_procName.empty())
				// 				{
				// 					_procName = procname;
				// 				}
				result = true;
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));
		CloseHandle(hProcessSnap);
		return result;
	}

//#endif
}

namespace timefn
{
	int getTimeoutValue(const SYSTEMTIME& _scheduledTime, unsigned int _waitPeriod)
	{
		SYSTEMTIME current;
		GetLocalTime(&current);

		int n = (_scheduledTime.wHour * 60 + _scheduledTime.wMinute) - (current.wHour * 60 + current.wMinute);

		// Time has passed, We have to calculate new time interval 
		if (n < 0)
		{
			// Time is come to do something..
			if (_waitPeriod == 0)
			{
				return 30;
			}
			if ((abs(n) % _waitPeriod) == 0)
			{
				return 0;
			}

			int timePast = abs(n) % _waitPeriod;
			int timeout = _waitPeriod - timePast;
			return timeout;
		}
		else if (n > 0)
		{
			// Time isn't come. Wait all the time.
			return n;
		}

		return 0;
	}
	
	std::string getCurrentTime()
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		return getTime(st);
	}

	std::string getTime(const SYSTEMTIME& _time)
	{
		char time[256] = { 0 };
		wsprintfA(time, "%d:%d:%d %d.%d.%d",
			_time.wHour, _time.wMinute, _time.wSecond,
			_time.wDay, _time.wMonth, _time.wYear);

		return time;
	}

	// get timestamp in milliseconds since unix epoch
	int64_t getTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
		auto epoch = now_ms.time_since_epoch();
		auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

		return value.count();														// timestamp with milliseconds
	}

}

namespace memory
{
	PVOID getmem(size_t x)
	{
		return (PVOID)((char*)new(std::nothrow) char[x]);
	}

	void freemem(PVOID x)
	{
		if (x)
		{
			char* p = (char*)x;
			delete[] p;
		}
	}
}

namespace winerror
{
	// DWORD errDw = GetLastError(); shold be called before
	std::string getLastErrorCodeMessage(DWORD _dwErrCode)
	{
		char* messageBuffer = nullptr;

		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			//FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			_dwErrCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR)&messageBuffer,
			0,
			NULL);

		char szErrorCode[32] = { 0 };
		//_itoa(_dwErrCode, szErrorCode, 10);
		wsprintfA(szErrorCode, "%d", _dwErrCode);
		std::string resMess(messageBuffer);
		LocalFree(messageBuffer); 

		if (size = 0)
		{
			resMess = "No description for the error code.";
		}
		return "(" + std::string(szErrorCode) + "): " + resMess;
	}
}

namespace strings
{
	std::string ws_s(const std::wstring& s, UINT cp)
	{
		int slength = (int)s.length(); /* + 1*/;
		int len = WideCharToMultiByte(cp, 0, s.c_str(), slength, 0, 0, 0, 0);
		std::string r(len, '\0');
		WideCharToMultiByte(cp, 0, s.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}

	std::wstring s_ws(const std::string& s, UINT cp)
	{
		int slength = (int)s.length(); /* + 1*/;
		int len = MultiByteToWideChar(cp, 0, s.c_str(), slength, 0, 0);
		std::wstring r(len, L'\0');
		MultiByteToWideChar(cp, 0, s.c_str(), slength, &r[0], len);
		return r;
	}

	void toUpper(std::string& _s)
	{
		std::transform(_s.begin(), _s.end(), _s.begin(), toupper);
	}

	void toLower(std::string& _s)
	{
		std::transform(_s.begin(), _s.end(), _s.begin(), tolower);
	}

	void toUpper(std::wstring& _s)
	{
		std::transform(_s.begin(), _s.end(), _s.begin(), toupper);
	}

	void toLower(std::wstring& _s)
	{
		std::transform(_s.begin(), _s.end(), _s.begin(), tolower);
	}

	// Compares strings without case match.
	bool equalStrings(std::string _s1, std::string _s2)
	{
		toLower(_s1);
		toLower(_s2);
		return _s1.compare(_s2) == 0;
	}

	bool equalStrings(std::wstring _s1, std::wstring _s2)
	{
		toLower(_s1);
		toLower(_s2);
		return _s1.compare(_s2) == 0;
	}

	void addBytes(__inout std::string& _str, const char* _p, std::size_t _length)
	{
		_str += std::string(_p, _length);
	}


	void splitString(const std::wstring& _in, const std::wstring& _delimiter, std::vector<std::wstring>& _out)
	{
        if (_delimiter.empty())
            return;
		_out.clear();
		//boost::split(_out, _in, boost::is_any_of(_delimiter)); // Works incorrect - adds empty string to vector if several L'\0' exists in a row
		size_t prev_pos = 0;
		size_t pos = 0;
		std::wstring substring;

		while (pos != std::wstring::npos)
		{
			pos = _in.find(_delimiter, prev_pos);
			substring.assign(_in.substr(prev_pos, pos - prev_pos));
			if (!substring.empty() && substring != std::wstring(1, L'\0')) // This verification distinguish this implementation from boost version
			{
				_out.push_back(substring + std::wstring(1, L'\0'));
			}
			prev_pos = pos + 1;
		}
	}
}

namespace cpu
{
	bool IsCPUx64()
	{
		SYSTEM_INFO si = {0};
		GetNativeSystemInfo(&si);
		return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	}
}

namespace system_power
{
	bool reboot(std::wstring _message, unsigned long _timeoutSeconds, bool _closeAllImmediately)
	{
		const wchar_t* ptrMsg = nullptr;

		if (!_message.empty())
		{
			ptrMsg = _message.c_str();
		}

		return InitiateSystemShutdownW(NULL, const_cast<wchar_t*>(ptrMsg), _timeoutSeconds, _closeAllImmediately, TRUE) == TRUE;
	}
}
