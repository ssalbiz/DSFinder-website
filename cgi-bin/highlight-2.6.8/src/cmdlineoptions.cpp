/***************************************************************************
                          cmdlineoptions.cpp  -  description
                             -------------------
    begin                : Sun Nov 25 2001
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

#include "cmdlineoptions.h"
#include "platform_fs.h"
#include "configurationreader.h"
#include "stringtools.h"
#include "datadir.h"
#include <sstream>

using namespace std;

/* Siehe man getopt (3)
   Konstruktor legt Optionen und Argumente fest
*/
CmdLineOptions::CmdLineOptions(int argc, char *argv[]):
    numberSpaces(0),
    lineNrWidth(5),
    lineLength(80),
    lineNrStart(1),
    wrappingStyle(highlight::WRAP_DISABLED),
    outputType (highlight::HTML),
    keywordCase (highlight::CASE_UNCHANGED),
    className("hl"),
    opt_syntax (false),
    opt_include_style (false),
    opt_help (false),
    opt_version (false),
    opt_verbose (false),
    opt_print_config(false),
    opt_linenumbers (false),
    opt_style (false),
    opt_batch_mode (false),
    opt_fragment (false) ,
    opt_attach_line_anchors (false),
    opt_show_themes (false),
    opt_show_langdefs (false),
    opt_printindex(false),
    opt_quiet(false),
    opt_replacequotes(false),
    opt_babel(false),
    opt_print_progress(false),
    opt_fill_zeroes(false),
    opt_stylepath_explicit(false),
    opt_force_output(false),
    opt_ordered_list(false),
    opt_fnames_as_anchors(false),
    opt_validate(false),
    opt_inlineCSS(false),
    opt_enclosePreTag(false),
    configFileRead(false),
    anchorPrefix("l"),
    helpLang("en"),
    encodingName("ISO-8859-1")
{

  loadConfigurationFile();

  int c, option_index = 0;
  static struct option long_options[] =
      {
        {OPT_OUT, 1, 0, S_OPT_OUT},
        {OPT_IN, 1, 0, S_OPT_IN},
        {OPT_SYNTAX, 1, 0, S_OPT_SYNTAX},
        {OPT_VERBOSE, 0, 0, S_OPT_VERBOSE},
        {OPT_INC_STYLE, 0, 0, S_OPT_INC_STYLE},
        {OPT_HELP, 0, 0, S_OPT_HELP},
        {OPT_HELP_LANG, 1, 0, 0},
        {OPT_LINENO,0,0,S_OPT_LINENO},
        {OPT_STYLE, 1,0,S_OPT_STYLE},
        {OPT_STYLE_OUT, 1, 0,S_OPT_STYLE_OUT},
        {OPT_STYLE_IN, 1, 0,S_OPT_STYLE_IN},
        {OPT_DELTABS,1,0,S_OPT_DELTABS},
        {OPT_HTML, 0,0,S_OPT_HTML},
        {OPT_XHTML, 0,0,S_OPT_XHTML},
        {OPT_RTF, 0,0,S_OPT_RTF},
        {OPT_TEX,0, 0,S_OPT_TEX},
        {OPT_LATEX,0, 0,S_OPT_LATEX},
        {OPT_ANSI,0, 0,S_OPT_ANSI},
        {OPT_XML,0, 0,S_OPT_XML},
        {OPT_XTERM256,0, 0,S_OPT_XTERM256},
        {OPT_BATCHREC,1,0,S_OPT_BATCHREC},
        {OPT_FRAGMENT,0,0,S_OPT_FRAGMENT},
        {OPT_ANCHORS, 0,0,S_OPT_ANCHORS },
        {OPT_LISTTHEMES, 0,0,S_OPT_LISTTHEMES },
        {OPT_LISTLANGS, 0,0,S_OPT_LISTLANGS },
        {OPT_OUTDIR,1,0,S_OPT_OUTDIR},
        {OPT_VERSION,0,0,0},
        {OPT_FORMAT,1,0,S_OPT_FORMAT},
        {OPT_DATADIR,1,0,S_OPT_DATADIR},
        {OPT_ADDDATADIR,1,0,S_OPT_ADDDATADIR},
        {OPT_INDEXFILE,0,0,S_OPT_INDEXFILE},
        {OPT_WRAP,0,0,S_OPT_WRAP},
        {OPT_WRAPSIMPLE,0,0,S_OPT_WRAPSIMPLE},
        {OPT_QUIET,0,0,S_OPT_QUIET},
        {OPT_REPLACE_QUOTES,0,0,S_OPT_REPLACE_QUOTES},
        {OPT_PROGRESSBAR,0,0,S_OPT_PROGRESSBAR},
        {OPT_FILLZEROES,0,0,S_OPT_FILLZEROES},
        {OPT_ENCODING,1,0,S_OPT_ENCODING},
        {OPT_BASE_FONT,1,0,S_OPT_BASE_FONT},
        {OPT_BASE_FONT_SIZE,1,0,S_OPT_BASE_FONT_SIZE},
        {OPT_ORDERED_LIST,0,0,S_OPT_ORDERED_LIST},
        {OPT_LNR_LEN,1,0,S_OPT_LNR_LEN},
        {OPT_LINE_LEN,1,0,S_OPT_LINE_LEN},
        {OPT_LNR_START,1,0,S_OPT_LNR_START},
        {OPT_BABEL,0,0,S_OPT_BABEL},
        {OPT_PAGE_SIZE,1,0,S_OPT_PAGE_SIZE},
        {OPT_DOC_TITLE,1,0,S_OPT_DOC_TITLE},
        {OPT_PRINT_CONFIG,0,0,0},
        {OPT_ANCHOR_PFX,1,0,S_OPT_ANCHOR_PFX},
        {OPT_ANCHOR_FN,0,0,S_OPT_ANCHOR_FN},
        {OPT_CLASSNAME,1,0,S_OPT_CLASSNAME},
        {OPT_KW_CASE,1,0,0},
        {OPT_MARK_LINES,1,0,0},
        {OPT_FORCE_OUTPUT,0,0,0},
        {OPT_TEST_INPUT,0,0,0},
        {OPT_INLINE_CSS,0,0,0},
        {OPT_ENCLOSE_PRE,0,0,0},
        {OPT_ADDCONFDIR,1,0,0},
        {0, 0, 0, 0}
      };

  while (1)
    {
      c = getopt_long (argc, argv,S_OPTIONS_STRING,long_options, &option_index);
      if (c == -1)
        break;

      switch (c)
        {
        case 0:   // long options
          if (long_options[option_index].name==OPT_VERSION) {
              opt_version = true;
            }
            else if (long_options[option_index].name==OPT_FORCE_OUTPUT) {
                opt_force_output = true;
            }
            else if (long_options[option_index].name==OPT_PRINT_CONFIG) {
               opt_print_config = true;
            }
            else if (long_options[option_index].name==OPT_HELP_LANG){
		helpLang = string(optarg);
		opt_help = true;
	    }
            else if (long_options[option_index].name==OPT_KW_CASE){
		if (StringTools::lowerCase(  string(optarg))=="upper")
			keywordCase = highlight::CASE_UPPER;
		else if (StringTools::lowerCase(  string(optarg))=="lower")
			keywordCase = highlight::CASE_LOWER;
		else if (StringTools::lowerCase(  string(optarg))=="capitalize")
			keywordCase = highlight::CASE_CAPITALIZE;
	    }
            else if (long_options[option_index].name== OPT_MARK_LINES) {
		markLinesArg=string(optarg);
            }
            else if (long_options[option_index].name== OPT_TEST_INPUT) {
		opt_validate=true;
            }
            else if (long_options[option_index].name== OPT_INLINE_CSS) {
		opt_inlineCSS=true;
            }
            else if (long_options[option_index].name== OPT_ENCLOSE_PRE) {
		opt_enclosePreTag=true;
            }
            else if (long_options[option_index].name== OPT_ADDCONFDIR) {
		additionalConfigDir=string(optarg);
            }
          break;
        case S_OPT_OUT:
          outFilename=string(optarg);
          break;
        case S_OPT_IN:
          inputFileNames.push_back(string(optarg));
          break;
        case S_OPT_STYLE_OUT:
          styleOutFilename=string(optarg);
          opt_stylepath_explicit=true;
          break;
        case S_OPT_STYLE_IN:
          styleInFilename=string(optarg);
          break;
        case S_OPT_PAGE_SIZE:
          pageSize=string(optarg);
          break;
        case S_OPT_VERBOSE:
          opt_verbose = true;
          break;
        case S_OPT_QUIET:
          opt_quiet = true;
          break;
         case S_OPT_INC_STYLE:
          opt_include_style = true;
          break;
        case S_OPT_HELP:
          opt_help = true;
          break;
        case S_OPT_LINENO:
          opt_linenumbers = true;
          break;
        case '?':
          //opt_help = true;
          break;
        case S_OPT_STYLE:
          styleName=string(optarg);
          opt_style = true;
          break;
        case S_OPT_SYNTAX:
          syntax=string(optarg);
          opt_syntax = true;
          break;
        case S_OPT_DELTABS:
          StringTools::str2num<int> (numberSpaces, string(optarg), std::dec);
          break;
        case S_OPT_HTML:
          outputType=highlight::HTML;
          break;
        case S_OPT_XHTML:
          outputType=highlight::XHTML;
          break;
        case S_OPT_RTF:
          outputType=highlight::RTF;
          break;
        case S_OPT_TEX:
          outputType=highlight::TEX;
          break;
        case S_OPT_LATEX:
          outputType=highlight::LATEX;
          break;
        case S_OPT_XML:
          outputType=highlight::XML;
          break;
        case S_OPT_ANSI:
          outputType=highlight::ANSI;
          break;
        case S_OPT_XTERM256:
          outputType=highlight::XTERM256;
          break;
        case S_OPT_BATCHREC:
          opt_batch_mode = true;
          readDirectory(string(optarg));
          break;
        case S_OPT_FRAGMENT:
          opt_fragment = true;
          break;
        case S_OPT_ANCHORS:
          opt_attach_line_anchors = true;
          break;
        case S_OPT_ANCHOR_PFX:
          anchorPrefix =string(optarg);
          break;
        case S_OPT_LISTTHEMES:
          opt_show_themes = true;
          break;
        case S_OPT_LISTLANGS:
          opt_show_langdefs = true;
          break;
        case S_OPT_OUTDIR:
          outDirectory = validateDirPath(string(optarg));
          break;
        case S_OPT_FORMAT:
          indentScheme =string(optarg);
          break;
        case S_OPT_ENCODING:
          encodingName =string(optarg);
          break;
        case S_OPT_DATADIR:
          dataDir=validateDirPath(string(optarg));
          break;
        case S_OPT_ADDDATADIR:
          additionalDataDir=validateDirPath(string(optarg));
          break;
         case S_OPT_INDEXFILE:
          opt_printindex=true;
          break;
         case S_OPT_WRAPSIMPLE:
          wrappingStyle = highlight::WRAP_SIMPLE;
          break;
         case S_OPT_WRAP:
          wrappingStyle = highlight::WRAP_DEFAULT;
          break;
         case S_OPT_REPLACE_QUOTES:
          opt_replacequotes=true;
          break;
         case S_OPT_BABEL:
          opt_babel=true;
          break;
         case S_OPT_PROGRESSBAR:
          opt_print_progress=true;
          break;
         case S_OPT_FILLZEROES:
          opt_fill_zeroes=true;
          break;
         case S_OPT_BASE_FONT:
          baseFont=string(optarg);
          break;
         case S_OPT_BASE_FONT_SIZE:
          baseFontSize=string(optarg);
          break;
         case S_OPT_LNR_LEN:
          StringTools::str2num<int>(lineNrWidth, string(optarg), std::dec);
          break;
         case S_OPT_LINE_LEN:
          StringTools::str2num<int>(lineLength, string(optarg), std::dec);
          break;
         case S_OPT_LNR_START:
          StringTools::str2num<int>(lineNrStart, string(optarg), std::dec);
          break;
         case S_OPT_ORDERED_LIST:
          opt_ordered_list = opt_linenumbers = true;
          break;
        case S_OPT_DOC_TITLE:
          docTitle = string(optarg);
          break;
        case S_OPT_ANCHOR_FN:
            opt_fnames_as_anchors=true;
            break;
        case S_OPT_CLASSNAME:
          className =string(optarg);
          break;
        default:
          cerr <<"higlight: Unknown option " <<c<< endl;
        }
    }

  if (optind < argc)   //still args left
    {
      if  (inputFileNames.empty()) {
        while (optind < argc){
          inputFileNames.push_back(string(argv[optind++]));
         }
      }
    } else if (inputFileNames.empty()) {
       inputFileNames.push_back("");
    }
  if (printDebugInfo() && configFileRead) {
    cout << "Configuration file \""<<configFilePath<<"\" was read.\n";
  }
}

CmdLineOptions::~CmdLineOptions(){}

const string &CmdLineOptions::getSingleOutFilename()
  {
   if (!inputFileNames.empty() && !outDirectory.empty()) {
      if (outFilename.empty()) {
        outFilename = outDirectory;
        int delim = getSingleInFilename().find_last_of(Platform::pathSeparator)+1;
        outFilename += getSingleInFilename().substr((delim>-1)?delim:0)
                       + getOutFileSuffix();
      }
   }
   return outFilename;
  }

const string &CmdLineOptions::getSingleInFilename()  const
  {
  return inputFileNames[0];
  }

const string &CmdLineOptions::getOutDirectory()
  {
    if (!outFilename.empty() && !enableBatchMode()){
      outDirectory=getDirName(outFilename);
    }
    return outDirectory;
  }

const string CmdLineOptions::getStyleOutFilename() const
  {
      if (!styleOutFilename.empty()) return styleOutFilename;
      return (outputType==highlight::HTML ||
              outputType==highlight::XHTML)? "highlight.css":"highlight.sty";
  }
const string &CmdLineOptions::getStyleInFilename() const
  {
    return styleInFilename;
  }
int CmdLineOptions::getNumberSpaces() const
  {
    return numberSpaces;
  }
bool CmdLineOptions::printVersion()const
  {
    return opt_version;
  }
bool CmdLineOptions::printHelp()const
  {
    return opt_help;
  }
bool CmdLineOptions::printDebugInfo()const
  {
    return opt_verbose;
  }
bool CmdLineOptions::printConfigInfo()const
  {
    return opt_print_config;
  }
bool CmdLineOptions::quietMode()const
  {
    return opt_quiet;
  }
bool CmdLineOptions::includeStyleDef()const
  {
      return opt_include_style;
  }
bool CmdLineOptions::useFNamesAsAnchors() const
{
        return opt_fnames_as_anchors;
}

bool CmdLineOptions::formatSupportsExtStyle(){
      return outputType==highlight::HTML ||
             outputType==highlight::XHTML ||
             outputType==highlight::LATEX ||
             outputType==highlight::TEX;
}

bool CmdLineOptions::printLineNumbers()const
  {
    return opt_linenumbers;
  }

string CmdLineOptions::getThemeName()const
  {
    return ( ( opt_style) ? styleName+".style" : "kwrite.style" );
  }
bool CmdLineOptions::enableBatchMode()const{
    return inputFileNames.size()>1 || opt_batch_mode;
}
bool CmdLineOptions::fragmentOutput()const{
    return opt_fragment;
}
string CmdLineOptions::getOutFileSuffix()const{
    switch (outputType){
      case highlight::XHTML: return ".xhtml";
      case highlight::RTF:   return ".rtf";
      case highlight::TEX:
      case highlight::LATEX: return ".tex";
      case highlight::XML:   return ".xml";
      case highlight::ANSI:  return ".ansi";
      case highlight::XTERM256: return ".xterm";
      default:    return ".html";
    }
}
string CmdLineOptions::getDirName(const string & path) {
  size_t dirNameLength=path.rfind(Platform::pathSeparator);
  return (dirNameLength==string::npos)?string():path.substr(0, dirNameLength+1);
}
bool CmdLineOptions::attachLineAnchors()const{
    return opt_attach_line_anchors;
}
bool CmdLineOptions::showThemes()const{
    return opt_show_themes;
}
bool CmdLineOptions::showLangdefs()const{
    return opt_show_langdefs;
}
bool CmdLineOptions::outDirGiven()const{
    return !outFilename.empty();
}
bool CmdLineOptions::replaceQuotes() const {
   return opt_replacequotes;
}
bool CmdLineOptions::disableBabelShorthands() const {
  return opt_babel;
}
bool CmdLineOptions::getFlag( const string& paramVal){
   return StringTools::lowerCase(paramVal)=="true";
}
bool CmdLineOptions::formattingEnabled(){
    return !indentScheme.empty();
}
bool CmdLineOptions::orderedList() const {
  return opt_ordered_list;
}
const string &CmdLineOptions::getDataDir() const {
 return dataDir;
}
const string &CmdLineOptions::getIndentScheme() const {
 return indentScheme;
}
const string &CmdLineOptions::getAdditionalDataDir()const{
 return additionalDataDir;
}
const string &CmdLineOptions::getAdditionalConfDir()const{
 return additionalConfigDir;
}
const string &CmdLineOptions::getLanguage() const {
 return syntax;
}
const string&CmdLineOptions::getEncoding() const{
    return encodingName;
}

const string& CmdLineOptions::getAnchorPrefix() const {
    return anchorPrefix;
}

const string &CmdLineOptions::getPageSize() const{
   return pageSize;
}
bool CmdLineOptions::printIndexFile() const{
 return opt_printindex && (outputType==highlight::HTML ||
                           outputType==highlight::XHTML);
}
bool CmdLineOptions::printProgress() const{
 return opt_print_progress;
}
bool CmdLineOptions::fillLineNrZeroes() const {
  return opt_fill_zeroes;
}
bool CmdLineOptions::syntaxGiven() const {
  return opt_syntax;
}
bool CmdLineOptions::omitEncoding() const{
    return StringTools::lowerCase(encodingName)=="none";
}
bool CmdLineOptions::forceOutput() const {
    return opt_force_output;
}
bool CmdLineOptions::validateInput() const {
    return opt_validate;
}
bool CmdLineOptions::inlineCSS() const {
    return opt_inlineCSS;
}
bool CmdLineOptions::enclosePreTag() const {
    return opt_enclosePreTag;
}
const string  CmdLineOptions::getHelpLang() const {
  return helpLang+".help";
}
const string &CmdLineOptions::getConfigFilePath() const {
  return configFilePath;
}

const string& CmdLineOptions::getDocumentTitle() const {
   return docTitle;
}

highlight::WrapMode CmdLineOptions::getWrappingStyle() const {
  return wrappingStyle;
}
const vector <string> & CmdLineOptions::getInputFileNames() const{
  return inputFileNames;
}

const map <int,string> & CmdLineOptions::getMarkLines() {
	markLines.clear();
	istringstream valueStream;
	string elem;
	size_t delimPos;
	int markLineNo;
	valueStream.str(markLinesArg);
	// Format: "1=help line one; 3=help line three; 5 "
	while (getline( valueStream, elem, ';')){
		delimPos = elem.find('=');
		markLineNo=0;
		StringTools::str2num<int> (markLineNo, elem.substr(0,delimPos), std::dec);
		if (markLineNo) {
			markLines[markLineNo] = 
			(delimPos!=string::npos)?elem.substr(delimPos+1):"";
		}
	}
	return markLines;
}
void CmdLineOptions::readDirectory(const string & wildcard){
  // get matching files, use  recursive search
  bool directoryOK=Platform::getDirectoryEntries(inputFileNames, wildcard, true);
  if (!directoryOK)
    {
       cerr << "highlight: No files matched the pattern \""
            << wildcard << "\"."<< endl;
    }
}

string CmdLineOptions::validateDirPath(const string & path){
   return (path[path.length()-1] !=Platform::pathSeparator)?
              path+Platform::pathSeparator : path;
}

highlight::OutputType CmdLineOptions::getOutputType() const {
    return outputType;
}

highlight::KeywordCase CmdLineOptions::getKeywordCase() const {
    return keywordCase;
}

bool CmdLineOptions::hasBaseFont() const {
   return (! baseFont.empty()) ;
}

const string& CmdLineOptions::getBaseFont() const {
   return baseFont ;
}

bool CmdLineOptions::hasBaseFontSize() const {
   return (! baseFontSize.empty()) ;
}

const string& CmdLineOptions::getBaseFontSize() const {
   return baseFontSize ;
}

const string& CmdLineOptions::getClassName() const {
   return className ;
}

int CmdLineOptions::getNumberWidth(){
   return lineNrWidth;
}

int CmdLineOptions::getLineLength(){
   return lineLength;
}

int CmdLineOptions::getNumberStart(){
   return lineNrStart;
}

void CmdLineOptions::loadConfigurationFile()
{
  #ifndef _WIN32
    #ifdef CONFIG_FILE_PATH
       configFilePath=CONFIG_FILE_PATH;
    #else
      char* homeEnv=getenv("HOME");
      if (homeEnv==NULL) return;
      configFilePath=string(homeEnv)+"/.highlightrc";
    #endif
  #else
    configFilePath = Platform::getAppPath() + "highlight.conf";
  #endif
  ConfigurationReader userConfig(configFilePath);

  if (userConfig.found())
    {
      string paramVal;
      configFileRead=true;

      styleOutFilename = userConfig.getParameter(OPT_STYLE_OUT);
      styleInFilename = userConfig.getParameter(OPT_STYLE_IN);
      styleName = userConfig.getParameter(OPT_STYLE);
      opt_style = !styleName.empty();
      syntax = userConfig.getParameter(OPT_SYNTAX);
      opt_syntax = !syntax.empty();
      StringTools::str2num<int>(numberSpaces, userConfig.getParameter(OPT_DELTABS), std::dec);
      indentScheme = userConfig.getParameter(OPT_FORMAT);
      baseFontSize = userConfig.getParameter(OPT_BASE_FONT_SIZE);
      baseFont = userConfig.getParameter(OPT_BASE_FONT);

      paramVal = userConfig.getParameter(OPT_DATADIR);
      if (!paramVal.empty()) {
         dataDir=validateDirPath( paramVal);
      }
      paramVal = userConfig.getParameter(OPT_ADDDATADIR);
      if (!paramVal.empty()) {
        additionalDataDir=validateDirPath(paramVal);
      }
      paramVal = userConfig.getParameter(OPT_ADDCONFDIR);
      if (!paramVal.empty()) {
        additionalConfigDir=validateDirPath(paramVal);
      }
      paramVal = userConfig.getParameter(OPT_OUTDIR);
      if (!paramVal.empty()) {
        outDirectory=validateDirPath(paramVal);
      }
      paramVal = userConfig.getParameter(OPT_ENCODING);
      if (!paramVal.empty()) {
          encodingName=paramVal;
      }
      paramVal = userConfig.getParameter(OPT_LNR_LEN);
      if (!paramVal.empty()) {
          StringTools::str2num<int>(lineNrWidth, string(paramVal), std::dec);
      }
      paramVal = userConfig.getParameter(OPT_LNR_START);
      if (!paramVal.empty()) {
          StringTools::str2num<int>(lineNrStart, string(paramVal), std::dec);
      }
      paramVal = userConfig.getParameter(OPT_ANCHOR_PFX);
      if (!paramVal.empty()) {
          anchorPrefix=paramVal;
      }

      opt_include_style=getFlag(userConfig.getParameter(OPT_INC_STYLE));
      opt_verbose=getFlag(userConfig.getParameter(OPT_VERBOSE));
      opt_ordered_list=getFlag(userConfig.getParameter(OPT_ORDERED_LIST));
      opt_linenumbers=opt_ordered_list || getFlag(userConfig.getParameter(OPT_LINENO));
      opt_fragment=getFlag(userConfig.getParameter(OPT_FRAGMENT));
      opt_attach_line_anchors=getFlag(userConfig.getParameter(OPT_ANCHORS));
      opt_printindex=getFlag(userConfig.getParameter(OPT_INDEXFILE));
      opt_quiet=getFlag(userConfig.getParameter(OPT_QUIET));
      opt_replacequotes=getFlag(userConfig.getParameter(OPT_REPLACE_QUOTES));
      opt_print_progress=getFlag(userConfig.getParameter(OPT_PROGRESSBAR));
      opt_fill_zeroes=getFlag(userConfig.getParameter(OPT_FILLZEROES));
      opt_fnames_as_anchors=getFlag(userConfig.getParameter(OPT_ANCHOR_FN));
      opt_validate=getFlag(userConfig.getParameter(OPT_TEST_INPUT));
      opt_fnames_as_anchors=getFlag(userConfig.getParameter(OPT_ANCHOR_FN));
      opt_inlineCSS=getFlag(userConfig.getParameter(OPT_INLINE_CSS));

      if (getFlag(userConfig.getParameter(OPT_WRAP))) {
          wrappingStyle=highlight::WRAP_DEFAULT;
      }
      if (getFlag(userConfig.getParameter(OPT_WRAPSIMPLE))) {
          wrappingStyle=highlight::WRAP_SIMPLE;
      }
      if (getFlag(userConfig.getParameter(OPT_XHTML))) {
          outputType=highlight::XHTML;
      } else if (getFlag(userConfig.getParameter(OPT_RTF))) {
          outputType=highlight::RTF;
      } else if (getFlag(userConfig.getParameter(OPT_TEX))) {
          outputType=highlight::TEX;
      } else if (getFlag(userConfig.getParameter(OPT_LATEX))) {
          outputType=highlight::LATEX;
      } else if (getFlag(userConfig.getParameter(OPT_ANSI))) {
          outputType=highlight::ANSI;
      } else if (getFlag(userConfig.getParameter(OPT_XML))) {
          outputType=highlight::XML;
      } else if (getFlag(userConfig.getParameter(OPT_XTERM256))) {
          outputType=highlight::XTERM256;
      }
    }
}

