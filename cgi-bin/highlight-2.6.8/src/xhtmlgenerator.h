/***************************************************************************
                         xhtmlgenerator.h  -  description
                             -------------------
    begin                : Mo Jun 21 2004
    copyright            : (C) 2004 by Andre Simon
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


#ifndef XHTMLGENERATOR_H
#define XHTMLGENERATOR_H

#include "htmlgenerator.h"

namespace highlight {

/**
   \brief This class generates XHTML.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/


class XHtmlGenerator : public highlight::HtmlGenerator
  {
  public:

    XHtmlGenerator();

    ~XHtmlGenerator() {};

  private:

    /** prints document header
    */
    string getHeader();

    string getHeaderStart(const string &title);

  };

}

#endif
