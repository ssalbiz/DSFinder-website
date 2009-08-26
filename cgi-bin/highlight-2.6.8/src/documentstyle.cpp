/***************************************************************************
                          documentstyle.cpp  -  description
                             -------------------
    begin                : Son Nov 10 2002
    copyright            : (C) 2002 by Andre Simon
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

#include "documentstyle.h"
#include "stringtools.h"

namespace highlight {

DocumentStyle::DocumentStyle(const string &styleDefinitionFile)
{
  fileFound=load(styleDefinitionFile);
}
DocumentStyle::DocumentStyle():fileFound(false)
{}

bool DocumentStyle::load(const string &styleDefinitionPath)
{
  ConfigurationReader styleConfig(styleDefinitionPath);
  fileFound = false;
  if (styleConfig.found()){
    fontsize = styleConfig.getParameter("fontsize");
    bgColour.setRGB(styleConfig.getParameter("bgcolour"));
    defaultElem.set(styleConfig.getParameter("defaultcolour"));
    comment.set(styleConfig.getParameter("comment"));
    directive.set(styleConfig.getParameter("directive"));
    str.set(styleConfig.getParameter("string"));
    escapeChar.set(styleConfig.getParameter("escapechar"));
    number.set(styleConfig.getParameter("number"));
    dstr.set(styleConfig.getParameter("string-directive"));
    line.set(styleConfig.getParameter("line"));
    symbol.set(styleConfig.getParameter("symbol"));
    markLineColour.setRGB(styleConfig.getParameter("mark-line"));

    string tmpstr;
    // TODO: Remove this check as soon as all themes have a sl-comment attribute
    tmpstr=styleConfig.getParameter("sl-comment");
    if (tmpstr.empty()) {
      tmpstr=styleConfig.getParameter("comment");
    }
    slcomment.set(tmpstr);

    string paramVal;
    vector<string> paramNames=styleConfig.getParameterNames();

    //collect keyword groups, save corresponding style definition
    for (unsigned int i=0;i<paramNames.size();i++){
      paramVal=paramNames[i];
      if (paramVal.find("kw-group") != string::npos || paramVal.find("kw_group") != string::npos){
         keywordStyles.insert(make_pair(StringTools::getParantheseVal(paramVal),
                             ElementStyle(styleConfig.getParameter(paramVal))));
      }
    }
    fileFound = true;
  }
  return fileFound;
}

DocumentStyle::~DocumentStyle()
{
  /*for(KSIterator iter = keywordStyles.begin(); iter != keywordStyles.end(); iter++){
    delete (*iter).second;  //remove ElementStyle*
  }*/
}

string& DocumentStyle::getFontSize()
  {
    return fontsize;
  }
StyleColour& DocumentStyle::getBgColour()
  {
    return bgColour;
  }
StyleColour& DocumentStyle::getMarkLineColour()
  {
    return markLineColour;
  }
ElementStyle& DocumentStyle::getDefaultStyle()
  {
    return defaultElem;
  }
ElementStyle& DocumentStyle::getCommentStyle()
  {
    return comment;
  }
ElementStyle& DocumentStyle::getSingleLineCommentStyle()
  {
    return slcomment;
  }
ElementStyle& DocumentStyle::getStringStyle()
  {
    return str;
  }
ElementStyle& DocumentStyle::getDirectiveStringStyle()
  {
    return dstr;
  }
ElementStyle& DocumentStyle::getEscapeCharStyle()
  {
    return escapeChar;
  }
ElementStyle& DocumentStyle::getNumberStyle()
  {
    return number;
  }
ElementStyle& DocumentStyle::getDirectiveStyle()
  {
    return directive;
  }
ElementStyle& DocumentStyle::getLineStyle()
  {
    return line;
  }
ElementStyle& DocumentStyle::getSymbolStyle()
  {
    return symbol;
  }
bool DocumentStyle::found () const
  {
    return fileFound;
  }
ElementStyle& DocumentStyle::getKeywordStyle(const string &className){
  if (!keywordStyles.count(className)) return defaultElem;
  return keywordStyles[className];
}

vector <string> DocumentStyle::getClassNames(){
  vector <string> kwClassNames;
  for(KSIterator iter = keywordStyles.begin(); iter != keywordStyles.end(); iter++){
    kwClassNames.push_back( (*iter).first);
  }
  return kwClassNames;
}

KeywordStyles& DocumentStyle::getKeywordStyles(){
  return keywordStyles;
}

}
