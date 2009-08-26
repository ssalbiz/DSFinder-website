/***************************************************************************
                          codegenerator.cpp  -  description
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

#include <climits>
#include <memory>

#include "codegenerator.h"

#include "htmlgenerator.h"
#include "html32generator.h"
#include "xhtmlgenerator.h"
#include "rtfgenerator.h"
#include "latexgenerator.h"
#include "texgenerator.h"
#include "xmlgenerator.h"
#include "re/Matcher.h"
#include "astyle/astyle.h"

//-> siehe wx/defs.h
#ifndef __WXWINDOWS__
  #include "ansigenerator.h"
  #include "xterm256generator.h"
#endif

using namespace std;

namespace highlight {

const int CodeGenerator::NUMBER_BUILTIN_STYLES = 10;

CodeGenerator * CodeGenerator::getInstance(OutputType type) {
  CodeGenerator* generator=NULL;
  switch (type){
     case HTML:
        generator = new HtmlGenerator();
        break;
      case XHTML:
        generator = new XHtmlGenerator();
        break;
      case TEX:
        generator = new TexGenerator ();
        break;
      case LATEX:
        generator = new LatexGenerator();
        break;
      case RTF:
        generator = new RtfGenerator ();
        break;
      case XML:
        generator = new XmlGenerator();
        break;
      case HTML32:
        generator = new Html32Generator();
        break;
      #ifndef __WXWINDOWS__
      case ANSI:
        generator = new AnsiGenerator();
        break;
      case XTERM256:
        generator = new Xterm256Generator();
        break;
      #endif
      default:
        break;
  }
  return generator;
}

CodeGenerator::CodeGenerator(highlight::OutputType type)
   :in(NULL),
    out(NULL),
    encoding("none"),
    docTitle("Source file"),
    maskWs(false),
    excludeWs(false),
    fragmentOutput(false),
    showLineNumbers (false),
    lineNumberFillZeroes(false),
    lineNumber(0),
    lineNumberOffset(0),
    includeStyleDef(false),
    lineIndex(0),
    lineNumberWidth(5),
    maxLineCnt(UINT_MAX),
    terminatingChar('\0'),
    formatter(NULL),
    formattingEnabled(false),
    formattingPossible(false),
    validateInput(false),
    keywordCase(CASE_UNCHANGED),
    outputType(type)
{
}

CodeGenerator::~CodeGenerator()
{
  delete formatter;
}

/** Getter and Setter*/

void CodeGenerator::setSpecialOptions(bool htmlAttachAnchors,bool htmlOrderedList,
                           bool htmlInlineCSS, bool htmlEnclosePreTag,
                           bool latexReplQuotes, bool latexDisableShorthands,
                           const string& rtfPageSize, const string& htmlAnchorPrefix,
			   const string& htmlCSSClassName) {
   switch (outputType) {
     case HTML:
          ((HtmlGenerator*)this)->setAttachAnchors(htmlAttachAnchors);
          ((HtmlGenerator*)this)->setAnchorPrefix(htmlAnchorPrefix);
          ((HtmlGenerator*)this)->setOrderedList(htmlOrderedList);
          ((HtmlGenerator*)this)->setCSSClassName(htmlCSSClassName);
          ((HtmlGenerator*)this)->setInlineCSS(htmlInlineCSS);
          ((HtmlGenerator*)this)->setEnclosePreTag(htmlEnclosePreTag);
          break;
     case XHTML:
          ((XHtmlGenerator*)this)->setAttachAnchors(htmlAttachAnchors);
          ((XHtmlGenerator*)this)->setAnchorPrefix(htmlAnchorPrefix);
          ((XHtmlGenerator*)this)->setOrderedList(htmlOrderedList);
          ((XHtmlGenerator*)this)->setCSSClassName(htmlCSSClassName);
          ((XHtmlGenerator*)this)->setInlineCSS(htmlInlineCSS);
          ((XHtmlGenerator*)this)->setEnclosePreTag(htmlEnclosePreTag);
          break;
     case LATEX:
          ((LatexGenerator*)this)->setReplaceQuotes(latexReplQuotes);
          ((LatexGenerator*)this)->setDisableShorthands(latexDisableShorthands);
          break;
     case RTF:
          ((RtfGenerator*)this)->setPageSize(rtfPageSize);
          break;
     default: break;
   }
}

void CodeGenerator::setHTMLAnchorPrefix(const string & p){
        if (outputType==HTML)
            ((HtmlGenerator*)this)->setAnchorPrefix(p);
        if (outputType==XHTML)
            ((XHtmlGenerator*)this)->setAnchorPrefix(p);
}

bool CodeGenerator::initTheme(const string& themePath) {
  this->themePath=themePath;
  return docStyle.load(themePath);
}

bool CodeGenerator::hasWhiteBGColour(){
    StyleColour bgCol = docStyle.getBgColour();
    return bgCol.getRed(HTML) == "ff" && bgCol.getGreen(HTML) == "ff" && bgCol.getBlue(HTML) == "ff";
}

const string& CodeGenerator::getStyleName(){
  return themePath;
}

void CodeGenerator::setLineNumberWidth(int w) {
  lineNumberWidth=w;
}

int CodeGenerator::getLineNumberWidth(){
  return lineNumberWidth;
}

void CodeGenerator::setPrintLineNumbers(bool flag, unsigned int startCnt){
  showLineNumbers=flag;
   lineNumberOffset = startCnt-1;
}

bool CodeGenerator::getPrintLineNumbers(){
  return showLineNumbers;
}

void CodeGenerator::setPrintZeroes(bool flag){
  lineNumberFillZeroes=flag;
}

bool CodeGenerator::getPrintZeroes(){
  return lineNumberFillZeroes;
}

void CodeGenerator::setIncludeStyle(bool flag){
    includeStyleDef = flag;
}

void CodeGenerator::setStyleInputPath(const string& path){
    styleInputPath = path;
}
void CodeGenerator::setStyleOutputPath(const string& path){
    styleOutputPath = path;
}

const string&  CodeGenerator::getStyleInputPath(){
    return styleInputPath;
}
const string&  CodeGenerator::getStyleOutputPath(){
    return styleOutputPath;
}

void CodeGenerator::setFragmentCode(bool flag){
  fragmentOutput=flag;
}
bool CodeGenerator::getFragmentCode(){
  return fragmentOutput;
}

void CodeGenerator::setValidateInput(bool flag){
  validateInput=flag;
}
bool CodeGenerator::getValidateInput(){
  return validateInput;
}

void CodeGenerator::setBaseFont(const string& s) {
   baseFont = s;
}

void CodeGenerator::setBaseFontSize(const string& s) {
   baseFontSize = s ;
}

const string CodeGenerator::getBaseFont() const {
  if (baseFont.empty()){
     switch (outputType) {
        case LATEX:
          return "ttfamily";
          break;
       case TEX:
          return "tt";
          break;
       default:
          return "Courier New";
     }
   } else {
     return baseFont ;
   }
}

const string& CodeGenerator::getBaseFontSize()  {
   if (baseFontSize.empty() && outputType != LATEX && outputType != TEX){
      return docStyle.getFontSize();
   } else {
      return baseFontSize;
   }
}

void CodeGenerator::setTitle(const string & title){
        if (!title.empty()) docTitle= title;
}

string &CodeGenerator::getTitle(){
        return docTitle;
}

void CodeGenerator::setEncoding(const string& encodingName){
   encoding = encodingName;
}

bool CodeGenerator::formattingDisabled(){
  return !formattingEnabled;
}

void CodeGenerator::setMaxInputLineCnt(unsigned int cnt){
    maxLineCnt = cnt;
}

bool CodeGenerator::formattingIsPossible(){
  return formattingPossible;
}

void CodeGenerator::setPreformatting(WrapMode lineWrappingStyle,
                                     unsigned int lineLength,
                                     int numberSpaces ){
  bool enableWrap = lineWrappingStyle!=WRAP_DISABLED;
  bool replaceTabs = numberSpaces > 0;

  if (enableWrap || replaceTabs)  {
    preFormatter.setWrap(enableWrap);
    preFormatter.setWrapIndentBraces(lineWrappingStyle==WRAP_DEFAULT);
    preFormatter.setWrapLineLength(lineLength);
    preFormatter.setReplaceTabs(replaceTabs);
    preFormatter.setNumberSpaces(numberSpaces);
  }
}

void CodeGenerator::setKeyWordCase(KeywordCase keyCase){
   keywordCase = keyCase;
}

void CodeGenerator::addMarkedLine(int lineNo, string& helpTxt){
   markLines[lineNo] = helpTxt;
}
/*
highlight::DocumentStyle& CodeGenerator::getTheme() {
  return docStyle;
}
*/
const LanguageDefinition &CodeGenerator::getLanguage(){
  return langInfo;
}

void CodeGenerator::reset()
{
  lineIndex = 0;
  lineNumber = 0;
  line.clear();
  preFormatter.reset();
}

/** sucht vorwaerts ab Position searchPos Ziffer in s und liefert Integerwert
der gefundenen Zahl zurueck.
Im SymbolString stehen die den einzelnen Symbolen zugeordneten Konstanten
immer HINTER diesen Symbolen*/
State CodeGenerator::getState(const string &s, unsigned int searchPos)
{
  string::size_type pos = s.find_first_of("1234567890", searchPos+1);
  if (pos==string::npos) return _UNKNOWN;

  string::size_type pos2 = s.find(' ', pos);
  int result=_UNKNOWN;
  StringTools::str2num<int>(result, s.substr(pos, pos2-pos), std::dec);
  return (State)result;
}

unsigned int CodeGenerator::getLineNumber()
{
  return lineNumber;
}

bool CodeGenerator::readNewLine(string &newLine){
  bool eof;
  if (lineIndex) terminatingChar=newLine[lineIndex-1];
  if (formattingPossible && formattingEnabled) {
     eof=!formatter->hasMoreLines();
     if (!eof) {
         newLine = formatter->nextLine();
     }
   }
   // reformatting not enabled
   else {
     eof = ! getline( *in, newLine);
   }
   return eof || (lineNumber == maxLineCnt);
}



void CodeGenerator::matchRegex(const string &line){

    regexGroups.clear();
    int matchBegin=0;
    int matchLen=0;
    int groupID=0;
    // cycle through all regex, save the start and ending indices to report them later
    for (unsigned int i=0; i<langInfo.getRegexElements().size(); i++){
        RegexElement *regexElem = langInfo.getRegexElements()[i];
        auto_ptr<Matcher> matcher(regexElem->rePattern->createMatcher(line)); 
        while (matcher->findNextMatch()) {
            groupID = (regexElem->groupID<0)? matcher->getGroupNum()-1 : regexElem->groupID;
            matchBegin =  matcher->getStartingIndex(groupID);
            if (matchBegin<0) continue;
            matchLen = matcher->getEndingIndex(groupID) - matchBegin;
/*
            std::cerr << "\nmatchBegin="<<1+ matchBegin
                      << " matchLen old" << ( matcher->getGroup(matcher->getGroupNum()-1).size())
                      << " matchLen new" << matchLen<<" group: "<<(matcher->getGroup(matcher->getGroupNum()-1))
                      << " group id= "<<regexElem->groupID<<"\n";
*/
            regexGroups.insert(
            make_pair(matchBegin+1, ReGroup(regexElem->open, matchLen, regexElem->kwClass)) );
        }
   }
}

unsigned char CodeGenerator::getInputChar()
{
  bool eol = lineIndex == line.length();

  if (eol) {
    bool eof=false;
    if (preFormatter.isEnabled()){
       if (!preFormatter.hasMoreLines()) {
          eof=readNewLine(line);
          preFormatter.setLine(line);
       }
       line = preFormatter.getNextLine();
    } else {
      eof=readNewLine(line);
    }
    lineIndex=0;
    ++lineNumber;
    line=StringTools::trimRight(line);
    matchRegex(line);

    return (eof)?'\0':'\n';
  }

  return line[lineIndex++];
}

State CodeGenerator::getCurrentState ()
{

  unsigned char c='\0';

  if (token.length()==0) {
    c=getInputChar();
  }  else {
    lineIndex-= (token.length()-1);
    c=token[0];
  }
  if (c=='\n'){
    return _EOL;   // End of line
  }

  if (c=='\0') {
    return _EOF;   // End of file
  }

  if (iswspace(c)) {
      token= c;
      return _WS;
  }

  /** TODO
      COMMENT ... END 2 eintraege in langdef (ML_COMMENT_START=regex(), ML_COMMENT_END=regex())
      weil comment sonst als identifier erkannt wird
  */

  // Test if a regular expression was found at the current position
  if (!regexGroups.empty()){
      if (regexGroups.count(lineIndex)) {
       token = line.substr(lineIndex-1, regexGroups[lineIndex].length);
       unsigned int oldIndex= lineIndex;
       if (regexGroups[oldIndex].length>1) lineIndex+= regexGroups[lineIndex].length-1;

       if (regexGroups[oldIndex].state==IDENTIFIER_BEGIN || regexGroups[oldIndex].state==KEYWORD) {
           string reservedWord=(langInfo.isIgnoreCase()) ? StringTools::lowerCase(token):token;
           currentKeywordClass=langInfo.isKeyword(reservedWord);
           if (!currentKeywordClass && regexGroups[oldIndex].state==KEYWORD)
               currentKeywordClass = regexGroups[oldIndex].kwClass;
           return (currentKeywordClass) ? KEYWORD : STANDARD;
       } else {
           return regexGroups[oldIndex].state;
        }
     }
  }

  unsigned int symbolLength;
  size_t symbolPos;
  size_t symbolFind;
  bool found=false;
  string symbols=langInfo.getSymbolString();

  //TODO this while loop kills performance - adjust search algorithm

  symbolPos = symbols.find(c);
  // search symbols (comment delimiters, directives etc.)
  // before keywords, because alphabetic chars may be part of symbols, too
  while ((symbolPos!= string::npos) && (!found))
    {
      symbolFind = symbols.find(' ', symbolPos);
      if (symbolFind==string::npos) break;
      symbolLength=symbolFind-symbolPos;
      token = symbols.substr(symbolPos, symbolLength);
      // Abfrage nach Leerzeichen in SymbolString verhindert falsches
      // Erkennen von Symbolteilen:
      if (lineIndex && token == line.substr(lineIndex-1, symbolLength)
                    && iswspace(symbols[symbolPos-1]) ) {
         found = true;
         lineIndex += (symbolLength-1);
      } else {
        symbolPos = symbols.find_first_not_of(' ',symbols.find(' ',symbolPos));
      }
    }
  //std::cerr << "symbols found="<<found<<" token = "<<token<<"\n";


  // dirty workaround stuff in here
  if (found) {
    State foundState = getState(symbols, symbolPos);

     // get the current keyword class id to apply the corresponding formatting style
     /*   if (foundState==TAG_BEGIN ) {
       currentKeywordClass=langInfo.getDelimPrefixClassID(token);
     }*/
     // simple one byte escape characters:
     /*elseif (foundState==ESC_CHAR && lineIndex<line.length()) {
         if (token.length()==1) token += line[lineIndex++];
     } */
     // VHDL Workaround: distinguish string delimiters and event markers
     // (same eymbol: ')
     /*else */
     if (langInfo.isVHDL()
             && foundState==STRING && currentState!=STRING
             && lineIndex > 1
             &&(isdigit(line[lineIndex-2]) || isalpha(line[lineIndex-2]))){
        return STANDARD;
  }
    return foundState;
  }

  // Character not referring to any state
  token = c;
  return STANDARD;
}

//it is faster to pass ostream reference
void CodeGenerator::maskString( ostream& ss, const string & s)
{
  for (unsigned int i=0;i< s.length();i++){
      ss << maskCharacter(s[i]);
  }
}

void CodeGenerator::printMaskedToken(bool flushWhiteSpace, KeywordCase tcase)
{
  if (flushWhiteSpace) flushWs();

  if (tcase==CASE_LOWER) {
      token=StringTools::lowerCase(token);
  } else if (tcase==CASE_UPPER) {
      token=StringTools::upperCase(token);
  } else if (tcase==CASE_CAPITALIZE) {
      token=StringTools::capitalize(token);
  }
  maskString(*out, token);
  token.clear();
}

bool CodeGenerator::styleFound(){
  return docStyle.found();
}

bool CodeGenerator::printIndexFile(const vector<string> &fileList,
                                   const string &outPath){
  return true;
}

bool CodeGenerator::initIndentationScheme(const string &schemePath){

  if (formatter!=NULL){
    return true;
  }

  ConfigurationReader indentScheme(schemePath);
  if (indentScheme.found()){
    if (formatter==NULL) {
       formatter=new astyle::ASFormatter();

       string brackets=indentScheme.getParameter("brackets");
       if (!brackets.empty()){
         // Break brackets from pre-block code (i.e. ANSI C/C++ style).
         if (brackets=="break"){
           formatter->setBracketFormatMode(astyle::BREAK_MODE);
         }
         //Attach brackets to pre-block code (i.e. Java/K&R style).
         else if (brackets=="attach"){
           formatter->setBracketFormatMode(astyle::ATTACH_MODE);
         }
         // Break definition-block brackets and attach command-block brackets.
         else if (brackets=="linux"){
           formatter->setBracketFormatMode(astyle::BDAC_MODE);
         }
         // Break brackets before closing headers (e.g. 'else', 'catch', ..).
         // Should be appended to --brackets=attach or --brackets=linux.
         else if (brackets=="break-closing-headers"){
           formatter->setBreakClosingHeaderBracketsMode(true);
         }
       }

       string pad=indentScheme.getParameter("pad");
       if (!pad.empty()){
         //Insert space paddings around parenthesies only.
         if (pad=="paren"){
           formatter->setParensOutsidePaddingMode(true);
		   formatter->setParensInsidePaddingMode(true);
         }
         else if ( pad=="paren-out" ) { // FIX DOC
		   formatter->setParensOutsidePaddingMode(true);
	      }
	      else if ( pad=="paren-in" ) { // FIX DOC
		   formatter->setParensInsidePaddingMode(true);
	      }
         // Insert space paddings around operators only.
         else if (pad=="oper"){
           formatter->setOperatorPaddingMode(true);
         }
         //Insert space paddings around operators AND parenthesies.
        /* else if (pad=="all"){
           formatter->setOperatorPaddingMode(true);
           ///formatter->setParenthesisPaddingMode(true);
         }*/
       }

       string oneLine=indentScheme.getParameter("one-line");
       if (!oneLine.empty()){
         // Don't break one-line blocks.
         if (oneLine=="keep-blocks"){
           formatter->setBreakOneLineBlocksMode(false);
         }
         // Don't break complex statements and multiple statements residing in a
         // single line.
         else if (oneLine=="keep-statements"){
           formatter->setSingleStatementsMode(false);
         }
       }

       // Insert empty lines around unrelated blocks, labels, classes, ...
       string breakBlocks=indentScheme.getParameter("break-blocks");
       if (!breakBlocks.empty()){
         if (breakBlocks=="all"){
           //Like --break-blocks, except also insert empty lines around closing
           //headers (e.g. 'else', 'catch', ...).
           formatter->setBreakClosingHeaderBlocksMode(true);
         }
         formatter->setBreakBlocksMode(true);
       }
       string trueVal="true";

       // Other options...

       //Indent using # spaces per indent. Not specifying # will result in a
       //default of 4 spaces per indent.
       string indentSpaces=indentScheme.getParameter("indent-spaces");

       // Indent a minimal # spaces in a continuous conditional belonging to a
       //conditional header.
       string minConditionalIndent=indentScheme.getParameter("min-conditional-indent");

       // Indent a maximal # spaces in a continuous statement, relatively to the
       // previous line.
       string maxInStatementIndent=indentScheme.getParameter("max-instatement-indent");

       // Add extra indentation to '{' and '}' block brackets.
       string indentBrackets=indentScheme.getParameter("indent-brackets");

       // Add extra indentation entire blocks (including brackets).
       string indentBlocks=indentScheme.getParameter("indent-blocks");

       // Indent the contents of namespace blocks.
       string indentNamespaces=indentScheme.getParameter("indent-namespaces");

       // Indent 'class' blocks, so that the inner 'public:','protected:' and
       // 'private: headers are indented inrelation to the class block.
       string indentClasses=indentScheme.getParameter("indent-classes");

       // Indent 'switch' blocks, so that the inner 'case XXX:' headers are
       // indented in relation to the switch block.
       string indentSwitches=indentScheme.getParameter("indent-switches");

       // Indent 'case XXX:' lines, so that they are flush with their bodies..
       string indentCases=indentScheme.getParameter("indent-cases");

       // Indent labels so that they appear one indent less than the current
       // indentation level, rather than being    flushed completely to the left
       // (which is the default).
       string indentLabels=indentScheme.getParameter("indent-labels");

       // Indent multi-line #define statements
       string indentPreprocessor=indentScheme.getParameter("indent-preprocessor");

       // Break 'else if()' statements into two different lines.
       string breakElseIfs = indentScheme.getParameter("break-elseifs");

       string javaStyle = indentScheme.getParameter("java-style");

       // default values in ASBeautifier are false, it is ok to set them false
       // if parameter does not exist in scheme file
       formatter->setBracketIndent(indentBrackets==trueVal);
       formatter->setBlockIndent(indentBlocks==trueVal);
       formatter->setNamespaceIndent(indentNamespaces==trueVal);
       formatter->setClassIndent(indentClasses==trueVal);
       formatter->setSwitchIndent(indentSwitches==trueVal);
       formatter->setCaseIndent(indentCases==trueVal);
       formatter->setLabelIndent(indentLabels==trueVal);
       formatter->setPreprocessorIndent(indentPreprocessor==trueVal);
       formatter->setBreakElseIfsMode(breakElseIfs==trueVal);

       if (javaStyle==trueVal){
         formatter->setJavaStyle();
       }

       if (!indentSpaces.empty()){
         int intVal=0;
         StringTools::str2num<int>(intVal, indentSpaces, std::dec);
         formatter->setSpaceIndentation(intVal);
       }
       if (!minConditionalIndent.empty()){
         int intVal=0;
         StringTools::str2num<int>(intVal, minConditionalIndent, std::dec);
         formatter->setMinConditionalIndentLength(intVal);
       }
       if (!maxInStatementIndent.empty()){
         int intVal=0;
         StringTools::str2num<int>(intVal, maxInStatementIndent, std::dec);
         formatter->setMinConditionalIndentLength(intVal);
       }
    }
    formattingEnabled=(formatter != NULL);
    return true;
  } else {
    return false;
  }
}

LoadResult CodeGenerator::initLanguage(const string& langDefPath){
  bool reloadNecessary= langInfo.needsReload(langDefPath);
  if (reloadNecessary){

    if (!langInfo.load(langDefPath)) {
      return LOAD_FAILED;
    }

    formattingPossible=langInfo.enableReformatting();

    if (styleTagOpen.size()>NUMBER_BUILTIN_STYLES){
       // remove dynamic keyword tag delimiters of the old language definition
       vector<string>::iterator keyStyleOpenBegin =
           styleTagOpen.begin() + NUMBER_BUILTIN_STYLES;
       vector<string>::iterator keyStyleCloseBegin =
           styleTagClose.begin()+ NUMBER_BUILTIN_STYLES;
       styleTagOpen.erase(keyStyleOpenBegin, styleTagOpen.end());
       styleTagClose.erase(keyStyleCloseBegin, styleTagClose.end());
    }
    // add new keyword tag delimiters
    for (unsigned int i=0;i< langInfo.getKeywordClasses().size(); i++){
      styleTagOpen.push_back(getMatchingOpenTag(i));
      styleTagClose.push_back(getMatchingCloseTag(i));
    }
  }
  return (reloadNecessary) ? LOAD_NEW : LOAD_NONE;
}


bool CodeGenerator::validateInputStream(){
  if (!in) return false;
  // Sources: http://en.wikipedia.org/wiki/Magic_number_(programming)
  // Magic configuration of "file"
  // This is intended for web plugins - only check filetypes often found in the net
  char magic_gif[]    = {'G','I','F','8', 0};
  char magic_png[]    = {0x89,'P','N','G', 0};
  char magic_java[]   = {0xCA,0xFE,0xBA,0xBE, 0};
  char magic_jpeg[]   = {0xFF,0xD8,0xFF, 0};
  char magic_bmp[]    = {'B','M', 0};
  char magic_pdf[]    = {'%','P','D','F', 0};
  char magic_utf8[]   = {0xEF,0xBB,0xBF,0};
  char magic_rar[]    = {'R','a','r','!', 0};
  char magic_zip[]    = {'P','K',0x03,0x04, 0};
  char magic_ace[]    = {'*','*','A','C','E','*','*', 0};
  char magic_tgz[]    = {0x8b,0x1f, 0x00, 0x08, 0};
  char magic_bzip[]   = {'B','Z', 0};

  char* magic_table[] = {magic_utf8,
			magic_gif, magic_png, magic_jpeg, magic_bmp, magic_pdf,
			magic_java,
			magic_rar, magic_zip, magic_ace, magic_tgz, magic_bzip,
			0};

  char buffer [10]={0};
  in->read (buffer,8);  //only read the first 8 bytes of input stream

  int magic_index=0;
  while (magic_table[magic_index]){
       if (!strncmp(buffer, magic_table[magic_index], strlen(magic_table[magic_index]))) {
		break;
	}
	magic_index++;
  }
  int streamReadPos=0;
  if ( magic_table[magic_index] == magic_utf8) {
	//setEncoding("utf-8");
	streamReadPos=3; // remove UTF-8 magic number from output
  }
  in -> seekg(streamReadPos, ios::beg);
   in-> clear();  // clear fail bit to continue reading

  return !magic_table[magic_index] // points to 0 if no pattern was found
         || magic_table[magic_index] == magic_utf8;
}



ParseError CodeGenerator::generateFile (const string &inFileName,
                                        const string &outFileName)
{
  if (!docStyle.found()) {
    return BAD_STYLE;
  }

  reset();

  ParseError error=PARSE_OK;

  in = (inFileName.empty()? &cin :new ifstream (inFileName.c_str()));

  if (validateInput)
    if (!validateInputStream()) error= BAD_INPUT;

  if (!in->fail() && error==PARSE_OK) {
    out = (outFileName.empty()? &cout :new ofstream (outFileName.c_str()));
    if ( out->fail()) {
      error=BAD_OUTPUT;
    }
  }

  if ( in->fail()){
     error=BAD_INPUT;
  }

  if (error==PARSE_OK) {
    if (formatter != NULL){
       formatter->init(new astyle::ASStreamIterator(in));
    }
    if (! fragmentOutput){
      *out << getHeader();
    }

    printBody();

    if (! fragmentOutput){
      *out << getFooter();
    }
  }

  if (!outFileName.empty()){
    delete out; out=NULL;
  }
  if (!inFileName.empty()) {
    delete in; in=NULL;
  }
  return error;
}

string CodeGenerator::generateString(const string &input){

  if (!docStyle.found()) {
    return "";
  }

  reset();

  in = new istringstream (input);
  out = new ostringstream ();

  if ( in->fail() || out->fail()){
     return "";
  }

  if (formatter != NULL){
       formatter->init(new astyle::ASStreamIterator(in));
  }
  if (! fragmentOutput){
    *out << getHeader();
  }

  printBody();

  if (! fragmentOutput){
    *out << getFooter();
  }

  string result = static_cast<ostringstream*>(out)->str();

  delete out; out=NULL;
  delete in; in=NULL;

  return result;
}


string CodeGenerator::generateStringFromFile(const string &inFileName){

  if (!docStyle.found()) {
    return "";
  }

  reset();

  in = new ifstream (inFileName.c_str());
  out = new ostringstream ();

  if ( in->fail() || out->fail()){
     return "";
  }

  if (validateInput && !validateInputStream()) {
    return "ERROR: detected binary input";
  }

  if (formatter != NULL){
       formatter->init(new astyle::ASStreamIterator(in));
  }
  if (! fragmentOutput){
    *out << getHeader();
  }

  printBody();

  if (! fragmentOutput){
    *out << getFooter();
  }

  string result = static_cast<ostringstream*>(out)->str();

  delete out; out=NULL;
  delete in; in=NULL;

  return result;
}


const char* CodeGenerator::generateCString(const string &input) {
  return generateString(input).c_str();
}

unsigned int CodeGenerator::getStyleID(State s, unsigned int kwClassID){
  if (s==KEYWORD && kwClassID){
    return NUMBER_BUILTIN_STYLES + kwClassID-1;
  }
  return (unsigned int) s ;
}

void CodeGenerator::openTag(State s){
  *out << styleTagOpen[(unsigned int)s];
  currentState=s;
}

void CodeGenerator::closeTag(State s){
  *out << styleTagClose[(unsigned int)s];
  flushWs();
  currentState=_UNKNOWN;
}

void CodeGenerator::openKWTag(unsigned int kwClassID){
    *out << styleTagOpen[getStyleID(KEYWORD, kwClassID)];
    currentState=KEYWORD;
}

void CodeGenerator::closeKWTag(unsigned int kwClassID){
    *out << styleTagClose[getStyleID(KEYWORD, kwClassID)];
    flushWs();
    currentState=_UNKNOWN;
}


///////////////////////////////////////////////////////////////////////////////

void CodeGenerator::processRootState()
{

  bool eof=false,
       firstLine=true; // avoid newline before printing the first output line

  if (langInfo.highlightingDisabled()){
     string line;
     while (getline(*in, line)){
       ++lineNumber;
       insertLineNumber(!firstLine);
       flushWs();
       firstLine=false;
       maskString(*out, line);
     }
     *out << flush;
     return;
  }

  State state=STANDARD;

  openTag(STANDARD);
  do {
    // determine next state
    state= getCurrentState();

    // handle current state
    switch(state)
      {
      case KEYWORD:
        closeTag(STANDARD);
        eof=processKeywordState(state);
        openTag(STANDARD);
        break;
      case NUMBER:
        closeTag(STANDARD);
        eof=processNumberState();
        openTag(STANDARD);
        break;
      case ML_COMMENT:
        closeTag(STANDARD);
        eof=processMultiLineCommentState();
        openTag(STANDARD);
        break;
      case SL_COMMENT:
        closeTag(STANDARD);
        eof=processSingleLineCommentState();
        openTag(STANDARD);
        break;
      case STRING:
        closeTag(STANDARD);
        eof=processStringState(STANDARD);
        openTag(STANDARD);
        break;
      case DIRECTIVE:
        closeTag(STANDARD);
        eof=processDirectiveState();
        openTag(STANDARD);
        break;
/*
      case TAG_BEGIN:
        closeTag(STANDARD);
        eof=processTagState();
        openTag(STANDARD);
        break;
*/
      case ESC_CHAR:
        if (langInfo.allowExtEscSeq()){
          closeTag(STANDARD);
          eof=processEscapeCharState();
          openTag(STANDARD);
        } else {
          printMaskedToken();
        }
        break;
      case SYMBOL:
        closeTag(STANDARD);
        eof=processSymbolState();
        openTag(STANDARD);
        break;
      case _EOL:
        insertLineNumber(!firstLine);
        firstLine=false;
        break;
      case _EOF:
        eof=true;
        break;
      case _WS:
        processWsState();
        break;
      default:
        printMaskedToken();
        break;
      }
    }
  while (!eof);
  closeTag(STANDARD);
  *out << getNewLine();
  *out << flush;
}

bool CodeGenerator::processKeywordState(State myState){
  State newState=STANDARD;
  unsigned int myClassID=currentKeywordClass;
  bool eof=false,
       exitState=false;

   openKWTag(myClassID);
  do {
    printMaskedToken(newState!=_WS,
        (langInfo.isIgnoreCase())?keywordCase:highlight::CASE_UNCHANGED);
    newState= getCurrentState();
    switch(newState)
      {
      case _WS:
        processWsState();
        break;
      case _EOL:
        insertLineNumber();
        exitState=true;
        break;
      case _EOF:
        eof = true;
        break;
      case KEYWORD_END:
         exitState=true;
        break;
      default:
        exitState= (myClassID!=currentKeywordClass)||(myState!=newState);
        break;
      }
  } while ((!exitState) && (!eof));

  closeKWTag(myClassID);

  currentKeywordClass=0;
  return eof;
}

bool CodeGenerator::processNumberState(){
  State newState=STANDARD;
  bool eof=false,
       exitState=false;
  openTag(NUMBER);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();
    switch(newState)
      {
      case _WS:
        processWsState();
        break;
      case _EOL:
        insertLineNumber();
        exitState=true;
        break;
      case _EOF:
        eof = true;
        break;
      default:
        exitState=newState!=NUMBER;
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(NUMBER);
  return eof;
}

bool CodeGenerator::processMultiLineCommentState()
{
  int commentCount=1;
  int delimPairID = langInfo.getDelimiterPairID(token);
  State newState=STANDARD;
  bool eof=false, exitState=false;
  openTag(ML_COMMENT);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();

    switch(newState)
      {
      case _WS:
        processWsState();
        break;
      case _EOL:
        wsBuffer += styleTagClose[ML_COMMENT];
        insertLineNumber();
        wsBuffer += styleTagOpen[ML_COMMENT];
        break;
      case _EOF:
        eof = true;
        break;
      case ML_COMMENT:
        if (langInfo.allowNestedMLComments()) {
              ++commentCount;
              // SmallTalk-Fix: if multiline comment delims are equal, 
              // end comment when the same delimiter is read again and
              // nested comments are not allowed
              //break;
        }
        // if delimiters are equal, close the comment by continueing to 
        // ML_COMMENT_END section
        if (langInfo.delimiterIsDistinct(ML_COMMENT))  break;

      case ML_COMMENT_END:
        if (delimPairID!=langInfo.getDelimiterPairID(token)) break;
        commentCount--;
        if (!commentCount ) {
            printMaskedToken();
            exitState=true;
        }
        break;
      default:
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(ML_COMMENT);
  return eof;
}

bool CodeGenerator::processSingleLineCommentState()
{

  //if ( checkSpecialCmd()) return false;

  State newState=STANDARD;
  bool eof=false, exitState=false;

  openTag(SL_COMMENT);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();

    switch(newState)
      {
      case _WS:
        processWsState();
        break;
     case _EOL:
        printMaskedToken();
        if (preFormatter.isEnabled() && preFormatter.isWrappedLine(lineNumber-1)){
		exitState=false;
	} else {
 		exitState=true;
	}
        if (!exitState) wsBuffer += styleTagClose[SL_COMMENT];
        insertLineNumber();
        if (!exitState) wsBuffer += styleTagOpen[SL_COMMENT];

        break;
      case _EOF:
        eof = true;
        break;
      default:
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(SL_COMMENT);
  return eof;
}

bool CodeGenerator::processDirectiveState()
{
  State  newState=STANDARD;
  bool eof=false, exitState=false;

  openTag(DIRECTIVE);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();
    switch(newState)
      {
      case _WS:
        processWsState();
        break;
      case DIRECTIVE_END:
        printMaskedToken();
        exitState=true;
        break;
      case _EOL:
        printMaskedToken();
        if (preFormatter.isEnabled() && preFormatter.isWrappedLine(lineNumber-1 )){
		exitState=false;
	} else {
		exitState=(terminatingChar!=langInfo.getContinuationChar());
	}
        if (!exitState) wsBuffer += styleTagClose[DIRECTIVE];
        insertLineNumber();
        if (!exitState) wsBuffer += styleTagOpen[DIRECTIVE];
        break;
      case ML_COMMENT:
        closeTag(DIRECTIVE);
        eof= processMultiLineCommentState();
        openTag(DIRECTIVE);
        break;
      case SL_COMMENT:
        closeTag(DIRECTIVE);
        eof= processSingleLineCommentState();
        openTag(DIRECTIVE);
        exitState=true;
        break;
      case STRING:
        closeTag(DIRECTIVE);
        eof=processStringState(DIRECTIVE);
        openTag(DIRECTIVE);
        break;
      case _EOF:
        eof = true;
        break;
      default:
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(DIRECTIVE);
  return eof;
}

bool CodeGenerator::processStringState(State oldState)
{
  State newState=STANDARD;
  bool eof=false, exitState=false;
  bool returnedFromOtherState=false;
  // Test if character before string open delimiter token equals to the
  // raw string prefix (Example: r" ", r""" """ in Python)
  bool isRawString=false;
  if (lineIndex>token.length())
      isRawString = line[lineIndex-token.length()-1]==langInfo.getRawStringPrefix();

  //string openStringDelimiter=token;
  int delimPairID = langInfo.getDelimiterPairID(token);
  State myState= (oldState==DIRECTIVE) ? DIRECTIVE_STRING : STRING;
  openTag(myState);
  do {
    // true if last token was an escape char
    if (!returnedFromOtherState) {
         printMaskedToken(newState!=_WS);
    }
    returnedFromOtherState=false;
    newState= getCurrentState();

    switch(newState)
      {
      case _WS:
        processWsState();
        break;
      case _EOL:
        wsBuffer += styleTagClose[myState];
        insertLineNumber();
        wsBuffer += styleTagOpen[myState];
        break;
      case STRING_END:
          exitState= true;
          printMaskedToken();
          break;
      case STRING:
        // if there exist multiple string delimiters, close string if
        // current delimiters is equal to the opening delimiter
        //exitState= openStringDelimiter==token;
        exitState = (delimPairID==langInfo.getDelimiterPairID(token));
        printMaskedToken();
        break;
      case ESC_CHAR:
        if (!isRawString){
           closeTag(myState);
           eof=processEscapeCharState();
           openTag(myState);
           returnedFromOtherState=true;
        }
        break;
      case _EOF:
        eof = true;
        break;
      default:
        printMaskedToken();
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(myState);
  return eof;
}

/*
bool CodeGenerator::processTagState()
{
  State  newState=STANDARD;
  bool eof=false, exitState=false, returnedFromOtherState=false;
  unsigned int myKeywordClass=currentKeywordClass;

  openTag(KEYWORD);
  do {
    if (!returnedFromOtherState) {
       printMaskedToken(newState!=_WS);
    }
    returnedFromOtherState = false;
    newState= getCurrentState();
    // evil workaround to highlight tag content correctly:
    if (newState!=TAG_END && token==">") break;
    switch(newState)
      {
      case _WS:
        processWsState();
        exitState=true;
        returnedFromOtherState = true;
        break;
      case _EOL:
        insertLineNumber();
        exitState=true;
        break;

      case TAG_END:
        printMaskedToken();
        exitState=true;
        break;
      case STRING:
        closeTag(KEYWORD);
        eof=processStringState(KEYWORD);
        currentKeywordClass=myKeywordClass;
        openTag(KEYWORD);
        returnedFromOtherState = true;
        break;
      case ESC_CHAR:
        closeTag(KEYWORD);
        eof=processEscapeCharState();
        currentKeywordClass=myKeywordClass;
        openTag(KEYWORD);
        returnedFromOtherState = true;
        break;
      case NUMBER:
        closeTag(KEYWORD);
        eof=processNumberState();
        currentKeywordClass=myKeywordClass;
        openTag(KEYWORD);
        returnedFromOtherState = true;
        break;
      case _EOF:
        eof = true;
        break;
      default:
        printMaskedToken();
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(KEYWORD);
  currentKeywordClass=0;

  return eof;
}
*/

bool CodeGenerator::processSymbolState(){

  State newState=STANDARD;
  bool eof=false,
       exitState=false;

  openTag(SYMBOL);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();
    switch(newState)
      {
     case _WS:
        processWsState();
        break;
      case _EOL:
        insertLineNumber();
        exitState=true;
        break;
      case _EOF:
        eof = true;
        break;
      default:
        exitState=newState!=SYMBOL;
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(SYMBOL);
  return eof;
}

bool CodeGenerator::processEscapeCharState()
{
  State newState=STANDARD;
  bool eof=false, exitState=false;
  openTag(ESC_CHAR);
  do {
    printMaskedToken(newState!=_WS);
    newState= getCurrentState();
    switch(newState) {
      case _EOL:
        insertLineNumber();
        exitState=true;
        break;
      case _WS:
        processWsState();
        break;
      case _EOF:
        eof = true;
        break;
      default:
        exitState=newState!=ESC_CHAR;
        break;
      }
  } while ((!exitState) && (!eof));

  closeTag(ESC_CHAR);
  return eof;
}

void CodeGenerator::processWsState()
{
  if (!maskWs) {
    wsBuffer += token;
    token.clear();
    return;
  }
  flushWs();
  int cntWs=0;
  lineIndex--;

   while (iswspace(line[lineIndex])  ) {
     ++cntWs;
     ++lineIndex;
  }

  if (cntWs>1) {
    unsigned int styleID=getStyleID(currentState, currentKeywordClass);
    if (excludeWs && styleID!=_UNKNOWN) {
        *out << styleTagClose[styleID];
    }
    *out << maskWsBegin;
    for (int i=0; i<cntWs; i++){
        *out << spacer;
    }
    *out << maskWsEnd;
    if (excludeWs && styleID!=_UNKNOWN){
        *out << styleTagOpen[styleID];
    }
  } else {
    *out << spacer; //Bugfix fehlender Space nach Strings
  }
  token.clear();
}

void CodeGenerator::flushWs(){
   *out<<wsBuffer;
   wsBuffer.clear();
}

string CodeGenerator::getNewLine(){
  return newLineTag;
}

void CodeGenerator::insertLineNumber(bool insertNewLine) {
  if (insertNewLine){
    wsBuffer += getNewLine();
  }

  if (showLineNumbers) {
    ostringstream os;
    ostringstream numberPrefix;
    if (lineNumberFillZeroes) {
      os.fill('0');
    }
    os <<setw(getLineNumberWidth()) << right << lineNumber+lineNumberOffset;

    numberPrefix << styleTagOpen[LINENUMBER];
    maskString(numberPrefix, os.str());
    numberPrefix << spacer
                 << styleTagClose[LINENUMBER];

    wsBuffer += numberPrefix.str();
  }
}

unsigned int CodeGenerator::getLineIndex(){
  return lineIndex;
}

bool CodeGenerator::printExternalStyle(const string &outFile)
{
    if (!includeStyleDef && langInfo.getSyntaxHighlight()) {
        ostream *cssOutFile = (outFile.empty()? &cout :new ofstream (outFile.c_str()));
        if (!cssOutFile->fail()) {
            *cssOutFile << styleCommentOpen
                       <<" Style definition file generated by highlight "
                       << HIGHLIGHT_VERSION << ", " << HIGHLIGHT_URL
                       << " " << styleCommentClose << "\n";
            *cssOutFile << "\n" << styleCommentOpen
                       << " Highlighting theme definition: "
                       << styleCommentClose << "\n\n"
                       << getStyleDefinition()
                       << "\n";
            *cssOutFile << readUserStyleDef();
            if (!outFile.empty()) delete cssOutFile;
        } else {
            return false;
        }
    }
    return true;
}

string CodeGenerator::readUserStyleDef(){
    ostringstream ostr;
    if (!styleInputPath.empty()){
        ifstream userStyleDef(styleInputPath.c_str());
        if (userStyleDef) {
            ostr << "\n" << styleCommentOpen
                 << " Content of " << styleInputPath
                 << ": " <<styleCommentClose << "\n";
            string line;
            while (getline(userStyleDef, line)){
                ostr << line << "\n";
            }
            userStyleDef.close();
        } else {
            ostr << styleCommentOpen
                 << " ERROR: Could not include " << styleInputPath
                 << "." << styleCommentClose << "\n";
        }
    }
    return ostr.str();
}

/*
bool CodeGenerator::checkSpecialCmd(){
    bool insertNL = (lineIndex-token.length());
    //cerr << "token: "<<token<< " index"<< lineIndex << " "<<line [ lineIndex ]<<endl;

    if (line [ lineIndex ]=='!'){
      // find cmd
      size_t cmdPos1 = line.find('@', lineIndex+1);

      cerr << "cmdPos"<<cmdPos1<<endl;
      if(cmdPos1==string::npos) return false;
      size_t cmdPos2=cmdPos1+1;
      while (cmdPos2 < line.length() && StringTools::isAlpha(line[cmdPos2])) cmdPos2++;
      cerr << "cmdPos2"<<cmdPos2<<endl;
      cerr << line.substr(cmdPos1, cmdPos2)<<endl;

      // hide comment line
      token.clear();
      lineIndex=line.length();
      getInputChar(); lineNumber--;
      if (insertNL) {
        lineNumber++;
        insertLineNumber();
      }
      // end hide

      return true;
    }

    return false;
}*/

}
