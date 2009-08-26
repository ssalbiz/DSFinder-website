/***************************************************************************
                          codegenerator.h  -  description
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

#ifndef CODEPARSER_H
#define CODEPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

// Avoid problems with isspace and UTF-8 characters, use iswspace instead
//#include <cctype>
#include <wctype.h>

#include "languagedefinition.h"
#include "documentstyle.h"
#include "astyle/astyle.h"
#include "preformatter.h"
#include "enums.h"
#include "stringtools.h"


//#define NUMBER_BUILTIN_STYLES 10
//#define DEFAULT_LINE_NR_WIDTH     5

/// The highlight namespace contains all classes and data structures needed for parsing input data.

namespace highlight {

class ReGroup;

/** \brief Base class for parsing. Works like a finite state machine.

    The virtual class provides source code parsing functionality, based on
    information stored in language definitions.<br>
    The derived classes have to define the output format.<br>
    The colour information is stored in a DocumentStyle instance.<br>
    Codegenerator is a singleton class.

* @author Andre Simon
*/

class CodeGenerator
  {

  public:

    virtual ~CodeGenerator();

    /**
      Get appropriate Codegenerator instance
      \param type Output file type (HTML, XHTML, RTF, LATEX, TEX, ANSI, XTERM256)
    */
    static CodeGenerator* getInstance(OutputType type);

    /**
      Delete Codegenerator instance (this is intended for SWIG integration, 
      in normal C++ code the result should be saved in an auto_ptr)
      \param CodeGenerator* CodeGenerator instance
    */
    static void deleteInstance(CodeGenerator* inst) {if (inst) delete inst;}

    /**
     Generates output file
     \param inFileName Path of input file (if empty use stdin)
     \param outFileName Path of output file (if empty use stdout)
     \return ParseError
    */
    ParseError generateFile(const string &inFileName, const string &outFileName);

    /**
     Generates output string from input string
     \param input input code string
     \return formatted output code
    */
    string generateString(const string &input);

    /**
     Generates output string from input file
     \param inFileName file path
     \return formatted output code
    */
    string generateStringFromFile(const string &inFileName);

    /**
     Generates output string (added to make SWIG integration easier)
     \param input input code
     \return formatted output code
    */
    const char * generateCString(const string &input);

    /**
     Define colour theme information
     \param themePath Path of style description file
     \return true if successfull
    */
    virtual bool initTheme(const string& themePath);

    /*
     \return document style
    */
  //  DocumentStyle& getTheme();

    /** \return True if document style was found */
    bool styleFound();

    /** \return True if reformatting of current input is disabled */
    bool formattingDisabled();

    /** \return True if reformatting of current input is possible */
    bool formattingIsPossible();

    /** \param langDefPath Absolute path to language definition
        \return  Failure: LOAD_FAILED; Reload necessary: LOAD_NEW,
                 no reload necessary:  LOAD_NONE */
    LoadResult initLanguage(const string& langDefPath);

    /** \return Language definition*/
    const LanguageDefinition &getLanguage();

    /** tell parser to output line numbers
       \param  flag true if line numbers should be printed
       \param startCnt line number starting count
    */
    void setPrintLineNumbers(bool flag, unsigned int startCnt=1);

    /** \return line number flag */
    bool getPrintLineNumbers();

    /** tell parser to output line numbers filled with zeroes
        \param  flag true if zeroes should be printed
    */
    void setPrintZeroes(bool flag);

    /** \return print zeroes flag */
    bool getPrintZeroes();

    /** tell parser to omit document header and footer
       \param  flag true if output should be fragmented
    */
    void setFragmentCode(bool flag);

    /** define line number width
       \param  w width
    */
    void setLineNumberWidth(int w);

    /** \return line number width */
    int getLineNumberWidth();

    /** \return fragment flag */
    bool getFragmentCode();

    /** tell parser to test if input is binary or text
       \param  flag true if input should be checked
    */
    void setValidateInput(bool flag);

    /** \return input validation flag */
    bool getValidateInput();

    /** \return style path */
    const string& getStyleName();

    /** tell parser to use this font as base font
    \param s the font name, e.g. "Courier New"
     */
    void setBaseFont(const string& s);

    /** \return base font */
    const string getBaseFont() const ;

    /** tell parser to use this size as base font size
    \param s the font size, e.g. "12"
     */
    void setBaseFontSize(const string& s);

    /** \return base font size*/
    const string& getBaseFontSize();

    /** tell parser the wrapping mode
       \param lineWrappingStyle wrapping style
       \param lineLength max line length
       \param numberSpaces number of spaces which replace a tab
    */
    void setPreformatting(WrapMode lineWrappingStyle, unsigned int lineLength,int numberSpaces);

    /** tell parser the include style definition in output
        \param flag true if style should be included
     */
    void setIncludeStyle(bool flag);

    /** define output format specific options
        \param htmlAttachAnchors set true if anchors should be added to line numbers
        \param htmlOrderedList set true if line numbers should be replaced by ordered list
        \param htmlInlineCSS set true if CSS should be outputted in each span element
        \param htmlEnclosePreTag set true if fragmented output should be enclosed in pre tag
        \param latexReplQuotes set true if quotes should be replaced by \dq
        \param latexDisableShorthands set true if Babel shorthands should be disabled
        \param rtfPageSize RTF page size (a3, a4, a5, b4, b5, b6, letter, legal)
        \param htmlAnchorPrefix HTML Anchor prefix
        \param htmlCSSClassName HTML CSS class name
     */
    void setSpecialOptions(bool htmlAttachAnchors,bool htmlOrderedList, bool htmlInlineCSS, bool htmlEnclosePreTag,
                           bool latexReplQuotes, bool latexDisableShorthands,
                           const string&rtfPageSize, const string& htmlAnchorPrefix,
                           const string& htmlCSSClassName="hl");

   /** Print style definitions to external file
      TODO if outile is empty, dump style info to stdout
     \param outFile Path of external style definition
     */
    bool printExternalStyle(const string &outFile);

   /** Print index file with all input file names
      \param fileList List of output file names
      \param outPath Output path
    */
    virtual bool printIndexFile(const vector<string> & fileList,
                                const string &outPath);

   /** initialize source code indentation
       \param indentSchemePath Path of indentation scheme
       \return true id successfull
    */
    bool initIndentationScheme(const string&indentSchemePath);

   /** \return pointer to formatter object (NULL if no formatting possible or defined)
    */
    astyle::ASFormatter *getFormatter() { return formatter; }

    /** Set style input path
       \param path path to style input file
     */
    void setStyleInputPath(const string& path);

    /** Set style output path
      \param path path to style output file
    */
    void setStyleOutputPath(const string& path);

    /** Set encloding
      \param encodingName encoding name
    */
    void setEncoding(const string& encodingName);

    /** \return style input file path */
    const string&  getStyleInputPath();

    /** \return style output file path */
    const string&  getStyleOutputPath();

    /** \param title Document title */
    void setTitle(const string & title);

    /** \return Document title */
    string &getTitle();

    /** \param p HTML anchor prefix */
    void setHTMLAnchorPrefix(const string & p);

    /** \param cnt maximum number of input lines to be processed */
    void setMaxInputLineCnt(unsigned int cnt);

    /** \return true if chosen document style has white background colour */
    bool hasWhiteBGColour();

   /** \param keyCase Keyword case */
   void setKeyWordCase(KeywordCase keyCase);

   /** \param lineNo number of line that should be marked
       \param helpTxt additional help text */
   void addMarkedLine(int lineNo, string &helpTxt);

protected:

    static const int NUMBER_BUILTIN_STYLES;

    /** \param type Output type */
    CodeGenerator(highlight::OutputType type);
    CodeGenerator() {};

    /** \param c Character to be masked
        \return Escape sequence of output format */
    virtual string maskCharacter(unsigned char c) = 0;

    /** \param ss destination stream
        \param s string */
    void maskString(ostream& ss, const string &s ) ;

    /** \param s Symbol string
        \param searchPos Position where search starts
        \return Found state (integer value)  */
    State getState(const string &s, unsigned int searchPos);

    /** Get current line number
      \return line number  */
    unsigned int getLineNumber();

    vector <string> styleTagOpen,   ///< list of format delimiters (open new format descriptions)
                   styleTagClose;   ///< list of format delimiters (close format descriptions)

    /** Description of document colour style*/
    DocumentStyle docStyle;

    /** Language definition*/
    LanguageDefinition langInfo;

    /** Tag for inserting line feeds*/
    string newLineTag;

    /** String that represents a white space in output */
    string spacer;

    /** file input*/
    istream *in;

    /** file output*/
    ostream *out;

    string maskWsBegin,  ///< open whitespace mask 
           maskWsEnd;    ///< close whitespace mask

    string styleCommentOpen,  ///< open comment delimiter
           styleCommentClose; ///< close comment delimiter

    /** Encoding name */
    string encoding;

    /** Encoding name */
    string docTitle;

    /** Test if maskWsBegin and maskWsEnd should be applied */
    bool maskWs;

    /** Test if whitespace sould always be separated from enclosing tokens */
    bool excludeWs;

    /** Test if header and footer should be omitted */
    bool fragmentOutput;

    /** Test if line numbers should be printed */
    bool showLineNumbers;

    /** Test if leading spyce of line number should be filled with zeroes*/
    bool lineNumberFillZeroes;

    /** The base font to use */
    string baseFont ;

    /** The base font size to use */
    string baseFontSize ;

    /** Current line of input file*/
    string line;

    /** Current line number */
    unsigned int lineNumber;

    /**output line number count start */
    int lineNumberOffset;

    /** Current state*/
    State currentState;

    /** keyword class id, used to apply the corresponding keyword style*/
    unsigned int currentKeywordClass;

    /** Processes origin state */
    void processRootState();

    /** \return line break sequence */
    virtual string getNewLine();

    /**
       \param s current state
       \param kwClassID keyword class (has to be set when s=KEYWORD)
       \return Index of style tag corresponding to the states
    */
    unsigned int getStyleID(State s, unsigned int kwClassID = 0);

    /** \return line index */
    unsigned int  getLineIndex();

    /** print all remaining white space*/
    void flushWs();

    /** \return Content of user defined input style */
    string readUserStyleDef();

    /** \return Style definition of the chosen output format */
    virtual string  getStyleDefinition() {return "";};

    /** \return true id encoding is defined */
    bool encodingDefined() {return StringTools::lowerCase(encoding)!="none";}

    /** contains white space, which will be printed after a closing tag */
    string wsBuffer;

    /** Flag to test if style definition should be included in output document */
    bool includeStyleDef;

   /** map which saves all lines that should be marked as special*/
    map <int, string> markLines;

private:

    CodeGenerator(const CodeGenerator&){cerr<< "Doh!\n";}

    CodeGenerator& operator=(CodeGenerator&){ return *this;}


    /** Insert line number at the beginning of current output line */
    virtual void insertLineNumber(bool insertNewLine=true);

    /** Prints document footer
        @return footer */
    virtual string getFooter() = 0;

    /** Prints document body*/
    virtual void printBody() = 0;

    /** Prints document header
        @return header
    */
    virtual string getHeader() = 0;


    /** return matching open tags of the given state */
    virtual string getMatchingOpenTag(unsigned int) = 0;

    /** return matching close tags of the given state */
    virtual string getMatchingCloseTag(unsigned int) = 0;

   /** open a new tag, set current state to s*/
    void openTag(State s);

    /** close opened tag, clear current state */
    void closeTag(State s);

    void closeTag(unsigned int styleID);

    void openTag(unsigned int styleID);

    /// path to style definition file
    string  themePath;

    /// contains current position in line
    unsigned int lineIndex;

    /// width of line numbering coloumn
    unsigned int lineNumberWidth;

    /**maximum count of input lines to be processed*/
    unsigned int maxLineCnt;

    /**last character of the last line*/
    unsigned char terminatingChar;

    /** Class for reformatting */
    astyle::ASFormatter *formatter;

    /** Class for line wrapping and tab replacement*/
    PreFormatter preFormatter;

    /** Flag to test if formatting is enabled with current input document*/
    bool formattingEnabled;

    /** Flag to test if formatting is possible with current input document*/
    bool formattingPossible;

    /** Flag to test if input should be validated (binary or text) */
    bool validateInput;

    /** flag which determines keyword output (unchangeed, uppercase, lowercase)*/
    KeywordCase keywordCase;

    /** contains the current token*/
    string token;

    string styleInputPath, styleOutputPath;

    /** Resets parser to origin state, call this after every file conversion */
    void reset();

    /** read new line from in stream */
    bool readNewLine(string &newLine);

    /** return next character from in stream */
    unsigned char getInputChar();

    OutputType outputType;

   /** return new state */
    State getCurrentState ();

    /** Methods that represent a parsing state */
    bool processKeywordState(State myState) ;
    bool processNumberState() ;
    bool processMultiLineCommentState();
    bool processSingleLineCommentState();
    bool processStringState(State oldState);
    bool processEscapeCharState();
    bool processDirectiveState();
    bool processTagState();
    bool processSymbolState();
    void processWsState();

    /** print escaped token and clears it */
    void printMaskedToken(bool flushWhiteSpace=true, KeywordCase tcase=CASE_UNCHANGED);

    /** close Keyword tag of corresponding style ID */
    void closeKWTag(unsigned int styleID);

   /** open Keyword tag of corresponding style ID */
    void openKWTag(unsigned int styleID);

    /* look for special commands in comments, take action in derived class
        \return true if command was found
    */
   // bool checkSpecialCmd();

    map <int, ReGroup> regexGroups;

    void matchRegex(const string &);

    /** \return true if input is no binary stream */
    bool validateInputStream();

  };

  class ReGroup {
  public:

    ReGroup(): length(0), state(STANDARD), kwClass(0) {
//      cerr << "new REGroup len "<<l<< "  state "<<s<<"  class "<<c <<endl;
    }

    ReGroup(State s, unsigned int l , unsigned int c ): length(l), state(s), kwClass(c) {
//      cerr << "new REGroup len "<<l<< "  state "<<s<<"  class "<<c <<endl;
    }

    ReGroup(const ReGroup& other) {
      length = other.length;
      state = other.state;
      kwClass = other.kwClass;
    } 

    ReGroup& operator=(const ReGroup & other) {
      length = other.length;
      state = other.state;
      kwClass = other.kwClass;
      return *this;
    }

    ~ReGroup(){
     }

    unsigned int length;
    State state;
    unsigned int kwClass;
  };

}

#endif
