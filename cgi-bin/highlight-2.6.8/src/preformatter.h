/***************************************************************************
                        PreFormatter.cpp  -  description
                             -------------------
    begin                : Mo Jan 03 2005
    copyright            : (C) 2005-2007 by Andre Simon
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

#ifndef PreFormatter_H
#define PreFormatter_H

#include <string>
#include <set>

namespace highlight {

/** \brief Class which provides intelligent line wrapping.
* @author Andre Simon
*/

class PreFormatter{
public:

    PreFormatter();

    ~PreFormatter();

    /**
     Sets new line to be wrapped
     \param newline New line
    */
    void setWrap(bool wrap) {wrapLines = wrap;}

    /**
     Sets new line to be wrapped
     \param newline New line
    */
    void setReplaceTabs(bool replTabs) {replaceTabs = replTabs;}

    /**
     \return True if current line can be wrapped again
    */
    bool hasMoreLines();

    /**
     Sets new line to be wrapped
     \param newline New line
    */
    void setLine(const std::string & newline);

    /**
     The method will indent function calls and statements
     \return Next line
    */
    std::string getNextLine();

    /**
     \return True if lines following open braces should be indented
    */
    bool indentCode();

    /**
      \param maxlength max. length of output lines
    */
    void setWrapLineLength(unsigned int maxlength);

    /**
      \param indentAfterOpenBraces set true if lines should be indented after braces
    */
    void setWrapIndentBraces(bool indentAfterOpenBraces=true);

    /**
      \param num number of spaces which replace a tab
    */
    void setNumberSpaces(unsigned int num);

    /**
      \return true if preformatting is enabled
    */
    bool isEnabled(){
      return wrapLines || replaceTabs;
    }

    /**
      reset preformatting data for usage in a loop
    */
    void reset () {
       lineNumber=0;
       wrappedLines.clear();
    }

    /**
     \param lineNumber line number
      \return true if input line linenumber was wrapped
    */
    bool isWrappedLine(int lineNumber){
	return wrappedLines.count(lineNumber);
    }

private:

    unsigned int maxLineLength;

    std::string line, wsPrefix;
    unsigned int index;
    unsigned int numberSpaces;
    unsigned int lineNumber;
    size_t wsPrefixLength;
    bool hasMore, indentAfterOpenBraces;
    bool redefineWsPrefix;
    bool wrapLines, replaceTabs;

    std::set<int> wrappedLines;

    static const std::string LB_CHARS;
    static const std::string WS_CHARS;
    static const std::string INDENT_MARKERS;

};

}

#endif
