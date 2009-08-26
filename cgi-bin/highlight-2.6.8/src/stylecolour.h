/***************************************************************************
                          stylecolour.h  -  description
                             -------------------
    begin                : Die Nov 5 2002
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

#ifndef STYLECOLOUR_H
#define STYLECOLOUR_H

#include "enums.h"

#include <string>


using namespace std;

namespace highlight {

/**\brief Stores colours and returns red, green and blue values in different formats
* @author Andre Simon
 */

struct RGBVal {
  int iRed,    ///< Red value
      iGreen,  ///< Green value
      iBlue;   ///< Blue value
};

class StyleColour
  {
  public:
    /** Constructor
        \param red Red value in hex notation
        \param green Blue value in hex notation
        \param blue Green value in hex notation
    */
    StyleColour(const string & red, const string & green, const string & blue);

    /** Constructor
        \param styleColourString String with rgb values
    */
    StyleColour(const string & styleColourString);

    StyleColour();
    ~StyleColour(){};

   /** Sets red, green and blue values
     \param styleColourString String containing colour attributes
   */
   void setRGB(const string & styleColourString);


    /** Sets red value
        \param red New red value */
    void setRed(const string & red);

    /** Sets green value
        \param green New green value */
    void setGreen(const string & green);

    /** Sets blue value
        \param blue New blue value */
    void setBlue(const string & blue);

    /**  @param type Output type
         @return Red value in color representation according to output type */
    string getRed(OutputType type);
    /**  @param type Output type
         @return Green value in color representation according to output type */
    string getGreen(OutputType type);
    /**  @param type Output type
         @return Blue value in color representation according to output type */
    string getBlue(OutputType type);

  private:
    RGBVal rgb;
    string int2str(int, std::ios_base& (*f)(std::ios_base&) );
    string float2str(double);
  };

}

#endif
