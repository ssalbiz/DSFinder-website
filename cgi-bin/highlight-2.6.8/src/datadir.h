/***************************************************************************
                          datadir.h  -  description
                             -------------------
    begin                : Sam March 1 2003
    copyright            : (C) 2003 by Andre Simon
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

#ifndef DATADIR_H
#define DATADIR_H

using namespace std;

 /** \brief Manages access to installation directories.

     Apart from the standard installation directory, one can define additional
     search paths. If the additonal paths do not exist, the default paths are
     returned.
  **/

class DataDir
  {
     string dataDir;
     string additionalDataDir;
     string additionalConfDir;
     bool fileExists(const string&);

  public:

     DataDir();

     /** search for a valid installation directory
         \param  userDefinedDir Directory defined by user
         \return True if directory was found */
     bool searchDataDir(const string &userDefinedDir);

     /** add another installation directory, which is added to search path
         \param  dir Directory defined by user */
     void setAdditionalDataDir(const string& dir);

     /** add another installation directory, which is added to search path
         \param  dir Directory defined by user */
     void setAdditionalConfDir(const string& dir);

     /** \return data Directory defined by user */
    const string & getAdditionalDataDir();

     /** \return data Directory defined by user */
    const string & getAdditionalConfDir();

    /** \return Data installation directory */
     const string & getDir() ;

    /**  \param file filename
         \param forceDefault set tue if additional directory should be ignored
         \return Location of languafe definitions */
     const string getLangPath(const string & file="", bool forceDefault=false) ;

    /** \param file filename
        \param forceDefault set tue if additional directory should be ignored
        \return Location of themes */
     const string getThemePath(const string & file="", bool forceDefault=false) ;

    /** \param file filename
        \param forceDefault set tue if additional directory should be ignored
        \return Location of indentation schemes */
    const string getIndentSchemesPath(const string & file="", bool forceDefault=false);

    /** \return User defined location of indentation schemes */
    const string getAdditionalIndentSchemesDir();

     /** \return User defined location of languafe definitions */
     const string getAdditionalLangDefDir() ;

    /** \return User defined location of themes */
     const string getAdditionalThemeDir() ;

    /** \return Location of help files */
     const string getHelpMsgDir() ;

    /** \param forceDefault set tue if additional directory should be ignored 
        \return Location of configuration files */
     const string getConfDir(bool forceDefault=false) ;

      /** \return Location of GUI menu translation files */
     const string getI18nDir();

     /** \return Location of GUI file extension filter files */
     const string getExtDir();

     /** \return Location of documentation (README) files (GUI) */
     const string getDocDir();

  };

#endif
