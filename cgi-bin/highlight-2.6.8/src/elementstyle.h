/***************************************************************************
                          elementstyle.h  -  description
                             -------------------
    begin                : Son Nov 10 2002
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

#ifndef ELEMENTSTYLE_H
#define ELEMENTSTYLE_H

#include "stylecolour.h"

using namespace std;

namespace highlight {

/** \brief The class stores the basic text formatting properties.

* @author Andre Simon
*/

class ElementStyle {
  public:

    /** Constructor
        \param col Style colour
        \param b Bold flag
        \param i Italic flag
        \param u Underline flag */
    ElementStyle(StyleColour col, bool b, bool i, bool u);

   /** Constuctor
        \param elementStyleString String with formatting information (eg "00 aa ff bold") */
    ElementStyle(const string & elementStyleString);

    ElementStyle();

    ElementStyle(const ElementStyle &other){
       colour = other.getColour();
       bold = other.isBold();
       italic = other.isItalic();
       underline = other.isUnderline();
    }

    ElementStyle& operator=(const ElementStyle &other){
       colour = other.getColour();
       bold = other.isBold();
       italic = other.isItalic();
       underline = other.isUnderline();
       return *this;
    }

    ~ElementStyle();

   /** initialize object
       \param elementStyleString String which contains formatting attributes 
              (Format: "color attr" where 
               color can be HTML hex notation or a hex RGB tuple (ie "#2244ff" or "22 44 ff")
               attr can be a combination of "italic, "bold" and "underline")
   */
   void set(const string & elementStyleString);

   /** \return True if italic */
    bool isItalic() const;

   /** \return True if bold */
   bool isBold() const;

   /** \return True if underline */
   bool isUnderline() const;

   /** \param b set italic flag */
    void setItalic(bool b) {italic = b;}

   /** \param b set bold flag */
   void setBold(bool b) { bold = b; }

   /** \param b set underline flag */
   void setUnderline(bool b) {underline = b; }

   /** \return Element colour */
   StyleColour getColour() const;

   /** \param col colour of this element */
   void setColour(StyleColour& col) {colour = col;}

 private:
    StyleColour colour;
    bool bold, italic, underline;
  };

}

#endif
