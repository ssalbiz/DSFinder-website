/***************************************************************************
                          help.cpp  -  description
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

#include <iostream>
#include <fstream>

#include "help.h"

namespace Help
  {

 void printHelp(const std::string & helpFilePath)
 {
    std::ifstream helpFile(helpFilePath.c_str());
    std::string line;
    if (helpFile){
       while (getline(helpFile, line))
        std::cout << line << "\n";
      helpFile.close();
    }
    else {
     std::cerr <<"highlight: Could not read "<< helpFilePath << "\n";
    }
 }

}
