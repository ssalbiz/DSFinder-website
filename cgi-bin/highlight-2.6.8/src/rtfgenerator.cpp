/***************************************************************************
                          rtfcode.cpp  -  description
                             -------------------
    begin                : Die Jul 9 2002
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

#include <sstream>

#include "charcodes.h"
#include "version.h"
#include "rtfgenerator.h"

using namespace std;

namespace highlight {

string RtfGenerator::getAttributes( const ElementStyle & col)
{
  stringstream s;
  s  << "\\red"<< col.getColour().getRed(RTF)
     << "\\green"<<col.getColour().getGreen(RTF)
     << "\\blue"<<col.getColour().getBlue(RTF)
     << ";";
  return s.str();
}

string  RtfGenerator::getOpenTag(int styleNumber,const ElementStyle & elem)
{
  ostringstream s;
  s << "{\\cf"<<(styleNumber+2)<<"{";
  if (elem.isBold()) s << "\\b ";
  if (elem.isItalic()) s << "\\i ";
  if (elem.isUnderline()) s << "\\ul ";
  return  s.str();
}

string  RtfGenerator::getCloseTag(const ElementStyle &elem)
{
  ostringstream s;
  if (elem.isBold()) s << "\\b0 ";
  if (elem.isItalic()) s << "\\i0 ";
  if (elem.isUnderline()) s << "\\ul0 ";
  s << "}}";
  return  s.str();
}

RtfGenerator::RtfGenerator()
 : CodeGenerator(RTF),
   pageSize("a4") // Default: DIN A4
{
  newLineTag = "}\\par\\pard\n\\cbpat1{";
  spacer = " ";

  // Page dimensions
  psMap["a3"] = PageSize(16837,23811);
  psMap["a4"] = PageSize(11905,16837);
  psMap["a5"] = PageSize(8390,11905);

  psMap["b4"] = PageSize(14173,20012);
  psMap["b5"] = PageSize(9977,14173);
  psMap["b6"] = PageSize(7086,9977);

  psMap["letter"] = PageSize(12240,15840);
  psMap["legal"] = PageSize(12240,20163);
}


bool RtfGenerator::initTheme(const string& themePath) {
   bool result = CodeGenerator::initTheme(themePath);

   styleTagOpen.push_back(getOpenTag(STANDARD, docStyle.getDefaultStyle()));
   styleTagOpen.push_back(getOpenTag(STRING, docStyle.getStringStyle()));
   styleTagOpen.push_back(getOpenTag(NUMBER, docStyle.getNumberStyle()));
   styleTagOpen.push_back(getOpenTag(SL_COMMENT, docStyle.getSingleLineCommentStyle()));
   styleTagOpen.push_back(getOpenTag(ML_COMMENT,docStyle.getCommentStyle()));
   styleTagOpen.push_back(getOpenTag(ESC_CHAR, docStyle.getEscapeCharStyle()));
   styleTagOpen.push_back(getOpenTag(DIRECTIVE, docStyle.getDirectiveStyle()));
   styleTagOpen.push_back(getOpenTag(DIRECTIVE_STRING, docStyle.getDirectiveStringStyle()));
   styleTagOpen.push_back(getOpenTag(LINENUMBER, docStyle.getLineStyle()));
   styleTagOpen.push_back(getOpenTag(SYMBOL, docStyle.getSymbolStyle()));

   styleTagClose.push_back(getCloseTag(docStyle.getDefaultStyle()));
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

RtfGenerator::~RtfGenerator()
{}

string RtfGenerator::getHeader()
{
  return string();
}

void RtfGenerator::printBody()
{

 *out << "{\\rtf1\\ansi\\uc0 \\deff1"
       << "{\\fonttbl{\\f1\\fmodern\\fprq1\\fcharset0 " ;
 *out << this->getBaseFont() ;
 *out << ";}}"
      << "{\\colortbl;";


   *out    << "\\red"    << (docStyle.getBgColour().getRed(RTF));
   *out    << "\\green"    << (docStyle.getBgColour().getGreen(RTF));
   *out    << "\\blue"    << (docStyle.getBgColour().getBlue(RTF));
   *out     << ";";

  *out << getAttributes(docStyle.getDefaultStyle());

  *out << getAttributes(docStyle.getStringStyle());
  *out << getAttributes(docStyle.getNumberStyle());
  *out << getAttributes(docStyle.getSingleLineCommentStyle());

  *out << getAttributes(docStyle.getCommentStyle());
  *out << getAttributes(docStyle.getEscapeCharStyle());
  *out << getAttributes(docStyle.getDirectiveStyle());

  *out << getAttributes(docStyle.getDirectiveStringStyle());
  *out << getAttributes(docStyle.getLineStyle());
  *out << getAttributes(docStyle.getSymbolStyle());

  /* For output formats which can refer to external styles it is more safe
     to use the colour theme's keyword class names, since the language
     definitions (which may change during a batch conversion) do not have to define
     all keyword classes, that are needed to highlight all input files correctly.
     It is ok for RTF to use the language definition's class names, because RTF
     does not refer to external styles.
     We cannot use the theme's class names, because KSIterator returns an
     alphabetically ordered list, which is not good because RTF is dependent
     on the order. We access the keyword style with an ID, which is calculated
     ignoring the alphabetic order.
  */
  vector<string>  keywordClasses = langInfo.getKeywordClasses();
  for (unsigned int i=0;i<keywordClasses.size();i++){
      *out << getAttributes(docStyle.getKeywordStyle(keywordClasses[i]));
  }

  *out << "}\n";

  *out  << "\\paperw"<< psMap[pageSize].width <<"\\paperh"<< psMap[pageSize].height
        << "\\margl1134\\margr1134\\margt1134\\margb1134\\sectd" // page margins
        << "\\plain\\f1\\fs" ;  // Font formatting
  int fontSize=0;
  StringTools::str2num<int>(fontSize, this->getBaseFontSize(), std::dec);
  *out << ((fontSize)? fontSize*2: 20);  // RTF needs double amount
  *out << "\n\\pard \\cbpat1{";

  processRootState();

  *out << "}}"<<endl;
}

string RtfGenerator::getFooter()
{
 return string();
}

string RtfGenerator::maskCharacter(unsigned char c)
{
  switch (c)
    {
    case '}' :
    case '{' :
    case '\\' :
      {
        string m;
        m="\\";
        return m+=c;
      }
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
        string m;
        m="{";
        m+=c;
        m+="}";
        return m;
      }
      break;

    case AUML_LC:
      return "\\'e4";
      break;
    case OUML_LC:
      return "\\'f6";
      break;
    case UUML_LC:
      return "\\'fc";
      break;
    case AUML_UC:
      return "\\'c4";
      break;
    case OUML_UC:
      return "\\'d6";
      break;
    case UUML_UC:
      return "\\'dc";
      break;

    case AACUTE_LC:
      return "\\'e1";
      break;
    case EACUTE_LC:
      return "\\'e9";
      break;
    case OACUTE_LC:
      return "\\'f3";
      break;
    case UACUTE_LC:
      return "\\'fa";
      break;

    case AGRAVE_LC:
      return "\\'e0";
      break;
    case EGRAVE_LC:
      return "\\'e8";
      break;
    case OGRAVE_LC:
      return "\\'f2";
      break;
    case UGRAVE_LC:
      return "\\'f9";
      break;

    case AACUTE_UC:
      return "\\'c1";
      break;
    case EACUTE_UC:
      return "\\'c9";
      break;
    case OACUTE_UC:
      return "\\'d3";
      break;
    case UACUTE_UC:
      return "\\'da";
      break;
    case AGRAVE_UC:
      return "\\'c0";
      break;
    case EGRAVE_UC:
      return "\\'c8";
      break;
    case OGRAVE_UC:
      return "\\'d2";
      break;
    case UGRAVE_UC:
      return "\\'d9";
      break;

    case SZLIG:
      return "\\'df";
      break;

    default :
      {
       string m;
       return m += c;
      }
    }
}

string RtfGenerator::getMatchingOpenTag(unsigned int styleID){
 return getOpenTag(KEYWORD+styleID,
        docStyle.getKeywordStyle(langInfo.getKeywordClasses()[styleID]));
}

string RtfGenerator::getMatchingCloseTag(unsigned int styleID){
  return getCloseTag(docStyle.getKeywordStyle(langInfo.getKeywordClasses()[styleID]));
}

void RtfGenerator::setPageSize(const string & ps){
    if (psMap.count(ps)) pageSize = ps;
}

}
