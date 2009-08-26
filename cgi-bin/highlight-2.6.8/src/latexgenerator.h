/***************************************************************************
                          latexgenerator.h  -  description
                             -------------------
    begin                : Mit Jul 24 2002
    copyright            : (C) 2002 by Andr� Simon
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

#ifndef LATEXGENERATOR_H
#define LATEXGENERATOR_H

#include <string>
#include <iostream>
#include <sstream>

#include "codegenerator.h"
#include "version.h"
#include "charcodes.h"


namespace highlight {

/**
   \brief This class generates LaTeX.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/

class LatexGenerator : public highlight::CodeGenerator
  {
  public:
    LatexGenerator();
    ~LatexGenerator();

    /** set replace quotes flag
       \param  b flag
    */
    void setReplaceQuotes(bool b){ replaceQuotes = b; }

    /** set disable babal shorthand flag
       \param  b flag
    */
    void setDisableShorthands(bool b){ disableBabelShortHand = b; }

  private:


   /** prints document header
    */
    string getHeader();

    /** Prints document footer*/
    string getFooter();

    /** Prints document body*/
    void printBody();

     string styleDefinitionCache;
     string longLineTag;

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    /**\return text formatting attributes in LaTeX format */
    string getAttributes(const string & elemName,
                                 const ElementStyle & elem);

    /** test if double quotes should be replaced by \dq{} */
    bool replaceQuotes;

    /** test if Babel shorthand for " should be disabled */
    bool disableBabelShortHand;

    string getNewLine();

    string getStyleDefinition();

    string getMatchingOpenTag(unsigned int styleID);
    string getMatchingCloseTag(unsigned int styleID);
  };

}

#endif
