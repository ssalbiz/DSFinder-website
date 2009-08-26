/*
 * Copyright (c) 1998,1999,2000,2001,2002 Tal Davidson. All rights reserved.
 *
 * compiler_defines.h   (1 January 1999)
 * by Tal Davidson (davidsont@bigfoot.com)
 * This file is a part of "Artistic Style" - an indentater and reformatter
 * of C, C++, C# and Java source files.
 *
 * The "Artistic Style" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published 
 * by the Free Software Foundation; either version 2 of the License, 
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */


#ifndef ASSTREAMITERATOR_H
#define ASSTREAMITERATOR_H

#include "ASSourceIterator.h"

using namespace std;

namespace astyle
  {
  class ASStreamIterator :
        public ASSourceIterator
    {
    public:
      ASStreamIterator(istream *in);
      virtual ~ASStreamIterator();
      bool hasMoreLines() const;
      string nextLine();

    private:
      istream * inStream;
      char buffer[2048];
    };

}

#endif
