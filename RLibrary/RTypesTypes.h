//RTypesTypes.h - Provides basic types for RLibrary
#ifndef RTYPESTYPES_H
#define RTYPESTYPES_H

namespace rtypes
{
	// numeric types
#ifndef _BYTE_TDEF_DEF_
#define _BYTE_TDEF_DEF_ // this needs to be defined when any "typedef unsigned char byte" exists; specifically, this prevents a clash with <Windows.h>
	typedef unsigned char byte;
#endif
	typedef unsigned short word;
	typedef unsigned int dword;
	typedef unsigned long long qword;
	//

	// enumerations
	enum rlib_numeric_rep_flag
	{
		binary = 2,
		octal = 8,
		decimal = 10,
		hexadecimal = 16
	};
	//
}

#endif