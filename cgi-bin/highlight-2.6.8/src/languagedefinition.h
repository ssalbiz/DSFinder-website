/***************************************************************************
                          languagedefinition.h  -  description
                             -------------------
    begin                : Wed Nov 28 2001
    copyright            : (C) 2001-2007 by Andre Simon
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

#ifndef LANGUAGEDEFINITION_H
#define LANGUAGEDEFINITION_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

#include "configurationreader.h"
#include "platform_fs.h"
#include "enums.h"
#include "re/Pattern.h"
#include "re/Matcher.h"

namespace highlight {

class RegexElement;

/** maps keywords and the corresponding class IDs*/
typedef map <string, int> KeywordMap;


/**\brief Contains specific data of the programming language being processed.

   The load() method will only read a new language definition if the given
   file path is not equal to the path of the current language definition.

* @author Andre  Simon
*/

class LanguageDefinition  {

  public:

    LanguageDefinition();

    ~LanguageDefinition();

    /**\return Symbol string, containg all known symbols with the referencing state ids*/
    const string &getSymbolString() const { return symbolString; }

    /** \return Prefix of raw strings */
    unsigned char getRawStringPrefix() const { return rawStringPrefix; }

   /** \return Continuation Character */
    unsigned char getContinuationChar() const { return continuationChar; }

    /** \return true if syntax highlighting is enabled*/
    bool getSyntaxHighlight() const { return !disableHighlighting;}

    /** \return True if language is case sensitive */
    bool isIgnoreCase() const { return ignoreCase;}

    /** \param s String
         \return class id of keyword, 0 if s is not a keyword */
    int isKeyword(const string &s) ;

    /** Load new language definition
        \param langDefPath Path of language definition
        \param clear Test if former data should be deleted
        \return True if successfull  */
    bool load(const string& langDefPath, bool clear=true);

     /** \return  True if programming language is VHDL */
    bool isVHDL() const { return vhdlMode; }

    /** \return True if multi line comments may be nested */
    bool allowNestedMLComments() const { return allowNestedComments; }

    /** \return True if highlighting is disabled */
    bool highlightingDisabled() const  { return disableHighlighting; }

    /** \return True the next load() call will load a new language definition
        \param  langDefPath Path to language definition  */
    bool needsReload(const string &langDefPath) const { return currentPath!=langDefPath; }

    /** \return True if current language may be reformatted (c, c++, c#, java) */
    bool enableReformatting() const { return reformatCode;}

    /** \return True if escape sequences are allowed outsde of strings */
    bool allowExtEscSeq() const { return allowExtEscape; }

    /** \return Class ID of given keyword delimiter prefix
        \param prefix Keyword delimiter prefix   */
   //unsigned int getDelimPrefixClassID(const string& prefix);

    /** \return keywords*/
    const KeywordMap& getKeywords() const { return keywords; }

    /** \return keyword classes*/
    const vector<string>& getKeywordClasses() const { return keywordClasses;}

    /** \return regular expressions */
    const vector<RegexElement*>& getRegexElements() const {return regex;};

    /** \return description of the programming language */
    const string & getDescription () const {return langDesc;}

    bool delimiterIsDistinct (int classID) {
           return delimiterDistinct[classID];
    }

    int getDelimiterPairID(const string& delim){
       return delimiterPair[delim];
    }

  private:

    static const string REGEX_IDENTIFIER;
    static const string REGEX_NUMBER;

    // string containing symbols and their IDs of the programming language
    string symbolString;

    // path to laoded language definition
    string currentPath;

    // Language description
    string langDesc;

    KeywordMap keywords;

    vector <string> keywordClasses;

    vector <RegexElement*> regex;

    KeywordMap delimiterPrefixes;

    // saves if delimiter pair consists of the same delimiter symbol
    map <int, bool> delimiterDistinct;

    map <string, int> delimiterPair;

    // keywords are not case sensitive if set
    bool ignoreCase,

    // highlighting is disabled
    disableHighlighting,

    // Escape sequences are allowed outrside of strings
    allowExtEscape,

    // switch to enable VHDL workarounds
    vhdlMode,

    // allow nested multi line comment blocks
    allowNestedComments,

    // single line comments have to start in coloumn 1 if set
    fullLineComment,

    // code formatting is enabled if set
    reformatCode;

    // character which is prefix of raw string (c#)
    unsigned char rawStringPrefix,

   //character which continues curreent style on next line
    continuationChar;

    /** reset members */
    void reset();

    // add a symbol sequence to the symbolStream
    void addSimpleSymbol(stringstream& symbolStream, State state,
                         const string& paramValue );

    void addSymbol(stringstream& symbolStream,
                                       State stateBegin,
                                       State stateEnd,
                                       bool isDelimiter,
                                       const string& paramValue,
                                       unsigned int classID=0 );

    // add a delimiter symbol sequence to the symbolStream
    void addDelimiterSymbol(stringstream& symbolStream,
                            State stateBegin, State stateEnd,
                            const string& paramValue,
                            unsigned int classID=0);

    bool getFlag( string& paramValue);

    unsigned char getSymbol(const string& paramValue);

    // generate a unique class ID of the class name
    unsigned int generateNewKWClass(const string& newClassName);

    // add keywords to the given class
    void addKeywords(const string &kwList,State stateBegin, State stateEnd, int classID);

    struct RegexDef extractRegex(const string &paramValue);

    Pattern * reDefPattern;

  };


  class RegexElement {
    public:
         RegexElement():open(STANDARD), end(STANDARD), rePattern(NULL), kwClass(0),groupID(-1)
        {
        }

        RegexElement(State oState, State eState, Pattern *re, unsigned int cID=0, int group=-1):
        open(oState), end(eState), rePattern(re), kwClass(cID), groupID(group)
        {
       // cerr << "new re element "<<  rePattern->getPattern() <<" open: "<<open<<" end "<<end<<"\n";
        }

        ~RegexElement() { if (rePattern) delete rePattern; }

        State open, end;
        Pattern *rePattern;
        unsigned int kwClass;
        int groupID;
  };

  struct RegexDef
{
  RegexDef():capturingGroup(-1){}
  string reString;
  int capturingGroup;
};

}
#endif
