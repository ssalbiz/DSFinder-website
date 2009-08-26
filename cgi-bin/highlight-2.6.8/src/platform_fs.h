//
// C++ Interface: platform_fs
//
// Description:
//
//
// Author: André Simon <andre.simon1@gmx.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLATFORM_FS__H__INCLUDED
#define PLATFORM_FS__H__INCLUDED

#include <string>
#include <iostream>
#include <vector>

#ifdef USE_FN_MATCH
  #include <fnmatch.h>
#endif

namespace Platform
{
	extern const char pathSeparator;
	//extern const std::string pathSeparatorStr;

	std::string getAppPath();

	/** \param fileList Vector where found entries will be stored
        \param  wildcard Directory path and wildcard
        \param recursiveSearch Test if directory should be searched recursively */
        bool getDirectoryEntries(std::vector<std::string> &fileList,
                           std::string wildcard,
                           bool recursiveSearch=false);

#ifdef USE_FN_MATCH
  struct FnMatcher
  {
    FnMatcher(const char* pattern, int flags)
        : pattern_(pattern)
        , flags_(flags)
    {}
    bool operator()(const std::string& e) const {
	  //  std::cout << "pattern: "<<pattern_<<  "   entry: "<<e.c_str()<< "  Res fn: " <<::fnmatch(pattern_, e.c_str(), FNM_PATHNAME)<< " \n";
        return ! ::fnmatch(pattern_, e.c_str(), flags_);
    }
   private:
    const char* pattern_;
    int flags_;
  };
#endif
}
#endif
