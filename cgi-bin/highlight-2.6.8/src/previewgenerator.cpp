/***************************************************************************
                   previewgenerator.cpp  -  description
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

#include <sstream>

#include "version.h"
#include "previewgenerator.h"

using namespace std;

namespace highlight {

PreviewGenerator::PreviewGenerator ():
  CodeGenerator(HTML)
{
  newLineTag = "\n";
  spacer = " ";
  styleCommentOpen="/*";
  styleCommentClose="*/";
}

bool PreviewGenerator::initTheme(const string& themePath) {
   bool result = CodeGenerator::initTheme(themePath);

   styleTagOpen.push_back("");
   styleTagOpen.push_back(getOpenTag(docStyle.getStringStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getNumberStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getSingleLineCommentStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getCommentStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getEscapeCharStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getDirectiveStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getDirectiveStringStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getLineStyle()));
   styleTagOpen.push_back(getOpenTag(docStyle.getSymbolStyle()));

   styleTagClose.push_back("");
   styleTagClose.push_back(getCloseTag(docStyle.getStringStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getNumberStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getSingleLineCommentStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getCommentStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getEscapeCharStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getDirectiveStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getDirectiveStringStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getLineStyle()));
   styleTagClose.push_back(getCloseTag(docStyle.getSymbolStyle()));

   return result;
 }

string  PreviewGenerator::getOpenTag(const ElementStyle & elem) {
    ostringstream s;
  s << "<font color=\""<<"#"
    << elem.getColour().getRed(HTML)
    << elem.getColour().getGreen(HTML)
    << elem.getColour().getBlue(HTML)
    << "\">";
  if (elem.isBold()) s << "<b>";
  if (elem.isItalic()) s << "<i>";
  if (elem.isUnderline()) s << "<u>";
  return  s.str();
}

string  PreviewGenerator::getCloseTag(const ElementStyle & elem) {
  ostringstream s;
  if (elem.isUnderline()) s << "</u>";
  if (elem.isItalic()) s << "</i>";
  if (elem.isBold()) s << "</b>";
  s << "</font>";
  return  s.str();
}

string PreviewGenerator::getHeader()
{
  ostringstream os;
  os << "<html><body bgcolor=\"#"
     << (docStyle.getBgColour().getRed(HTML))
     << (docStyle.getBgColour().getGreen(HTML))
     << (docStyle.getBgColour().getBlue(HTML))
     << "\" text=\"#"
     << (docStyle.getDefaultStyle().getColour().getRed(HTML))
     << (docStyle.getDefaultStyle().getColour().getGreen(HTML))
     << (docStyle.getDefaultStyle().getColour().getBlue(HTML))
     << "\"><pre><font face=\"Times\">";
  return os.str();
}

string PreviewGenerator::getFooter()
{
  return "</font></pre></body></html>";
}

void PreviewGenerator::printBody()
{
  processRootState();
}

string PreviewGenerator::maskCharacter(unsigned char c)
{
  switch (c) {
    case '<' :
      return "&lt;";
      break;
    case '>' :
      return "&gt;";
      break;
    case '&' :
      return "&amp;";
      break;
    case '\"' :
      return "&quot;";
      break;

    //case '@' :
      //return "&#64;";
      //break;

    default :
      string m;
      return m += c;
    }
}

string PreviewGenerator::getMatchingOpenTag(unsigned int styleID){
  return getOpenTag(docStyle.getKeywordStyle(langInfo.getKeywordClasses()[styleID]));
 }

string PreviewGenerator::getMatchingCloseTag(unsigned int styleID){
  return getCloseTag(docStyle.getKeywordStyle(langInfo.getKeywordClasses()[styleID]));
}

}
