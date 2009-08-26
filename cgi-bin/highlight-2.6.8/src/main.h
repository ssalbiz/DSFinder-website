//
// C++ Interface: main
//
// Description:
//
//
// Author: Andre Simon <andre.simon1@gmx.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef HIGHLIGHT_APP
#define HIGHLIGHT_APP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cassert>

#include "./dirstream0.4/dirstream.h"
#include "cmdlineoptions.h"
#include "configurationreader.h"
#include "codegenerator.h"
#include "help.h"
#include "datadir.h"
#include "version.h"
#include "platform_fs.h"

#define IO_ERROR_REPORT_LENGTH 5
#define SHEBANG_CNT 12

typedef map<string, string> StringMap;

/// Main application class of the command line interface

class HighlightApp {

public:

   HighlightApp(){};
   ~HighlightApp(){};

   /** Start application
     \param argc Number of command line arguments
     \param argv values of command line arguments
     \return EXIT_SUCCESS or EXIT_FAILURE
   */
   int run(int argc, char **argv);

private:

  DataDir dataDir;
  StringMap extensions;
  StringMap scriptShebangs;
  stringstream cin_bufcopy;

  /** print version info*/
  void printVersionInfo();

  /** print configuration info*/
  void printConfigInfo(const string&);

  /** print error message*/
  void printBadInstallationInfo();

  /** print input and output errors */
  void printIOErrorReport(unsigned int numberErrorFiles, vector<string> & fileList, const string &action);

  /** print installed files
      \param showThemes Print installed themes if true, language definitions otherwise
  */
  bool listInstalledFiles(bool showThemes);

  void printDebugInfo(const highlight::LanguageDefinition &lang,
                      const string &langDefPath);

  string getFileSuffix(const string &fileName);

  string guessFileType(const string &suffix, const string &inputFile="");

  int getNumDigits(int i);

  void printProgressBar(int total, int count);
  void printCurrentAction(const string&outfilePath,
                          int total, int count, int countWidth);

  bool readInputFilePaths(vector<string> &fileList, string wildcard,
                          bool recursiveSearch);

  string analyzeFile(const string& file);
  bool loadFileTypeConfig(const string& name, StringMap* map, StringMap* shebangMap);

};

#endif
