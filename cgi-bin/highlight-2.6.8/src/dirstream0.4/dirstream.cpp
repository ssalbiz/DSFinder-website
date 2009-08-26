/////////////////////////////////////////////////////////////////////////////
//	file		:	dirstream.cpp
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Implementation des dirstreams
/////////////////////////////////////////////////////////////////////////////
#include "dirstream.h"
#include <cstring>
namespace dirstr {
///////////////////////////////////////////////////////////////////////////////
// dirstream
///////////////////////////////////////////////////////////////////////////////
dirstream::dirstream(const char* dirName, const filter_base& entryFilter,
					 recurse_mode recMode, const filter_base& subDirFilter)
	: m_EntryFilter(entryFilter.clone())
	, m_SubDirFilter(subDirFilter.clone())
	, m_RecurseMode(recMode)
	, m_DirName(dirName)
	, m_DirHandle(opendir(dirName))
	, m_CurrentEntry(0)
{
    assert(dirName);
	commonInit();
}

dirstream::~dirstream()
{
	closeDir();
}

void dirstream::commonInit()
{
    if (!m_DirHandle)
	{
		throw open_error(m_DirName.c_str());
	}

    if (!m_DirName.empty() && *m_DirName.rbegin() != PATH_SEPERATOR)
	{
		m_DirName += PATH_SEPERATOR;
	}
	if (m_DirName.length() == 2 && m_DirName[0] == '.' && m_DirName[1] == PATH_SEPERATOR)
		m_CurrentPath = "";
	else
		m_CurrentPath = m_DirName;
}

bool dirstream::dotOrDotDot(const std::string& str) const
{
	using namespace std;
	return strcmp(m_CurrentEntry->d_name, ".") == 0
		|| strcmp(m_CurrentEntry->d_name, "..") == 0;
}

void dirstream::closeDir()
{
	if (m_DirHandle)
	{
		closedir(m_DirHandle);
		m_DirHandle = 0;
		m_CurrentEntry = 0;
	}
}

bool dirstream::selectSubDir(const std::string& entry) const
{
	return	m_RecurseMode == recursive_yes && isDirectory(entry.c_str()) 
			&& !dotOrDotDot(entry) &&
			(!m_SubDirFilter.get() || (*m_SubDirFilter)(entry));
}

bool dirstream::selectEntry(const std::string& entry) const
{
	return (!m_EntryFilter.get() || (*m_EntryFilter)(entry));
}

dirstream& dirstream::operator >> (std::string& Dest)
{
	readsome(&Dest, 1);
	return *this;
}

std::streamsize dirstream::readsome(std::string* r, std::streamsize n)
{
	if (n <= 0) return 0;
	std::streamsize count = 0; 
	while (count != n && m_DirHandle)
	{
		if ((m_CurrentEntry  = readdir(m_DirHandle)) != 0)
        {
			r[count] = m_CurrentPath;
			r[count] += m_CurrentEntry->d_name;
			if (selectSubDir(r[count]))
			{
				m_Dirs.push(r[count] + PATH_SEPERATOR);
			}
			if (selectEntry(r[count]))
			{	
				++count;
			}
        }
        else
        {   // In diesem Verzeichnis sind keine Einträge mehr vorhanden.
            // Falls gewünscht, nächstes Unterverzeichnis öffnen...
            closeDir();
            if (m_RecurseMode == recursive_yes && !m_Dirs.empty())
            {
                m_CurrentPath = m_Dirs.top();
                m_Dirs.pop();
                if ( !(m_DirHandle = opendir(m_CurrentPath.c_str())) )
                    throw open_error(m_CurrentPath.c_str());
            }
        }
	}
    return count;
}

bool dirstream::isDirectory(const char* entry) const
{
	if (!m_CurrentEntry) return false;
#ifdef _WIN32
	return dirstr::isDir(m_CurrentEntry->attrib);
#else
	return dirstr::is_directory(entry);
#endif
}

bool dirstream::open(const char* dirName, const filter_base& ef, recurse_mode rm, const filter_base& sdf)
{
	close();
	m_EntryFilter = std::auto_ptr<filter_base>(ef.clone());
	m_SubDirFilter = std::auto_ptr<filter_base>(sdf.clone());
	m_RecurseMode = rm;
	if (! (m_DirHandle = opendir(dirName)) ) 
	{
		return false;
	}
	m_DirName = dirName;
	commonInit();
	return true;
}

bool dirstream::open(const char* dirName, recurse_mode rm)
{
	close();
	m_RecurseMode = rm;
	if (! (m_DirHandle = opendir(dirName)) ) 
	{
		return false;
	}
	m_DirName = dirName;
	commonInit();
	return true;
}

bool dirstream::is_open() const
{
	return !m_DirName.empty();
}

void dirstream::close()
{
	closeDir();
	m_DirName = "";
	while (!m_Dirs.empty()) 
	{
		m_Dirs.pop();
	}
}

void dirstream::rewind()
{
	assert(is_open());
	if (m_DirHandle)
    {
		rewinddir(m_DirHandle);
	}
	else
	{
		open(m_DirName.c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////
// Sonstiges
///////////////////////////////////////////////////////////////////////////////
filter_base::~filter_base() {}


}
