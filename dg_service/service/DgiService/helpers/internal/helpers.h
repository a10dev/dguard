//
//	Author: 
//			burluckij@gmail.com
//			Burlutsky Stanislav
//
//

#pragma once

#include <new>
#include <string>
#include <vector>
#define _WINSOCKAPI_
#include <windows.h>
#undef _WINSOCKAPI_
#include <set>
#include <map>
#include <ctime>
#include <stdint.h>
#include "log.h"
#include <lm.h>

#define NETWORK_ERROR		-1

#define COUNT_OF_CHARS(char_buffer) (sizeof(char_buffer) / sizeof(char_buffer[0]))
#define SET_LAST_ZERO_CHAR(char_buffer) (char_buffer[COUNT_OF_CHARS(char_buffer) - 1] = 0)

// Copies N wide-chars to new array and sets to zero last symbol in '_dest_array'. 
#define fill_wchars(_dest_array, _src_array)	\
	ZeroMemory(_dest_array, sizeof(_dest_array)); \
	wcsncpy(_dest_array, _src_array, COUNT_OF_CHARS(_dest_array) - 1);
	//wcsncpy(_dest_array, _src_array, COUNT_OF_CHARS(_dest_array));

//	wcscpy_s(_dest_array, COUNT_OF_CHARS(_dest_array) - 1, _src_array); \

// Copies N chars to new array and sets to zero last symbol in '_dest_array'. 
#define fill_chars(_dest_array, _src_array)	\
	ZeroMemory(_dest_array, sizeof(_dest_array)); \
	strncpy(_dest_array, _src_array, COUNT_OF_CHARS(_dest_array) - 1);
	// strncpy(_dest_array, _src_array, COUNT_OF_CHARS(_dest_array));

//strcpy_s(_dest_array, COUNT_OF_CHARS(_dest_array) - 1, _src_array);
	//_dest_array[COUNT_OF_CHARS(_dest_array) - 1] = 0;
	//memset(&_dest_array[COUNT_OF_CHARS(_dest_array) - 1], 0, sizeof(_dest_array[0]));

namespace process
{
	std::string getFilePath(DWORD _pid);

	bool find(std::string _name,
		bool _fullMatch,
		std::string& _procImageFilePath,
		DWORD& _pid);
}

namespace timefn
{
	int getTimeoutValue(const SYSTEMTIME& _scheduledTime, unsigned int _waitPeriod);
	std::string getCurrentTime();
	std::string getTime(const SYSTEMTIME& _time);

	// get timestamp in milliseconds since unix epoch
	int64_t getTimestamp();

	//std::string toDateStr(int64_t _timestamp);
}

namespace network
{
	int sendAll(int s, const char* buf, int len, int flags);
	int recvAll(int s, char* buf, int len, int flags);

	// Returns only two types values:
	// 0 - when connection closed.
	// len - count of read bytes.
	//int recvAll(int _hSocket, std::string& _outBuf, int len);

	// This method succeeds only when all data was read.
	bool read(int _hSocket, std::string& _outBuf, int len);

	//
	// Returns list of local IP addresses.
	// First entry is a main local IP.
	//
	std::vector<std::string> GetLocalIpAddrs();
	
	std::vector<std::string> GetIpList1();
	std::string GetCurrentIp();
	std::vector<std::pair<std::string, std::string>> GetMacs();
	std::string GetMac();
	std::string GetMacByIP(std::string ip);

	// Creates socket for local server.
	int createListenSocket(int _port);
}

namespace memory
{
	// Allocates bytes array with C++ operator - new [].
	// Returns NULL ptr if couldn't allocate memory.
	PVOID getmem(size_t x) throw();

	// Free memory function.
	// x - could be a NULL pointer.
	void freemem(PVOID x);
}

namespace winerror
{
	// DWORD _dwErrCode = GetLastError(); shold be called before
	std::string getLastErrorCodeMessage(DWORD _dwErrCode);
}

namespace strings
{
	std::string ws_s(const std::wstring& s, UINT cp = CP_ACP);
	std::wstring s_ws(const std::string& s, UINT cp = CP_ACP);

	void toUpper(std::string& _s);
	void toLower(std::string& _s);

	void toUpper(std::wstring& _s);
	void toLower(std::wstring& _s);

	// Compares strings without case match.
	bool equalStrings(std::string _s1, std::string _s2);
	bool equalStrings(std::wstring, std::wstring);

	void addBytes(std::string& _str, const char* _p, std::size_t _length);

	std::string generateUuid();

	// split string into substrings by delimiter, empty strings ignoring unlike boost::split
	void splitString(const std::wstring& _in, const std::wstring& _delimiter, std::vector<std::wstring>& _out);


	void SerializeString(std::ostringstream& _os, const std::string& _inStr);

	void DeserializeString(std::istringstream& _is, std::string& _outStr);

	void SerializeWString(std::ostringstream& _os, const std::wstring& _inStr);

	void DeserializeWString(std::istringstream& _is, std::wstring& _outStr);

	void WStringToBuffer(const std::wstring& _inStr, std::vector<char>& _outBuf);

	std::wstring BufferToWString(const std::vector<char>& _inBuf);
}

namespace cpu
{
	bool IsCPUx64();
}

namespace system_power
{
	bool reboot(std::wstring _message, unsigned long _timeoutSeconds, bool _closeAllImmediately);
}

