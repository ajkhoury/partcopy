#pragma once

#include <string.h>

bool strbeg( char* pszStr, char* pszPat )
{
	if (!strncmp( pszStr, pszPat, strlen( pszPat ) ))
		return 1;
	return 0;
}

bool strcase( char *pszStr, char *pszPat ) 
{
	return strcmp( pszStr, pszPat ) ? 0 : 1;
}

// atonum with support for decimal and 0x hex values
int atonum( char *psz )
{
	if (!strncmp( psz, "0x", 2 )) 
	{
#ifdef _MSC_VER
		return (int)_strtoui64( psz + 2, 0, 16 );
#else
		return (int)strtoull( psz + 2, 0, 16 );
#endif
	}
	else 
	{
#ifdef _WIN32
		return (int)_atoi64( psz );
#else
		return (int)atoll( psz );
#endif
	}
}

// sizefromstr
int numFromSizeStr( char *psz, bool bRelaxed )
{
	int nLen = strlen( psz );
	if (nLen >= 1) 
	{
		int lNum = atonum( psz );
		if (!strncmp( psz, "0x", 2 ))
			return lNum; // hex: always byte values
		char cPostFix = psz[nLen - 1];
		switch (cPostFix) {
		case 'b': return lNum;
		case 'k': return lNum * 1000;
		case 'K': return lNum * 1024;
		case 'm': return lNum * 1000000;
		case 'M': return lNum * 1048576;
		case 'g': return lNum * 1000000000;
		case 'G': return lNum * 1073741824;
		default:
			if (bRelaxed)
				return lNum;
			if (cPostFix >= '0' && cPostFix <= '9')
				return lNum; // no postfix at all: assume bytes
		}
	}
	fprintf( stderr, 
			"unexpected value: %s\n"
			"supply a value like 1000 500k 100m 5M [k=kbyte,m=mbyte]\n", psz );
	return -1;
}
