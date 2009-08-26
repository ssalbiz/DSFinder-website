/***************************************************************************
                          stringtools.h  -  description
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

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>
#include <vector>
#include <sstream>

using namespace std;

/// Contains methods for string manipulation

namespace StringTools
  {

  /** convert string to lowercase
      \param s String
      \returns lowercase string  */
  string lowerCase(const string &s);

  /** convert string to uppercase
      \param s String
      \returns uppercase string  */
  string upperCase(const string &s);

  /** convert string to capitalized string
      \param s String
      \returns capitalized string  */
  string capitalize(const string &s);

  /** \param value String
     \return string trimmed on the left side
  */
  string trimRight(const string &value);

  /** \param s String, containing a opening and a closing paranthesis
       \return value between "(", ")" */
  string getParantheseVal(const string &s);

  /** \param s string containing tokens
      \param delim Token delimiter
      \return vector containing found tokens */
  vector <string> splitString(const string& s, unsigned char delim);

  /** \param val variable of specified type which will contain the numeric value
      \param s string containing a number
      \param f format specifier function (IO manipulator)
      \return true if successfull */
  template <class T>
  bool str2num(T &val, const std::string& s, std::ios_base& (*f)(std::ios_base&))
  {
    std::istringstream iss(s);
    return !(iss >> f >> val).fail();
  }

}

#endif
