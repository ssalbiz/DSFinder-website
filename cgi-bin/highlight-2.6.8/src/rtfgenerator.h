/***************************************************************************
                          rtfcode.h  -  description
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

#ifndef RTFGENERATOR_H
#define RTFGENERATOR_H

#include <string>

#include "codegenerator.h"


namespace highlight {

/**
   \brief This class generates RTF.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/


struct PageSize {
  /// RTF page width
  int width;
  /// RTF page height
  int height;
  PageSize(){
  }
  /** Constructor to define page dimensions
      @param w width
      @param h height*/
  PageSize(int w, int h){
    width=w;
    height = h;
  }
};

/** mapping of page size names and dimensions */
typedef  map<string, struct PageSize> PagesizeMap;

class RtfGenerator : public highlight::CodeGenerator
  {
  public:

    RtfGenerator();

    ~RtfGenerator();

   /** Override base class function to initialize tags immediately
      * after the style definition is loaded

     \param  stylePath Path of style definition
   */
   bool initTheme(const string& themePath);

   /** \param page size*/
   void setPageSize(const string & ps);

  private:

   /** prints document header
    */
    string getHeader();

    /** Prints document footer*/
    string getFooter();

    /** Prints document body*/
    void printBody();

    /** Map of several pagesizes */
    PagesizeMap psMap;

    /** name of page size which is mapped to page dimensions*/
    string pageSize;

    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    /**\return text formatting attributes in RTF format */
    string  getAttributes( const ElementStyle & col);

    /** @param styleNumber number of current style
        @param elem associated element style
        @return RTF formatting seqence (colour index + bold + italic)*/
    string getOpenTag(int styleNumber,const ElementStyle &elem);

    /** @param elem associated element style
        @return RTF formatting sequnce to close element formatting */
    string getCloseTag(const ElementStyle &elem);

    /** @param styleID current style ID
        @return matching sequence to begin a new element formatting*/
    string getMatchingOpenTag(unsigned int styleID);

    /** @param styleID current style ID
        @return matching  sequence to stop element formatting*/
    string getMatchingCloseTag(unsigned int styleID);
  };

}
#endif
