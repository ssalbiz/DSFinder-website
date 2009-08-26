/***************************************************************************
                         ansicode.h  -  description
                             -------------------
    begin                : Jul 5 2004
    copyright            : (C) 2004-2007 by Andre Simon
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

#ifndef ANSIGENERATOR_H
#define ANSIGENERATOR_H

#include <string>

#include "codegenerator.h"
#include "charcodes.h"
#include "version.h"

namespace highlight {

/**
   \brief This class generates ANSI escape sequences.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/

class AnsiGenerator : public highlight::CodeGenerator
  {
  public:
    AnsiGenerator();
    ~AnsiGenerator();

   /** prints document header
    */
    string getHeader();

    /** Prints document footer*/
    string getFooter();

    /** Prints document body*/
    void printBody();

  private:

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );


    /** gibt ANSI-"Tags" zurueck (Farbindex+bold+kursiv)*/
    string getOpenTag(const string&font,
                      const string&fgCol, const string&bgCol="");

    /** @param styleID current style ID
        @return matching sequence to begin a new element formatting*/
    string getMatchingOpenTag(unsigned int styleID);

    /** @param styleID current style ID
        @return matching  sequence to stop element formatting*/
    string getMatchingCloseTag(unsigned int styleID);
  };

}
#endif
