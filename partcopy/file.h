#pragma once

#include <Windows.h>
#include <sys\stat.h>


namespace file {


	int getSize( char* pszFile )
	{
#ifdef _WIN32
		// using MSC specific 64-bit filesize and time stamp infos
		struct __stat64 buf;
		if (_stat64( pszFile, &buf ))
			return -1;
		return (int)buf.st_size;

#else
		// generic linux 64-bit stat
		struct stat64 buf;
		if (stat64( pszName, &buf ))
			return -1;
		return (int)buf.st_size;
#endif
	}

	bool exists( char* pszFile )
	{
		struct stat buf;
		return (stat( pszFile, &buf ) == 0);
	}




}