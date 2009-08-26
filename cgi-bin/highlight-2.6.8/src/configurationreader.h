/***************************************************************************
                          configurationreader.h  -  description
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

#ifndef CONFIGURATIONREADER_H
#define CONFIGURATIONREADER_H

#include <string>
#include <map>
#include <vector>

using namespace std;

/** Maps parameter keys to values*/
typedef map<string, string> ParameterMap;


/** \brief Class to handle ASCII config files

  Configuration file format:<br>
  $ParamName=ParamValue<br>
  ParamValue may be splittet over multiple lines<br>
  ParamName is not case sensitive<br>
  Comments start with # as the first character of a line

 **/

class ConfigurationReader
  {
  public:
    /** Constructor
        \param configuration_path Path to configuration file
    */
    ConfigurationReader(const string & configuration_path);
    ~ConfigurationReader();

    /** \param paramName Name of parameter
        \return Value of parameter */
    string &getParameter(const string & paramName);

    /** \param paramName Name of parameter
        \return Value of parameter */
    const char* getCParameter(const string & paramName);

    /** \return True if config file exists */
    bool found();

    /** \return List of parameter names */
    vector<string> &getParameterNames();

  private:
    ParameterMap parameterMap;
    bool fileFound;
    vector<string> parameterNames;
  };

#endif
