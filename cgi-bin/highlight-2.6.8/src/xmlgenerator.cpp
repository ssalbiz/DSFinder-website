/***************************************************************************
                          xmlcode.cpp  -  description
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

#include <string>
#include <sstream>

#include "version.h"
#include "xmlgenerator.h"

using namespace std;
namespace highlight {

XmlGenerator::XmlGenerator()
: CodeGenerator(XML)
{
    styleTagOpen.push_back(getOpenTag("def"));
    styleTagOpen.push_back(getOpenTag("str"));
    styleTagOpen.push_back(getOpenTag("num"));
    styleTagOpen.push_back(getOpenTag("slc"));
    styleTagOpen.push_back(getOpenTag("com"));
    styleTagOpen.push_back(getOpenTag("esc"));
    styleTagOpen.push_back(getOpenTag("dir"));
    styleTagOpen.push_back(getOpenTag("dstr"));
    styleTagOpen.push_back(getOpenTag("line"));
    styleTagOpen.push_back(getOpenTag("sym"));

    styleTagClose.push_back(getCloseTag("def"));
    styleTagClose.push_back(getCloseTag("str"));
    styleTagClose.push_back(getCloseTag("num"));
    styleTagClose.push_back(getCloseTag("slc"));
    styleTagClose.push_back(getCloseTag("com"));
    styleTagClose.push_back(getCloseTag("esc"));
    styleTagClose.push_back(getCloseTag("dir"));
    styleTagClose.push_back(getCloseTag("dstr"));
    styleTagClose.push_back(getCloseTag("line"));
    styleTagClose.push_back(getCloseTag("sym"));

    spacer = " ";
    newLineTag = "<br />\n";
}

XmlGenerator::~XmlGenerator() {}

string XmlGenerator::getStyleDefinition()
{
    if (styleDefinitionCache.empty()) {
        ostringstream os;
        os << "\n<style>\n"
           << "\t<bgcolor value=\""
           << (docStyle.getBgColour().getRed(XML))
           << (docStyle.getBgColour().getGreen(XML))
           << (docStyle.getBgColour().getBlue(XML))
           << "\" />\n";
        os << "\t<font size=\"" << getBaseFontSize();
        os << "\" family=\"" << getBaseFont() << "\" />\n";
        os << getAttributes("def", docStyle.getDefaultStyle())
           << getAttributes("num", docStyle.getNumberStyle())
           << getAttributes("esc", docStyle.getEscapeCharStyle())
           << getAttributes("str", docStyle.getStringStyle())
           << getAttributes("dstr", docStyle.getDirectiveStringStyle())
           << getAttributes("slc", docStyle.getSingleLineCommentStyle())
           << getAttributes("com", docStyle.getCommentStyle())
           << getAttributes("dir", docStyle.getDirectiveStyle())
           << getAttributes("sym", docStyle.getSymbolStyle())
           << getAttributes("line", docStyle.getLineStyle());

        KeywordStyles styles = docStyle.getKeywordStyles();
        for (KSIterator it=styles.begin(); it!=styles.end(); it++){
            os << getAttributes(it->first, it->second);
        }
        os << "</style>\n";
        styleDefinitionCache=os.str();
    }
    return styleDefinitionCache;
}


string XmlGenerator::getAttributes(const string & elemName,
                                           const ElementStyle & elem)
{
    ostringstream s;
    s  << "\t<class name=\""
       << elemName
       <<"\" color=\""
       << (elem.getColour().getRed(XML))
       << (elem.getColour().getGreen(XML))
       << (elem.getColour().getBlue(XML))
       << "\" bold=\""
       << ( elem.isBold() ? "true" :"false" )
       << "\" italic=\""
       << ( elem.isItalic() ? "true" :"false" )
       << "\" underline=\""
       << ( elem.isUnderline() ? "true" :"false" )
       << "\" />\n" ;
    return  s.str();
}

string  XmlGenerator::getOpenTag(const string& styleName ){
    return "<"+styleName+">";
}

string  XmlGenerator::getCloseTag(const string& styleName ){
    return "</"+styleName+">";
}

string XmlGenerator::getHeader()
{
    ostringstream header;
    header << "<?xml version=\"1.0\"";
    if (encodingDefined()) {
        header << " encoding=\"" << encoding << "\"";
    }
    header << "?>\n<document>";
    header << "\n<title>" << docTitle << "</title>";
    header << getStyleDefinition();
    return header.str();
}

void XmlGenerator::printBody()
{
  *out << "<source>\n";
  processRootState();
  *out << "</source>\n";
}


string XmlGenerator::getFooter()
{
  ostringstream os;
  os <<"</document>\n";
  os<< "<!-- XML generated by Highlight "
     << HIGHLIGHT_VERSION
     << ", "
     << HIGHLIGHT_URL
     <<" -->\n";
  return os.str();
}

string XmlGenerator::maskCharacter(unsigned  char c)
{
  switch (c)
    {
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
    default:
      string m;
      m += c;
      return m;
    }
}

string XmlGenerator::getMatchingOpenTag(unsigned int styleID){
    return getOpenTag(langInfo.getKeywordClasses()[styleID]);
}

string XmlGenerator::getMatchingCloseTag(unsigned int styleID){
    return getCloseTag(langInfo.getKeywordClasses()[styleID]);
}

}
