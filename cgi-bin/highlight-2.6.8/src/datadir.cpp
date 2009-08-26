/***************************************************************************
                          dataDir.cpp  -  description
                             -------------------
    begin                : Sam March 1 2003
    copyright            : (C) 2003 by André Simon
    email                : andre.simon1@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include <fstream>
#include <vector>
#include "platform_fs.h"

#include "datadir.h"

using namespace std;

bool DataDir::searchDataDir(const string &userDefinedDir){

#ifndef _WIN32
  bool found = false;

  vector <string> possibleDirs;
  if (!userDefinedDir.empty()) possibleDirs.push_back(userDefinedDir);
  #ifdef HL_DATA_DIR
  possibleDirs.push_back(HL_DATA_DIR);
  #endif
  possibleDirs.push_back("/usr/share/highlight/");

  for (int i=0;i<possibleDirs.size();i++)
    {
      if (fileExists(possibleDirs[i]))
        {
          dataDir=possibleDirs[i];
          found = true; break;
        }
    }
  return found;
#else
  dataDir=userDefinedDir;
  return true;
#endif
}

DataDir::DataDir()
{
}

void DataDir::setAdditionalDataDir(const string& dir){
  additionalDataDir=dir;
}
void DataDir::setAdditionalConfDir(const string& dir){
  additionalConfDir=dir;
}

const string & DataDir::getAdditionalDataDir(){
  return additionalDataDir;
}

const string & DataDir::getAdditionalConfDir(){
  return additionalConfDir;
}

const string &DataDir::getDir()
{
  return dataDir;
}

const string DataDir::getLangPath(const string & file, bool forceDefault)
{
    if (!forceDefault && !additionalDataDir.empty()){
       string path=getAdditionalLangDefDir()+file;
       if (fileExists(path)){
          return path;
       }
    }
    return dataDir+"langDefs"+Platform::pathSeparator+file;
}

const string DataDir::getThemePath(const string & file, bool forceDefault)
{
    if (!forceDefault && !additionalDataDir.empty()){
       string path=getAdditionalThemeDir()+file;
       if (fileExists(path)){
          return path;
       }
    }
    return dataDir+"themes"+Platform::pathSeparator+file;
}

const string DataDir::getIndentSchemesPath(const string & file, bool forceDefault)
{
    if (!forceDefault && !additionalDataDir.empty()){
       string path=getAdditionalIndentSchemesDir()+file;
       if (fileExists(path)){
          return path;
       }
    }
    return dataDir+"indentSchemes"+Platform::pathSeparator+file;
}

const string DataDir::getConfDir( bool forceDefault) {
    if (!forceDefault && !additionalConfDir.empty()){
       return additionalConfDir;
    }
  #ifndef _WIN32
     #ifdef HL_CONFIG_DIR
     return HL_CONFIG_DIR;
     #else
     return "/etc/highlight/";
     #endif
  #else
     return getDir();
  #endif
}

const string DataDir::getHelpMsgDir()
{
  return dataDir+"helpmsg"+Platform::pathSeparator;
}

const string DataDir::getAdditionalLangDefDir()
{
  return additionalDataDir+"langDefs"+Platform::pathSeparator;
}

const string DataDir::getAdditionalThemeDir()
{
  return additionalDataDir+"themes"+Platform::pathSeparator;
}

const string DataDir::getAdditionalIndentSchemesDir()
{
  return additionalDataDir+"indentSchemes"+Platform::pathSeparator;
}

const string  DataDir::getI18nDir(){
  return dataDir+"gui_files"+Platform::pathSeparator+"i18n"+Platform::pathSeparator;
}

const string  DataDir::getExtDir(){
  return dataDir+"gui_files"+Platform::pathSeparator+"ext"+Platform::pathSeparator;
}

const string DataDir::getDocDir() {
  #ifndef _WIN32
     #ifdef HL_DOC_DIR
     return HL_CONFIG_DIR;
     #else
     return "/usr/share/doc/highlight/";
     #endif
  #else
     return getDir();
  #endif
}

bool DataDir::fileExists(const string&f){
  ifstream file(f.c_str());
  bool exists=!file.fail();
  file.close();
  return exists;
}
