/***************************************************************************
                          elementstyle.cpp  -  description
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
#include <sstream>
#include "elementstyle.h"

namespace highlight {

ElementStyle::ElementStyle(StyleColour col, bool b, bool i, bool u)
    : colour(col) , bold(b), italic(i), underline(u)
{}

ElementStyle:: ElementStyle(const string & elementStyleString)
  : bold(false), italic(false), underline(false)
{
  set(elementStyleString);
}

ElementStyle::ElementStyle()
    : bold(false), italic(false), underline(false)
{}

void ElementStyle::set(const string & elementStyleString){

  istringstream valueStream(elementStyleString);
  string r, g, b, attr;

  char c='\0';
  valueStream >> c;

  if (c=='#') {
    string htmlNotation;
    valueStream >> htmlNotation;
    if (htmlNotation.size() < 6) return;
    r = htmlNotation.substr(0, 2);
    g = htmlNotation.substr(2, 2);
    b = htmlNotation.substr(4, 2);
  } else {
    valueStream.putback(c);
    valueStream >> r;
    valueStream >> g;
    valueStream >> b;
  }

  colour.setRed(r);
  colour.setGreen(g);
  colour.setBlue(b);
  while ( valueStream >> attr)
    {
      if (attr=="italic")
        {
          italic = true;
        }
      else if (attr=="bold")
        {
          bold = true;
        }
      else if (attr=="underline")
        {
          underline = true;
        }
    }
}

ElementStyle::~ElementStyle()
{}

bool ElementStyle::isItalic() const
{
  return italic;
}
bool ElementStyle::isBold() const
{
  return bold;
}
bool ElementStyle::isUnderline() const
{
  return underline;
}
StyleColour ElementStyle::getColour() const
{
  return colour;
}

}
