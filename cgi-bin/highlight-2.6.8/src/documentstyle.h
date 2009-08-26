/***************************************************************************
                          documentstyle.h  -  description
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

#ifndef DOCUMENTSTYLE_H
#define DOCUMENTSTYLE_H

#include <string>
#include "configurationreader.h"
#include "elementstyle.h"
#include "stylecolour.h"

using namespace std;

namespace highlight {

/** maps keyword class names and the corresponding formatting information*/
typedef map <string, ElementStyle> KeywordStyles;

/** iterator for keyword styles*/
typedef KeywordStyles::iterator KSIterator;

/** \brief Contains information about document formatting properties.
 * @author Andre Simon
 */

class DocumentStyle
  {
  private:
    ElementStyle comment, slcomment, str, dstr,
                 escapeChar, number, directive, line, symbol;
    ElementStyle defaultElem;
    StyleColour  bgColour;
    StyleColour  markLineColour;

    string fontsize;
    bool fileFound;

    KeywordStyles keywordStyles;

  public:
    /** Constructor
        \param styleDefinitionPath Style definition path */
    DocumentStyle(const string & styleDefinitionPath);
    DocumentStyle();
    ~DocumentStyle();

  /** load style definition
        \param styleDefinitionFile Style definition path
        \return True if successfull */
    bool load(const string & styleDefinitionFile);

    /** \return class names defined in the theme file */
    vector <string> getClassNames();

    /** \return keyword styles */
    KeywordStyles& getKeywordStyles();

    /** \return Font size */
    string &getFontSize();

    /** \return Background colour*/
    StyleColour& getBgColour();

    /** \return Mark line colour*/
    StyleColour& getMarkLineColour();

    /** \return Style of default (unrecognized) strings */
    ElementStyle & getDefaultStyle() ;

    /** \return Comment style*/
    ElementStyle & getCommentStyle() ;

    /** \return Single line comment style*/
    ElementStyle& getSingleLineCommentStyle() ;

    /** \return Keyword style*/
    ElementStyle & getKeywordStyle() ;

    /** \return String style*/
    ElementStyle & getStringStyle() ;

    /** \return Directive line string style*/
    ElementStyle & getDirectiveStringStyle() ;

    /** \return Escape character style*/
    ElementStyle & getEscapeCharStyle() ;

    /** \return Number style*/
    ElementStyle & getNumberStyle() ;

    /** \return Directive style*/
    ElementStyle & getDirectiveStyle() ;

    /** \return Type style*/
    ElementStyle & getTypeStyle() ;

    /** \return Line number style*/
    ElementStyle & getLineStyle() ;

    /** \return Bracket style*/
    ElementStyle & getSymbolStyle() ;

    /** \param className Name of keyword class (eg kwa, kwb, .., kwd)
        \return keyword style of the given className
    */
    ElementStyle & getKeywordStyle(const string &className);

    /** \return True if language definition was found */
    bool found() const ;
  };

}

#endif
