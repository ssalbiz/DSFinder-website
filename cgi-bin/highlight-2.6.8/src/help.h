/***************************************************************************
                          help.h  -  description
                             -------------------
    begin                : Die Apr 23 2002
    copyright            : (C) 2002-2007 by Andre Simon
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

#ifndef HELP_H
#define HELP_H

#include <string>

///Contains methods for printing help messages

namespace Help
  {
    /** print help message to stdout
        @helpFilePath path to help message file
     */
     void printHelp(const std::string &helpFilePath);
  }

#endif
