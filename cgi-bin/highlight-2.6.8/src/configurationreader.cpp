/***************************************************************************
                          configurationreader.cpp  -  description
                             -------------------
    begin                : Son Nov 10 2002
    copyright            : (C) 2002 by Andre Simon
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

#include "configurationreader.h"

#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>

#include "stringtools.h"

using namespace std;

ConfigurationReader::ConfigurationReader(const string & configuration_path)
{
  ifstream in (configuration_path.c_str());
  fileFound=in;
  if (fileFound) {
    string line;
    line.reserve(500);
    size_t lineBegin;
    size_t delimPos;
    string paramName, paramValue;
    while (getline(in, line)) {
      lineBegin=line.find_first_not_of("\t ");
      if ((line.size()>2) && (lineBegin!=string::npos)
            && (line.at(lineBegin)!='#')) {  //comment?
          if (line[lineBegin]=='$') {       // neuer Parametername?
             delimPos=line.find("=",lineBegin)-1;
             if (delimPos!=string::npos) {
                 paramName=StringTools::trimRight(
                           StringTools::lowerCase(line.substr(lineBegin+1, delimPos)));
                 parameterNames.push_back(paramName);
                 paramValue=line.substr(delimPos+2, line.length());
              }
            } else  { // line belongs to last parameter
                paramValue=line;
            }
            if (parameterMap[paramName].empty())  {
               parameterMap[paramName] = paramValue;
            } else {
                parameterMap[paramName]+= (" "+paramValue);
            }
         }  //if ((lineBegin!=string::npos) && (line.at(lineBegin)!='#'))
      } //while
    in.close();
  } //if (in)
}

ConfigurationReader::~ConfigurationReader()
{
}

bool ConfigurationReader::found()
{
  return fileFound;
}

string &ConfigurationReader::getParameter(const string & paramName)
{
  return parameterMap[paramName] ;
}

const char* ConfigurationReader::getCParameter(const string & paramName)
{
  return parameterMap[paramName].c_str() ;
}

vector<string> &ConfigurationReader::getParameterNames()
{
  return parameterNames;
}


