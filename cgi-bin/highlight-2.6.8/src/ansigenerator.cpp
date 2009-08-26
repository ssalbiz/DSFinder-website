/***************************************************************************
                    ansigenerator.cpp  -  description
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

#include <sstream>

#include "ansigenerator.h"

using namespace std;

namespace highlight {

string  AnsiGenerator::getOpenTag(const string&font,
                                  const string&fgCol, const string&bgCol) {
    ostringstream s;
    s  << "\033["<<font;
    if (!fgCol.empty())
        s<<";"<<fgCol;
    if (!bgCol.empty())
        s<<";"<<bgCol;
    s << "m";
    return  s.str();
}


AnsiGenerator::AnsiGenerator() : CodeGenerator(ANSI) {
    styleTagOpen.push_back("");
    styleTagOpen.push_back(getOpenTag("00", "31")); //str
    styleTagOpen.push_back(getOpenTag("00", "34"));//number
    styleTagOpen.push_back(getOpenTag("00", "34"));//sl comment
    styleTagOpen.push_back(getOpenTag("00", "34"));//ml comment
    styleTagOpen.push_back(getOpenTag("00", "35"));//escapeChar
    styleTagOpen.push_back(getOpenTag("00", "35"));//directive
    styleTagOpen.push_back(getOpenTag("00", "31"));//directive string
    styleTagOpen.push_back(getOpenTag("00", "30"));//linenum
    styleTagOpen.push_back(getOpenTag("00", "00"));//symbol

    styleTagClose.push_back("");
    for (int i=1;i<NUMBER_BUILTIN_STYLES; i++) {
        styleTagClose.push_back("\033[m");
    }
    newLineTag = "\n";
    spacer = " ";
}

AnsiGenerator::~AnsiGenerator() {}

string AnsiGenerator::getHeader() {
    return string();
}

void AnsiGenerator::printBody() {
    processRootState();
}

string AnsiGenerator::getFooter() {
    return string();
}

string AnsiGenerator::maskCharacter(unsigned char c) {
    string m;
    m+=c;
    return m;
}

string AnsiGenerator::getMatchingOpenTag(unsigned int styleID) {
    return (styleID)?getOpenTag("00", "32", ""):getOpenTag("00", "33");
}

string AnsiGenerator::getMatchingCloseTag(unsigned int styleID) {
    return "\033[m";
}

}
