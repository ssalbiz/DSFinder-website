/////////////////////////////////////////////////////////////////////////////
//	file		:	dirstreamw32.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Deklarationen die unter Win32 nötig sind.
/////////////////////////////////////////////////////////////////////////////
#ifndef DIRSTREAMW32_H_INCLUDED
#define DIRSTREAMW32_H_INCLUDED
/*

    Declaration of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.

*/
typedef struct DIR DIR;
struct dirent
{
	char *d_name;
	unsigned attrib; // BK: added to allow efficient isDir-Query
};

DIR*			opendir(const char *);
int				closedir(DIR *);
struct dirent*	readdir(DIR *);
void rewinddir(DIR *dir);
/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.

    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/

namespace dirstr 
{
	bool isDir(unsigned attrib);
	const char PATH_SEPERATOR = '\\';
}


#endif

