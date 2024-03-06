#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
//#include <fstream>

//#pragma once

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

//#pragma once

#include <Windows.h>
#include <sys\stat.h>


namespace file {


	int getSize( char* pszFile )
	{

#ifdef _WIN32
		// using MSC specific 64-bit filesize and time stamp infos
		//struct __stat64 buf;
		struct _stat buf;
		//if (_stat64( pszFile, &buf ))
		if (_stat( pszFile, &buf ))
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


#define MAX_BUF_SIZE 100000 // 100 Kb
unsigned char abBuf[MAX_BUF_SIZE + 100];

void printhelp( )
{
	printf( "<help> partcopy\n"
		"\n"
		"   [usage] :\n"
		"      partcopy infile offset length outfile [offset2] [-yes]\n"
		"      partcopy infile -allfrom startoffset outfile ...\n"
		"      partcopy infile -fromto startoffset endoffset outfile ...\n"
		"\n"
		"   [options] :\n"
		"      -quiet    do not print number of copied bytes.\n"
		"      -verbose  tell how offsets and lengths are calculated.\n"
		"                type directly after \"partcopy\" for all infos.\n"
		"      -fromto   use this if you don't want to specify offset\n"
		"                and length for the input, but a start and end\n"
		"                offset (with length being end minus start).\n"
		"      -allfrom  copy all from start offset, until end of file.\n"
		"      -noext    do not allow an extension of the output file,\n"
		"                stop with error if writing beyond output end.\n"
		"      -append   if given after output filename, data is added\n"
		"                to the end of the output file.\n"
		"\n"
		"   [description] :\n"
		"      reads a block of bytes from infile at specified offset,\n"
		"      writing them into another file. if offset2 is specified,\n"
		"      outfile is not created, but the bytes are used to overwrite\n"
		"      other bytes within the output file.\n"
		"\n"
		"      negative offsets can be used, to specify positions\n"
		"      relative to the file end instead of file start.\n"
		"\n"
		"      if output offset2 + length is larger than output file size,\n"
		"      the output file is extended. use -noext to avoid this.\n"
		"\n"
		"      length syntax:\n"
		"         100000   ,  100k        = 100000 bytes\n"
		"         2000000k , 2000m , 2g   = 2 giga-bytes\n"
		"\n"
		"   [examples] : \n"
		"      partcopy first.dat 10000 50 second.dat 500000\n"
		"        - copies 50 bytes from first.dat at offset 10000 into\n"
		"          second.dat (which should exist) at offset 500000\n"
		"\n"
		"      partcopy first.dat -fromto 0x2710 0x2950 second.dat\n"
		"        - input start and end offset in hexadecimal numbers,\n"
		"          output file is created (no output offset given).\n"
		"\n"
		"      partcopy in.txt -allfrom 1000 out.txt -append\n"
		"        - take from in.txt all data from offset 1000 until end\n"
		"          and append that to out.txt.\n"
		"\n"
		"      partcopy in.dat -fromto -1000 -900 out.dat -500\n"
		"        - take from in.dat all bytes from end minus 1000\n"
		"          until end minus 900, i.e. a 100 bytes block, and\n"
		"          write this into output at end minus 500 bytes.\n"
		"          (type -verbose after partcopy for details.)\n"
	);
}

void printerror( char* pszErr, ... )
{
	if (pszErr)
	{
		char szErr[1024];
		va_list args;
		va_start( args, pszErr );
		::vsnprintf( szErr, sizeof( szErr ) - 10, pszErr, args );
		va_end( args );

		fprintf( stderr,
			"<error> partcopy\n"
			"%s\n\n",
			szErr
		);
	}
	else
	{
		fprintf( stderr,
			"<error> partcopy - Invalid command line arguments!\n"
			"Usage: partcopy infile offset length outfile [offset2] [-yes]\n"
			"Try 'partcopy -h or partcopy --help' for more information."
		);
	}
}

int main( int argc, char *argv[], char *penv[] )
{
	char *pszSrc = 0;
	char *pszDst = 0;
	int nSrcOff = 0;
	int nDstOff = 0;
	int nCopyLen = 0;
	int nSrcEnd = 0;
	bool bDelDst = 0;
	bool bAbsolute = 0;
	bool bAllFrom = 0;
	bool bHaveSrcOff = 0;
	bool bHaveDstOff = 0;
	bool bDstAppend = 0;
	bool bNoExt = 0;

	int nstate = 1;
	int iDir = 1;
	int iVerbosity = 0;
	int iQuiet = 0;
//	  errno_t err = 0;

	// catch all help requests
	if (argc <= 2)
	{
		if (argc == 2 && 
			(
				   !strcmp( argv[1], "-h" ) || !strcmp( argv[1], "-help" )
				|| !strcmp( argv[1], "?" )  || !strcmp( argv[1], "--help" )
				|| !strcmp( argv[1], "-?" ) || !strcmp( argv[1], "/?" )
				|| !strcmp( argv[1], "/h" ) || !strcmp( argv[1], "/help" )
			)
		)
		{
			printhelp( );
			return 9;
		}
		else 
		{
			printerror( NULL );
			return -1;
		}
	}

	for (; iDir < argc; iDir++)
	{
		char *pszArg = argv[iDir];
	
		if (!strcmp( pszArg, "-fromto" )) 
		{
			bAbsolute = 1;
			continue;
		}
		else
		{
			if (!strcmp( pszArg, "-allfrom" )) 
			{
				bAllFrom = 1;
				continue;
			}
			else
			{
				if (!strcmp( pszArg, "-noext" )) 
				{
					bNoExt = 1;
					continue;
				}
				else 
				{
					if (strbeg( pszArg, "-append" ))
					{
						pszArg = "-0";
						// fall through
					}
					else 
					{
						if (*pszArg == '-' && isdigit( pszArg[1] )) 
						{
							// fall through
						}
						else 
						{
							if (*pszArg == '-') 
							{
								if (!strcmp( pszArg, "-verbose" ))
									iVerbosity = 1;
								else if (!strcmp( pszArg, "-verbose=0" ))
									iVerbosity = 0;
								else if (!strcmp( pszArg, "-verbose=2" ))
									iVerbosity = 2;
								else if (!strcmp( pszArg, "-verbose=3" ))
									iVerbosity = 3;
								else if (!strcmp( pszArg, "-verbose=4" ))
									iVerbosity = 4;
								else if (!strcmp( pszArg, "-quiet" ))
									iQuiet = 1;
								else if (!strcmp( pszArg, "-quiet=1" ))
									iQuiet = 1;
								else if (!strcmp( pszArg, "-quiet=2" ))
									iQuiet = 2;
								else 
								{
									printerror( "error: unknown option: %s\n", pszArg );
									return -1;
								}

								continue;
							}
						}
					}
				}
			}
		}
	
		// process non-option keywords:
		switch (nstate++) 
		{
		case 1:
			pszSrc = pszArg;
			continue;
		case 2:
			nSrcOff = atonum( pszArg );
			bHaveSrcOff = 1;
			if (bAllFrom)
				nstate++;
			continue;
		case 3:
			if (bAbsolute) 
			{
				nSrcEnd = atonum( pszArg );
			}
			else
			{
				nCopyLen = numFromSizeStr( pszArg, false );
				if (nCopyLen == -1)
					return -1;
			}
			continue;
		case 4:
			pszDst = pszArg;
			continue;
		case 5:
			if (!strcmp( pszArg, "-0" )) 
			{
				if (iVerbosity)
					printf( "append to end of output file.\n" );
				bDstAppend = 1;
			}
			nDstOff = atonum( pszArg );
			bHaveDstOff = 1;
			continue;
		}
	
		printerror( "error: unknown option: %s\n", pszArg );
		return -1;
	}

	//printf("%d %d %d\n", bDstAppend, nDstOff, bHaveDstOff);
				
	if (!pszSrc)
	{
		printerror( "missing input filename." );
		return -1;
	}
	if (!pszDst)
	{
		printerror( "missing output filename" );
		return -1;
	}
	if (!bHaveSrcOff)
	{
		printerror( "missing input offset" );
		return -1;
	}


//	  std::streampos inputSize = 0;
//	  std::ifstream inputFile( pszSrc );
//	  if (!inputFile)
//	  {	  
//	  	  printerror( "no such input file: %s", pszSrc );
//	  	  return -1;
//	  }
//	  inputFile.seekg( 0, std::ios::end );
//	  inputSize = inputFile.tellg( ) - inputSize;
//	  inputFile.seekg( 0, std::ios::beg );
//	  int nInFileSize = (int)inputSize;	
  
/*
	int inputSize = 0;
	FILE *inputFile;
	inputFile = fopen(pszSrc,"rb");

	if (!inputFile)
	{	
		printerror( "no such input file: %s", pszSrc );
		return -1;
	}
  
	fseek(inputFile,0,SEEK_END);
	inputSize    = ftell(inputFile) - inputSize;;

	//fseek(inputFile,0,SEEK_SET);
	fclose( inputFile ); 					// if using files in C, close after getting fsize
	
	int nInFileSize = (int)inputSize;
*/
	int nInFileSize = 0;					  // new alt code
	nInFileSize = file::getSize( pszSrc );	  // new alt code
	
	if (nSrcOff < 0) 
	{
		if (iVerbosity)
		{
			printf( "mapping source offset: %d = %d %d\n", nInFileSize + nSrcOff, nInFileSize, nSrcOff );
		}
		nSrcOff = nInFileSize + nSrcOff;
	}
	
	if (bAllFrom) 
	{
		if (iVerbosity)
		{
			printf( "mapping copy length  : %d = %d - %d\n", nInFileSize - nSrcOff, nInFileSize, nSrcOff );
		}
		nCopyLen = nInFileSize - nSrcOff;
	}
	
	if (bAbsolute == 1) 
	{
		if (nSrcEnd < 0) 
		{
			if (iVerbosity)
			{
				printf( "mapping source end   : %d = %d %d\n", nInFileSize + nSrcEnd, nInFileSize, nSrcEnd );
			}
			nSrcEnd = nInFileSize + nSrcEnd;
		}
	
		if (iVerbosity)
		{
			printf( "mapping copy length  : %d = %d - %d\n", nSrcEnd - nSrcOff, nSrcEnd, nSrcOff );
		}
	
		nCopyLen = nSrcEnd - nSrcOff;
		if (nCopyLen < 0)
		{
			printerror( "end offset is lower than start offset" );
			return -1; 
		}
	}
	
	if (nCopyLen <= 0)
	{
		printerror( "invalid copy length: %d\n", nCopyLen );
		return -1;
	}
	
	// output offset is optional.
	int nOutFileSize = -1;
	//if (bHaveDstOff && nDstOff != 0)
	if (bHaveDstOff)
	{
		nOutFileSize = file::getSize( pszDst );
		if (nOutFileSize < 0)
		{
			printerror( "destination offset set, but no such destination file exists: %s", pszDst );
			return -1;
		}
	
		if (bDstAppend)
		{
			nDstOff = nOutFileSize;
		}
		else
		{
			if (nDstOff < 0)
				nDstOff = nOutFileSize + nDstOff;
		}
	
		if (nDstOff > nOutFileSize)
		{
			printerror( "output offset %d is larger than output file (%d)\n", nDstOff, nOutFileSize );
			return -1;
		}
	}
	else 
	{
		bDelDst = 1;
		nDstOff = 0;
	}
	
	int nSrcOffMax = nSrcOff + nCopyLen;
	if (nInFileSize < nSrcOffMax)
	{
		printerror( "input file %s has a size of %d bytes, cannot copy requested part.", pszSrc, nInFileSize );
		return -1;
	}
	
	if (!bDelDst) 
	{
		// check target file size
		if (nOutFileSize < 0)
		{
			printerror( "no such output file: %s", pszDst );
			return -1;
		}
		
		int nDstOffMax = nDstOff + nCopyLen;
		if (nDstOffMax > nOutFileSize)
		{
			if (bNoExt)
			{
				printerror( "would copy beyond end of output file (%d).\n", nOutFileSize );
				return -1;
			}
	
			if (iVerbosity)
			{
				printf( "output file has a size of %d bytes and will be extended.\n", nOutFileSize );
			}
		}
	}
	
	FILE *fin = 0;
	FILE *fout = 0;
	
	fin = fopen(pszSrc, "rb" );
	if (!fin)
	{
		printerror( "unable to open input file: %s\n", pszSrc );
		return -1;
	}
	fout = fopen(pszDst, bDelDst ? "wb" : "r+b" );
	if (!fout)
	{ 
		fclose( fin ); 
		printerror( "unable to write: %s - error %d\n", pszDst, errno );
		return -1; 
	}	   

/*
	err = fopen_s( &fin, pszSrc, "rb" );
	if (!fin || err)
	{
		printerror( "unable to open input file: %s\n", pszSrc );
		return -1;
	}
	
	err = fopen_s( &fout, pszDst, bDelDst ? "wb" : "r+b" );
	if (!fout || err)
	{ 
		fclose( fin ); 
		printerror( "unable to write: %s - error %d\n", pszDst, err );
		return -1; 
	}
*/	  
	// seek to target locations
	if (fseek( fin, nSrcOff, SEEK_SET )) 
	{
		fclose( fin ); 
		if (fout) 
			fclose( fout );
		printerror( "unable to seek within source file\n" );
		return -1;
	}
	
	if (fseek( fout, nDstOff, SEEK_SET ))
	{
		fclose( fin ); 
		fclose( fout );
		printerror( "unable to seek within target file\n" );
		return -1;
	}
	
	// Verbose info
	if (iVerbosity) 
	{
		printf( "copying %d bytes, ", nCopyLen );
		printf( "from input offset %d ", nSrcOff );
		printf( "to %d, ", nSrcOffMax );
		printf( "to target offset %d\n", nDstOff );
	}
	
	// copy binary part
	int nRemain = nCopyLen;
	int nTotal = 0;
	int retVal = 0;
	
	while (nRemain > 0)
	{
		int nMaxRead = sizeof( abBuf );
		if (nMaxRead > nRemain)
			nMaxRead = (int)nRemain;
	
		int nread = fread( abBuf, 1, nMaxRead, fin );
		if (nread <= 0) 
		{
			printerror( "unable to fully read part from %s, copy failed", pszSrc );
			retVal = -1;
			break; // EOF on input
		}
	
		if (fout) 
		{
			int nwrite = fwrite( abBuf, 1, nread, fout );
			if (nwrite != nread) 
			{
				printerror( "error while writing: %s   \n", pszDst );
				retVal = -1;
				break;
			}
		}
	
		nRemain -= nread;
		nTotal += nread;
	}
	
	if (fout) 
	{
		fclose( fout );
	}
	
	fclose( fin );
	
	if (!iQuiet)
	{
		printf( "%d bytes copied.\n", nTotal );
	}

	return retVal;
}

