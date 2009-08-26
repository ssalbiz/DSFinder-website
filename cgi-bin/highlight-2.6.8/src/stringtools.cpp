/***************************************************************************
                          stringtools.cpp  -  description
                             -------------------
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by Andre Simon
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

#include "stringtools.h"

// Avoid problems with isspace and UTF-8 characters, use iswspace
#include <wctype.h>


using namespace std;

namespace StringTools
{

string lowerCase(const string& s)
{
  char* buf = new char[s.length()];
  s.copy(buf, s.length());
  for(unsigned int i = 0; i < s.length(); i++)
    buf[i] = tolower(buf[i]);
  string r(buf, s.length());
  delete[] buf;
  return r;
}

// TODO only one function to change case
string upperCase(const string& s)
{
  char* buf = new char[s.length()];
  s.copy(buf, s.length());
  for(unsigned int i = 0; i < s.length(); i++)
    buf[i] = toupper(buf[i]);
  string r(buf, s.length());
  delete[] buf;
  return r;
}

string capitalize(const string& s)
{
  char* buf = new char[s.length()];
  s.copy(buf, s.length());
  if (s.length()) buf[0] = toupper(buf[0]);
  for(unsigned int i = 1; i < s.length(); i++)
    buf[i] = tolower(buf[i]);
  string r(buf, s.length());
  delete[] buf;
  return r;
}

string trimRight(const string &value)
 {
  string::size_type where = value.find_last_not_of(" \t\r");

  if (where == string::npos)
   // string has nothing but space
   return string();

  if (where == (value.length() - 1))
   // string has no trailing space, don't copy its contents
   return value;

  return value.substr(0, where + 1);
 }

string getParantheseVal(const string &s){
  string::size_type openPos=s.find('(');
  string::size_type closePos=s.rfind(')');
  if (openPos ==string::npos || closePos==string::npos){
    return string();
  }
  return s.substr(openPos+1, closePos-openPos-1);
}

vector<string> splitString(const string& s, unsigned char delim){
  string::size_type pos=s.find(delim), oldPos=0;
  vector <string> results;

  if (pos ==string::npos) {
    if (!s.empty())results.push_back(s);
    return results;
  }

  do {
     if (oldPos-pos) results.push_back (s.substr(oldPos, pos-oldPos));
     oldPos=pos+1;
     pos=s.find(delim, pos+1);
  } while (pos!=string::npos);
  results.push_back (s.substr(oldPos));

  return results;
}

}
