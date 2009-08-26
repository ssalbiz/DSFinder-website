/***************************************************************************
                     previewgenerator.h  -  description
                             -------------------
    begin                : 06.01.2007
    copyright            : (C) 2007 by Andre Simon
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


#ifndef PREVIEWGENERATOR_H
#define PREVIEWGENERATOR_H


#include <string>

#include "codegenerator.h"
#include "elementstyle.h"

namespace highlight {

/**
   \brief This class generates "old style" HTML 3 for use in wxHTML
         (Preview function).

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/

class PreviewGenerator  : public highlight::CodeGenerator
  {
  public:

    PreviewGenerator();

    /** Destructor*/
    ~PreviewGenerator() {};

    bool initTheme(const string& themePath);

  private:


    /** Print document header
    */
    string getHeader();

    /** Print document body*/
    void printBody();

    /** Print document footer*/
    string getFooter();

    /**  \param styleName Style name
         \return Opening tag of the given style
    */
    string getOpenTag(const ElementStyle &);

    string getCloseTag(const ElementStyle &);

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    /**  \param styleID Style ID
         \return Opening tag of the given style
    */
    string getMatchingOpenTag(unsigned int styleID);

    /**  \param styleID Style ID
         \return Closing tag of the given style
    */
    string getMatchingCloseTag(unsigned int styleID);


  };

}

#endif
