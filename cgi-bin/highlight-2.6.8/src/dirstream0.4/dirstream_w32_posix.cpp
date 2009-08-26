/////////////////////////////////////////////////////////////////////////////
//	file		:	dirstream_w32_posix.cpp
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
// Implementation der platformspezfischen Funktionalität
/////////////////////////////////////////////////////////////////////////////
#include "dirstream.h"
#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <io.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>
/*

    Implementation of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.

*/

struct DIR
{
    long                handle; /* -1 for failed rewind */
    struct _finddata_t  info;
    struct dirent       result; /* d_name null iff first time */
    char                *name;  /* null-terminated char string */
};

DIR *opendir(const char *name)
{
    DIR *dir = 0;

    if(name && name[0])
    {
        size_t base_length = strlen(name);
        const char *all = /* search pattern must end with suitable wildcard */
            strchr("/\\", name[base_length - 1]) ? "*" : "/*";

        if((dir = (DIR *) malloc(sizeof *dir)) != 0 &&
           (dir->name = (char *) malloc(base_length + strlen(all) + 1)) != 0)
        {
            strcat(strcpy(dir->name, name), all);

            if((dir->handle = (long) _findfirst(dir->name, &dir->info)) != -1)
            {
                dir->result.d_name = 0;
            }
            else /* rollback */
            {
                free(dir->name);
                free(dir);
                dir = 0;
            }
        }
        else /* rollback */
        {
            free(dir);
            dir   = 0;
            errno = ENOMEM;
        }
    }
    else
    {
        errno = EINVAL;
    }

    return dir;
}

int closedir(DIR *dir)
{
    int result = -1;

    if(dir)
    {
        if(dir->handle != -1)
        {
            result = _findclose(dir->handle);
        }

        free(dir->name);
        free(dir);
    }

    if(result == -1) /* map all errors to EBADF */
    {
        errno = EBADF;
    }

    return result;
}

struct dirent *readdir(DIR *dir)
{
    struct dirent *result = 0;

    if(dir && dir->handle != -1)
    {
        if(!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
        {
            result         = &dir->result;
            result->d_name = dir->info.name;
			result->attrib = dir->info.attrib;	// BK: added
        }
    }
    else
    {
        errno = EBADF;
    }

    return result;
}

void rewinddir(DIR *dir)
{
    if(dir && dir->handle != -1)
    {
        _findclose(dir->handle);
        dir->handle = (long) _findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
    }
    else
    {
        errno = EBADF;
    }
}


/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.
    
    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/
#else	// POSIX
#include <stdlib.h>
#endif

namespace dirstr {
bool is_directory(const std::string& n)
{
	struct stat info;
	::stat(n.c_str() , &info);
#ifdef _WIN32
	return (info.st_mode & _S_IFDIR) != 0;
#else
	return S_ISDIR(info.st_mode);
#endif
}

std::string full_path(const std::string& entry)
{
#ifdef _WIN32
	char buffer[MAX_PATH];
	::GetFullPathName(entry.c_str(), MAX_PATH, buffer, 0);
	return buffer;
#else
	char buffer[PATH_MAX];
	if (!::realpath(entry.c_str(), buffer)) return "";
	return buffer;	
#endif
}

bool create_directory(const std::string& path)
{
#ifdef _WIN32
	return ::CreateDirectory(path.c_str(), 0) != 0;
#else
	return ::mkdir(path.c_str(), S_IRWXU|S_IRWXG|S_IRWXO ) == 0;
#endif
}

bool directory_exists(const std::string& path)
{
#ifdef _WIN32
	DWORD attribs = ::GetFileAttributes(path.c_str());
	return attribs != 0xFFFFFFFF && (attribs & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat info;
	return ::stat( path.c_str(), &info ) == 0 && S_ISDIR(info.st_mode);
#endif
}

#ifdef _WIN32
bool isDir(unsigned attrib)
{
	return (attrib & _A_SUBDIR) != 0;
}
#endif
}

