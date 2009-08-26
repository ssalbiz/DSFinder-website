//
// C++ Interface: enums
//
// Description:
//
//
// Author: Andre Simon <andre.simon1@gmx.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ENUMS_H
#define ENUMS_H

namespace highlight {

/** states which may occour during input file parsing
    TODO Clean up!
*/
enum State {
  STANDARD=0,
  STRING,
  NUMBER,
  SL_COMMENT,
  ML_COMMENT,
  ESC_CHAR,
  DIRECTIVE,
  DIRECTIVE_STRING,
  LINENUMBER,
  SYMBOL,

  // don't use constants > 10 as array indices!
  KEYWORD,
  STRING_END,
  NUMBER_END,
  SL_COMMENT_END,
  ML_COMMENT_END,
  ESC_CHAR_END,
  DIRECTIVE_END,
  SYMBOL_END,
  //TAG_BEGIN,
  //TAG_END,
  //KEYWORD_BEGIN,
  KEYWORD_END,
  IDENTIFIER_BEGIN,
  IDENTIFIER_END,

  _UNKNOWN=100,
  _EOL,
  _EOF,
  _WS
} ;

/** Parser return values*/
enum ParseError{
   PARSE_OK,
   BAD_INPUT=1,
   BAD_OUTPUT=2,
   BAD_STYLE=4,
   BAD_BINARY=8
};

/** line wrapping modes*/
enum WrapMode {
  WRAP_DISABLED,
  WRAP_SIMPLE,
  WRAP_DEFAULT
};

/** language definition loading results*/
enum LoadResult{
  LOAD_FAILED,
  LOAD_NEW,
  LOAD_NONE
};

/** output formats */
enum OutputType {
  HTML,
  XHTML,
  TEX,
  LATEX,
  RTF,
  XML,
  ANSI,
  XTERM256,
  HTML32
};

/** Change Keyword case*/
enum KeywordCase {
  CASE_UNCHANGED,
  CASE_LOWER,
  CASE_UPPER,
  CASE_CAPITALIZE
};


}

#endif
