/***************************************************************************
                          htmlcode.cpp  -  description
                             -------------------
    begin                : Wed Nov 28 2001
    copyright            : (C) 2001 by Andre Simon
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

#include "xhtmlgenerator.h"

using namespace std;

namespace highlight {

XHtmlGenerator::XHtmlGenerator ()
{
  fileSuffix=".xhtml";
  brTag="<br />";
  hrTag="<hr />";
  idAttr="id";
  cssClassName="hl";
}

string XHtmlGenerator::getHeaderStart(const string &title){
    ostringstream header;
    header << "<?xml version=\"1.0\"";
    if (encodingDefined()) {
      header << " encoding=\"" << encoding << "\"";
    }
    header << "?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""
           << "  \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
           << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
           << "<head>\n<title>" << title << "</title>\n";

    return header.str();
}


string XHtmlGenerator::getHeader()
{
  ostringstream os;
  os << getHeaderStart(docTitle);

  if (langInfo.getSyntaxHighlight()) {
    if (includeStyleDef) {
       os << "<style type=\"text/css\">\n";
       os << "<![CDATA[\n";
       os << getStyleDefinition();
       os << CodeGenerator::readUserStyleDef();
       os << "]]>\n";
       os << "</style>\n";
    } else {
       os << "<link rel=\"stylesheet\" type=\"text/css\" href=\""
          << getStyleOutputPath()
          << "\""
          << "/"
          << ">\n";
    }
  }
  os << "</head>\n<body class=\""<<cssClassName<<"\">\n<pre class=\""<<cssClassName<<"\">";
  if (showLineNumbers && orderedList) os << "<ol>";

  return os.str();
}

}
