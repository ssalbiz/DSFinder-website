/***************************************************************************
                          xmlcode.h  -  description
                             -------------------
    begin                : Do 20.01.2005
    copyright            : (C) 2005 by Andre Simon
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

#ifndef XMLGENERATOR_H
#define XMLGENERATOR_H

#include "codegenerator.h"

namespace highlight {

/**
   \brief This class generates XML.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/

class XmlGenerator : public highlight::CodeGenerator
  {
  public:

   XmlGenerator();
    ~XmlGenerator();

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

    string getAttributes(const string &, const ElementStyle &);

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    string getOpenTag(const string& );
    string getCloseTag(const string& );

    string getMatchingOpenTag(unsigned int styleID);
    string getMatchingCloseTag(unsigned int styleID);
  };

}

#endif
