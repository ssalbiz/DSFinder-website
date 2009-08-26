//
// C++ Implementation: platform_fs
//
// Description:
//
//
// Author: André Simon <andre.simon1@gmx.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "platform_fs.h"
#include "./dirstream0.4/dirstream.h"

#include <iostream>

using namespace std;

namespace Platform {

#ifdef _WIN32
	#include <windows.h>

	const char pathSeparator = '\\';
	//const std::string pathSeparatorStr = "\\";

	std::string getAppPath()
	{
		char pathAndName[MAX_PATH], path[MAX_PATH], drive[3];
		GetModuleFileName(NULL, pathAndName, MAX_PATH);
		_splitpath(pathAndName, drive, path, 0, 0);
		return std::string(drive)+path;
	}

#else
       const char pathSeparator = '/';
      // const std::string pathSeparatorStr = "/";

	std::string getAppPath()
	{
		return "";
	}

#endif

bool getDirectoryEntries(vector<string> &fileList,
                string wildcard,
                bool recursiveSearch)
{
  if (!wildcard.empty()) {
	string directory_path;
	string::size_type Pos = wildcard.find_last_of(pathSeparator);
	if (Pos == string::npos) {
		directory_path = ".";
	} else {
		directory_path = wildcard.substr(0, Pos + 1);
		wildcard = wildcard.substr(Pos + 1);
	}

	dirstr::dirstream str( directory_path.c_str(),
                              #ifdef USE_FN_MATCH
                                dirstr::pred_f(FnMatcher(wildcard.c_str(), 0)),
                              #else
                                dirstr::pattern_f(wildcard.c_str()),
                              #endif
                              (recursiveSearch)?dirstr::recursive_yes:dirstr::recursive_no);


	for(string entry; str >> entry;) {
		fileList.push_back(dirstr::full_path(entry));
		//std::cout  << "Entry " <<entry<<"\n";
	}
  }
  return !(fileList.empty());
}

}

