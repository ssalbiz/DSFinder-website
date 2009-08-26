/***************************************************************************
                          languagedefinition.cpp  -  description
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

#include <memory>

#include "languagedefinition.h"
#include "stringtools.h"


using namespace std;

namespace highlight {

const string LanguageDefinition::REGEX_IDENTIFIER =
             "[a-zA-Z_]\\w*";

const string LanguageDefinition::REGEX_NUMBER =
             "(?:0x|0X)[0-9a-fA-F]+|\\d*[\\.]?\\d+(?:[eE][\\-\\+]\\d+)?[lLuU]?";

LanguageDefinition::LanguageDefinition():
    ignoreCase(false),
    disableHighlighting(false),
    allowExtEscape(false),
    vhdlMode(false),
    allowNestedComments(true),
    reformatCode(false)
{
  reDefPattern=Pattern::compile("^regex\\((.+?)(,\\s*(\\d+))?\\)$");
}

LanguageDefinition::~LanguageDefinition(){
  for (vector<RegexElement*>::iterator it=regex.begin(); it!=regex.end();it++){
    delete *it;
  }
  delete reDefPattern;
}

int LanguageDefinition::isKeyword(const string &s)
{
  return (s.length() && keywords.count(s)) ? keywords[s] : 0;
}

void LanguageDefinition::addSymbol(stringstream& symbolStream,
                                   State stateBegin,
                                   State stateEnd,
                                   bool isDelimiter,
                                   const string& paramValue,
                                   unsigned int classID ) {
  RegexDef re = extractRegex(paramValue);
  if (!re.reString.empty() ){
    Pattern* p = Pattern::compile(re.reString);
    if (p!=NULL)
      regex.push_back(new RegexElement(stateBegin,stateEnd, p, re.capturingGroup));
    else
      cerr << "highlight: Regex failure ( "<< re.reString <<" )\n";
    return;
  }
  if (isDelimiter) {
      addDelimiterSymbol(symbolStream, stateBegin, stateEnd, paramValue, classID);
  } else {
      addSimpleSymbol( symbolStream, stateBegin,paramValue );
  }
}

RegexDef LanguageDefinition::extractRegex(const string &paramValue){
    RegexDef re_def;
    auto_ptr<Matcher> m(reDefPattern->createMatcher(paramValue)); 
    if (m.get() && m->matches()) {
       re_def.reString = m->getGroup(1);
       if (m->getStartingIndex(3)!=-1) {
         StringTools::str2num<int>(re_def.capturingGroup, m->getGroup(3), std::dec);
         //std::cerr << "capturingGroup "<<re_def.capturingGroup<<"\n";
       }
    }
    return re_def;
}

void LanguageDefinition::addSimpleSymbol(stringstream& symbolStream,
                                        State state,
                                        const string& paramValue ) {
  istringstream valueStream(paramValue);
  bool valExists=false;
  string value;
  int pairCount =0;
   while (valueStream >> value)
    {
      symbolStream << " " << value;
      valExists = true;
      delimiterPair[value] = ++pairCount;
    }
  if (valExists)
    {
      symbolStream << " " << state;
    }
}

void LanguageDefinition::addDelimiterSymbol(stringstream& symbolStream,
                                        State stateBegin, State stateEnd,
                                        const string& paramValue,
                                        unsigned int classID) {
  istringstream valueStream(paramValue);
  string delimPrefix, delimSuffix;
  int pairCount =0;
  while (valueStream>>delimPrefix){
    valueStream >> delimSuffix;
    symbolStream << " "<<delimPrefix <<" " << stateBegin;
    symbolStream <<" "<< delimSuffix<<" "<< stateEnd;
    delimiterPrefixes.insert(make_pair(delimPrefix, classID));
    // if no closing delimiter exists, open and close delims are equal:
    delimiterDistinct[stateBegin] = !delimSuffix.empty();
    ++pairCount;
    delimiterPair[delimPrefix] = delimiterPair[delimSuffix] = pairCount;
    //std::cout << "pair: "<< delimPrefix<<"->"<<delimiterPair[delimPrefix] 
    //          <<", "<<delimSuffix<<"->"<<delimiterPair[delimSuffix]<<"\n";
  };
}

bool LanguageDefinition::getFlag( string& paramValue){
      return (StringTools::lowerCase(paramValue)=="true");
}

unsigned char LanguageDefinition::getSymbol(const string& paramValue){
      istringstream valueStream(paramValue);
      unsigned char symbol;
      valueStream >> symbol;
      return symbol;
}

void LanguageDefinition::addKeywords(const string &kwList,
                                     State stateBegin, State stateEnd,
                                     int classID){
  RegexDef re = extractRegex(kwList);
  if (!re.reString.empty()){
      Pattern* p = Pattern::compile(re.reString);
      if (p!=NULL)
        regex.push_back(new RegexElement(stateBegin,stateEnd, p, classID, re.capturingGroup));
      else
        cerr << "highlight: Regex compilation failure ("<< re.reString <<")\n";
     return;
  }
  istringstream valueStream(kwList);
  string keyword;
  while (valueStream >> keyword){
    keywords.insert(make_pair(keyword, classID));
  }
}

unsigned int LanguageDefinition::generateNewKWClass(const string& newClassName){
  unsigned int newClassID=0;
  bool found=false;
  while (newClassID<keywordClasses.size() && !found){
     found = (newClassName==keywordClasses[newClassID++]);
  }
  if (!found){
     newClassID++;
     keywordClasses.push_back(newClassName);
  }
  return newClassID;
}
/*
unsigned int LanguageDefinition::getDelimPrefixClassID(const string& prefix) {
  if (delimiterPrefixes.count(prefix)){
    return delimiterPrefixes[prefix];
  }
  return 0;
}
*/

bool LanguageDefinition::load(const string& langDefPath, bool clear)
{
	if (clear)  reset();
	
	ConfigurationReader langDef(langDefPath);
	if (!langDef.found()) {
		currentPath.clear();
		return false;
	}
	currentPath=langDefPath;
	disableHighlighting=false;
	string token;
	stringstream symbolStrStream;
	
	addSymbol(symbolStrStream,
		STRING,
		STRING_END,
		false,
		langDef.getParameter("stringdelimiters") );
	
	addSymbol(symbolStrStream,
		STRING,
		STRING_END,
		true,
		langDef.getParameter("string_unequal") );
	
	addSymbol(symbolStrStream,
		DIRECTIVE,
		DIRECTIVE_END,
		false,
		langDef.getParameter("directive") );
	
	addSymbol(symbolStrStream,
		ESC_CHAR,
		ESC_CHAR_END,
		false,
		langDef.getParameter("escchar") );
	
	addSymbol(symbolStrStream,
		SL_COMMENT,
		SL_COMMENT_END,
		false,
		langDef.getParameter("sl_comment") );
	
	addSymbol(symbolStrStream,
		ML_COMMENT,
		ML_COMMENT_END,
		true,
		langDef.getParameter("ml_comment") );
	
	addSymbol(symbolStrStream,
		SYMBOL,
		SYMBOL_END,
		false,
		langDef.getParameter("symbols") );
	
	string paramName, className, classValue;
	vector<string> paramNames=langDef.getParameterNames();
	for (unsigned int i=0;i<paramNames.size();i++){
		paramName=paramNames[i];
		className=StringTools::getParantheseVal(paramName);
		classValue=langDef.getParameter(paramName);
		if (paramName.find("kw_list") != string::npos ){
			addKeywords(classValue, KEYWORD, KEYWORD_END, generateNewKWClass(className));
		}
		if (paramName.find("kw_re") != string::npos ){
			addKeywords(classValue,  KEYWORD, KEYWORD_END, generateNewKWClass(className));
		}
		/*
		if (paramName.find("tag_delim") != string::npos ) {
			addSymbol(symbolStrStream, TAG_BEGIN, TAG_END, true,
			classValue, generateNewKWClass(className));
		}
		*/
	}
	
	// use hardcoded regex if not defined in language definition
	string user_def_re = extractRegex(langDef.getParameter("digit")).reString;
	string re_digit = (user_def_re.empty()) ? REGEX_NUMBER : user_def_re;
	
	user_def_re = extractRegex(langDef.getParameter("identifier")).reString;
	string re_identifier= (user_def_re.empty()) ? REGEX_IDENTIFIER: user_def_re;
	
	// insert identifier and number regex after keyword regexes
	regex.push_back(new RegexElement(IDENTIFIER_BEGIN, IDENTIFIER_END,
			Pattern::compile(re_identifier)));
	regex.push_back(new RegexElement(NUMBER, NUMBER_END,
			Pattern::compile(re_digit)));
	
	symbolString = symbolStrStream.str();
	
	ignoreCase = getFlag(langDef.getParameter("ignorecase"));
	allowNestedComments = getFlag(langDef.getParameter("allownestedcomments"));
	disableHighlighting = getFlag(langDef.getParameter("disablehighlighting"));
	reformatCode = getFlag(langDef.getParameter("reformatting"));
	rawStringPrefix = getSymbol(langDef.getParameter("rawstringprefix"));
	continuationChar = getSymbol(langDef.getParameter("continuationsymbol"));
	allowExtEscape = getFlag(langDef.getParameter("allowextescape"));
	vhdlMode= getFlag(langDef.getParameter("vhdl_mode"));
	
	langDesc = langDef.getParameter("description");
	
	string fileToInclude=langDef.getParameter("include");
	if (!fileToInclude.empty()){
		string::size_type Pos = langDefPath.find_last_of(Platform::pathSeparator);
		string includeLangDefPath = langDefPath.substr(0, Pos+1) + fileToInclude;
		load(includeLangDefPath, false);
	}
	return true;
}

void LanguageDefinition::reset()
{
  keywords.clear();
  keywordClasses.clear();
  delimiterPrefixes.clear();
  delimiterDistinct.clear();
  delimiterPair.clear();
  ignoreCase= false;
  allowNestedComments= reformatCode = false;
  rawStringPrefix = continuationChar = '\0';
  disableHighlighting=false;

  // TODO eigene methode
  for (vector<RegexElement*>::iterator it=regex.begin(); it!=regex.end();it++){
      delete *it;
  }
  regex.clear();
}

}
