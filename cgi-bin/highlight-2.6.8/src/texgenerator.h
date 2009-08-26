/***************************************************************************
                          texcode.h  -  description
                             -------------------
    begin                : Mit Jul 24 2002
    copyright            : (C) 2002 by Andr�Simon
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

#ifndef TEXGENERATOR_H
#define TEXGENERATOR_H

#include <string>

#include "codegenerator.h"


namespace highlight {

/**
   \brief This class generates TeX.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/

class TexGenerator : public highlight::CodeGenerator
  {
  public:

    TexGenerator();
    ~TexGenerator();

  private:

   /** prints document header
    */
    string getHeader();

    /** Prints document footer*/
    string getFooter();

    /** Prints document body*/
    void printBody();

    string styleDefinitionCache;

    string getStyleDefinition();

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    /**\return text formatting attributes in RTF format */
    string getAttributes(const string & elemName, const ElementStyle & elem);

    /** @param styleID current style ID
        @return matching sequence to begin a new element formatting*/
    string getMatchingOpenTag(unsigned int styleID);

    /** @param styleID current style ID
        @return matching  sequence to stop element formatting*/
    string getMatchingCloseTag(unsigned int styleID);

  };

}

#endif
