/***************************************************************************
                          LatexCode.cpp  -  description
                             -------------------
    begin                : Mit Jul 24 2002
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

#include "latexgenerator.h"

namespace highlight {

LatexGenerator::LatexGenerator()
  : CodeGenerator(LATEX),
    replaceQuotes(false),
    disableBabelShortHand(false)
{
  styleTagOpen.push_back( "\\hlstd{");
  styleTagOpen.push_back( "\\hlstr{");
  styleTagOpen.push_back( "\\hlnum{");
  styleTagOpen.push_back( "\\hlslc{");
  styleTagOpen.push_back( "\\hlcom{");
  styleTagOpen.push_back( "\\hlesc{");
  styleTagOpen.push_back( "\\hldir{");
  styleTagOpen.push_back( "\\hldstr{");
  styleTagOpen.push_back( "\\hlline{");
  styleTagOpen.push_back( "\\hlsym{");

  for (int i=0;i<NUMBER_BUILTIN_STYLES; i++){
   styleTagClose.push_back( "}");
  }

  // avoid "Underfull \hbox (badness 10000)" warnings
  newLineTag = "\\\\\n";
  longLineTag = "\\hspace*{\\fill}" + newLineTag;

  spacer = "\\ ";

  maskWs=true;
  maskWsBegin = "\\hlstd{";
  maskWsEnd = "}";

  excludeWs=true;

  styleCommentOpen="%";
}

LatexGenerator::~LatexGenerator()
{}

string LatexGenerator::getAttributes(const string & elemName,
                                             const ElementStyle &elem)
{
  ostringstream s;
  s  << "\\newcommand{\\hl"
     << elemName
     << "}[1]{\\textcolor[rgb]{"
     << elem.getColour().getRed(LATEX) << ","
     << elem.getColour().getGreen(LATEX) << ","
     << elem.getColour().getBlue(LATEX)
     << "}{";

  if (elem.isBold())
    s << "\\bf{";
  if (elem.isItalic())
    s << "\\it{";

  s  <<"#1";

  if (elem.isBold())
    s << "}";
  if (elem.isItalic())
    s << "}";

  s  <<"}}\n";
  return s.str();
}

void LatexGenerator::printBody()
{
  *out << "\\noindent\n" ;
  if (! this->getBaseFont().empty() )
     *out << "\\" << this->getBaseFont() << "\n" ;
  else
     *out << "\\ttfamily\n";
  if (! this->getBaseFontSize().empty() )
     *out << "\\" << this->getBaseFontSize() << "\n" ;
  if (disableBabelShortHand)
     *out << "\\shorthandoff{\"}\n";

  processRootState();

  *out << "\\mbox{}\n"
       << "\\normalfont\n";
  if (! this->getBaseFontSize().empty())
     *out << "\\normalsize\n" ;
  if (disableBabelShortHand)
     *out << "\\shorthandon{\"}\n";
}

string LatexGenerator::getHeader()
{
  ostringstream os;

  os << "\\documentclass{article}\n"
     << "\\usepackage{color}\n"
     << "\\usepackage{alltt}\n";

  if (StringTools::lowerCase(encoding)=="utf-8"){
     os << "\\usepackage{ucs}\n\\usepackage[utf8x]{inputenc}\n";
  } else if (encodingDefined()){
     os << "\\usepackage[latin1]{inputenc}\n";
  }

  if (langInfo.getSyntaxHighlight()) {
    if (includeStyleDef) {
      os << "\n"<<getStyleDefinition();
      os << CodeGenerator::readUserStyleDef();
    }  else  {
      os << "\n\\input {"
         << getStyleOutputPath()
         << "}\n";
    }
  }

  os << "\n\\title{" << docTitle << "}\n"
     << "\\begin{document}\n"
     << "\\pagecolor{bgcolor}\n";
  return os.str();
}

string LatexGenerator::getFooter()
{
  ostringstream os;
  os << "\\end {document}\n"
     << "(* LaTeX generated by highlight "
     << HIGHLIGHT_VERSION
     << ", "
     << HIGHLIGHT_URL
     << " *)\n";
  return os.str();
}

string LatexGenerator::getNewLine(){
    return (showLineNumbers)? newLineTag:longLineTag;
}

string LatexGenerator::maskCharacter(unsigned char c)
{
  switch (c)
    {
    case '<' :
      return "$<$";
      break;
    case '>' :
      return "$>$";
      break;
    case '{':
    case '}':
    case '&':
    case '$':
    case '#':
    case '%':
      {
       string m;
       m  ="\\";
       m += c;
       return m;
      }
      break;
    case '\"':
      return (fragmentOutput && replaceQuotes)?"\\dq{}":"\"";
      break;
    case '_':
      return "\\textunderscore ";
      break;
    case '^':
      return "\\textasciicircum ";
      break;
    case '\\':
      return "$\\backslash$";
      break;
    case '~':
      return "$\\sim$";
      break;
    case '|':
      return "\\textbar ";
      break;
    // avoid latex compilation failure if [ or * follows a line break (\\)
    case '*':
    case '[':
    case ']':
    // avoid "merging" of consecutive '-' chars when included in bold font ( \bf )
    case '-':
      {
       string m;
       m= "{";
       m+= c;
       m+= "}";
       return m;
      }
      break;
    case ' ':
      return spacer;
      break;

    default :
      {
      string m;
      return m+=c;
     }
    }
}

string LatexGenerator::getMatchingOpenTag(unsigned int styleID){
  return "\\hl"+langInfo.getKeywordClasses()[styleID]+"{";
 }

string LatexGenerator::getMatchingCloseTag(unsigned int styleID){
  return "}";
}


string LatexGenerator::getStyleDefinition()
{
    if (styleDefinitionCache.empty()){
        ostringstream os;
        os << getAttributes("std", docStyle.getDefaultStyle());
        os << getAttributes("num", docStyle.getNumberStyle());
        os << getAttributes("esc", docStyle.getEscapeCharStyle());
        os << getAttributes("str", docStyle.getStringStyle());
        os << getAttributes("dstr", docStyle.getDirectiveStringStyle());
        os << getAttributes("slc", docStyle.getSingleLineCommentStyle());
        os << getAttributes("com", docStyle.getCommentStyle());
        os << getAttributes("dir", docStyle.getDirectiveStyle());
        os << getAttributes("sym", docStyle.getSymbolStyle());
        os << getAttributes("line", docStyle.getLineStyle());

        KeywordStyles styles = docStyle.getKeywordStyles();
        for (KSIterator it=styles.begin(); it!=styles.end(); it++){
            os << getAttributes(it->first, it->second);
        }
        os << "\\definecolor{bgcolor}{rgb}{"
                << docStyle.getBgColour().getRed(LATEX) << ","
                << docStyle.getBgColour().getGreen(LATEX) << ","
                << docStyle.getBgColour().getBlue(LATEX)
                << "}\n";
/*
        os << "\\oddsidemargin -3mm\n\\textwidth 165,2truemm\n"
                << "\\topmargin 0truept\n\\headheight 0truept\n"
                << "\\headsep 0truept\n\\textheight 230truemm\n";
*/
        styleDefinitionCache=os.str();
    }
    return styleDefinitionCache;
}


}
