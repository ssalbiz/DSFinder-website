/***************************************************************************
                          cmdlineoptions.h  -  description
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

#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#ifdef _WIN32
  #include <windows.h>
#endif

// If your system does not know getopt_long, define USE_LOCAL_GETOPT
#if defined(_WIN32) || defined(__SVR4) || defined(__sun__)
  // some compilers don't like redefinitions...
  #ifndef USE_LOCAL_GETOPT
     #define USE_LOCAL_GETOPT
  #endif
#endif

#ifdef USE_LOCAL_GETOPT
  #include "getopt.h"
#else
  #include <getopt.h>
#endif

#include <string>
#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "enums.h"

#define OPT_VERBOSE      "verbose"
#define OPT_INC_STYLE    "include-style"
#define OPT_HELP         "help"
#define OPT_LINENO       "linenumbers"
#define OPT_HTML         "html"
#define OPT_XHTML        "xhtml"
#define OPT_RTF          "rtf"
#define OPT_TEX          "tex"
#define OPT_LATEX        "latex"
#define OPT_FRAGMENT     "fragment"
#define OPT_ANCHORS      "anchors"
#define OPT_ANCHOR_PFX   "anchor-prefix"
#define OPT_ANCHOR_FN    "anchor-filename"
#define OPT_LISTTHEMES   "list-themes"
#define OPT_LISTLANGS    "list-langs"
#define OPT_VERSION      "version"
#define OPT_IN           "input"
#define OPT_OUT          "output"
#define OPT_SYNTAX       "syntax"
#define OPT_STYLE        "style"
#define OPT_STYLE_OUT    "style-outfile"
#define OPT_STYLE_IN     "style-infile"
#define OPT_INLINE_CSS   "inline-css"
#define OPT_ENCLOSE_PRE  "enclose-pre"

#define OPT_DELTABS      "replace-tabs"
#define OPT_BATCHREC     "batch-recursive"
#define OPT_OUTDIR       "outdir"
#define OPT_FORMAT       "reformat"
#define OPT_DATADIR      "data-dir"
#define OPT_ADDDATADIR   "add-data-dir"
#define OPT_ADDCONFDIR   "add-config-dir"
#define OPT_INDEXFILE    "print-index"
#define OPT_HELP_LANG    "help-lang"
#define OPT_WRAP         "wrap"
#define OPT_WRAPSIMPLE   "wrap-simple"
#define OPT_QUIET        "quiet"
#define OPT_REPLACE_QUOTES  "replace-quotes"
#define OPT_PROGRESSBAR  "progress"
#define OPT_FILLZEROES   "zeroes"
#define OPT_ANSI         "ansi"
#define OPT_XML          "xml"
#define OPT_ENCODING     "encoding"
#define OPT_FORCE_OUTPUT "force"
#define OPT_ORDERED_LIST "ordered-list"
#define OPT_LNR_LEN      "line-number-length"
#define OPT_LINE_LEN     "line-length"
#define OPT_BASE_FONT    "font"
#define OPT_BASE_FONT_SIZE "font-size"
#define OPT_LNR_START    "line-number-start"
#define OPT_BABEL        "babel"
#define OPT_PAGE_SIZE    "page-size"
#define OPT_PRINT_CONFIG "print-config"
#define OPT_DOC_TITLE    "doc-title"
#define OPT_XTERM256     "xterm256"
#define OPT_CLASSNAME    "class-name"
#define OPT_KW_CASE      "kw-case"
#define OPT_MARK_LINES   "mark-line"
#define OPT_TEST_INPUT   "validate-input"

#define S_OPT_ANSI       'A'
#define S_OPT_OUT        'o'
#define S_OPT_IN         'i'
#define S_OPT_SYNTAX     'S'
#define S_OPT_VERBOSE    'v'
#define S_OPT_INC_STYLE  'I'
#define S_OPT_HELP       'h'
#define S_OPT_LINENO     'l'
#define S_OPT_STYLE      's'
#define S_OPT_STYLE_OUT  'c'
#define S_OPT_STYLE_IN   'e'
#define S_OPT_DELTABS    't'
#define S_OPT_HTML       'H'
#define S_OPT_XHTML      'X'
#define S_OPT_RTF        'R'
#define S_OPT_TEX        'T'
#define S_OPT_LATEX      'L'
#define S_OPT_XML        'Z'
#define S_OPT_BATCHREC   'B'
#define S_OPT_FRAGMENT   'f'
#define S_OPT_ANCHORS    'a'
#define S_OPT_ANCHOR_PFX 'y'
#define S_OPT_ANCHOR_FN  'N'
#define S_OPT_LISTTHEMES 'w'
#define S_OPT_LISTLANGS  'p'
#define S_OPT_OUTDIR     'O'

#define S_OPT_FORMAT         'F'
#define S_OPT_DATADIR        'D'
#define S_OPT_ADDDATADIR     'E'
#define S_OPT_INDEXFILE      'C'
#define S_OPT_WRAP           'W'
#define S_OPT_WRAPSIMPLE     'V'
#define S_OPT_QUIET          'q'
#define S_OPT_REPLACE_QUOTES 'r'
#define S_OPT_VERSION        'Q'
#define S_OPT_PROGRESSBAR    'P'
#define S_OPT_FILLZEROES     'z'
#define S_OPT_ENCODING       'u'
#define S_OPT_LNR_LEN        'j'
#define S_OPT_LINE_LEN       'J'
#define S_OPT_LNR_START      'm'
#define S_OPT_BASE_FONT      'k'
#define S_OPT_BASE_FONT_SIZE 'K'
#define S_OPT_ORDERED_LIST   'n'
#define S_OPT_BABEL          'b'
#define S_OPT_PAGE_SIZE      'x'
#define S_OPT_DOC_TITLE      'd'
#define S_OPT_XTERM256       'M'
#define S_OPT_CLASSNAME      'G'

#define S_OPTIONS_STRING "abfhlnvwpqrzACHILMNYRTZXUV::W::Pk:K:j:x:o:i:S:B:O:s:c:e:t:u:m:F:D:E:d:y:J:G:"

using namespace std;

/// handle command line options

class CmdLineOptions
  {
  public:

    /**Constructor
     \param argc Argument count
     \param argv Argument strings
    */
    CmdLineOptions(int argc, char *argv[]);
    ~CmdLineOptions();

    /** \return Single output file name*/
    const string &getSingleOutFilename();

    /** \return Single input file name*/
    const string &getSingleInFilename() const;

    /** \return Output directory*/
    const string& getOutDirectory() ;

    /** \return Style output file name*/
    const string getStyleOutFilename() const;

    /** \return Style input file name*/
    const string& getStyleInFilename() const;

    /** \return Char set*/
    const string& getEncoding() const;

    /** \return Number of spaces to replace a tab*/
    int getNumberSpaces() const;

    /** \return True if version information should be printed*/
    bool printVersion() const;

   /** \return True if help information should be printed*/
    bool printHelp() const;

    /** \return True if debug information should be printed*/
    bool printDebugInfo()const;

    /** \return True if configuration information should be printed*/
    bool printConfigInfo()const;

    /** \return True if Style definition should be included in output*/
    bool includeStyleDef() const;

    /** \return True if line numbers should be printed*/
    bool printLineNumbers() const;

    /** \return colour theme name */
    string getThemeName()const ;

    /** gibt true zurck, falls deutsche Hilfe ausgegeben werden soll */
    int helpLanguage() const;

    /** \return True if batch mode is active*/
    bool enableBatchMode() const;

    /** \return True if output shluld be fragmented*/
    bool fragmentOutput() const;

    /** \return output file suffix */
    string getOutFileSuffix() const;

    /** \return True if anchors should be attached to line numbers*/
    bool attachLineAnchors() const;

    /** \return True if list of installed themes should be printed*/
    bool showThemes() const;

    /** \return True if list of installed language definitions should be printed*/
    bool showLangdefs() const;

    /** \return True if loutput directory is given*/
    bool outDirGiven() const;

    /** \return True if refomatting is enabled*/
    bool formattingEnabled();

    /** \return True if a new data directory is given*/
    bool dataDirGiven()const;

    /** \return True if an additional data directory is given*/
    bool additionalDataDirGiven()const;

    /** \return True if index file should be printed*/
    bool printIndexFile() const;

    /** \return True if quotes should be replaced by /dq in LaTeX*/
    bool replaceQuotes() const;

    /** \return True if shorthands of LaTeX Babel package should be disabled*/
    bool disableBabelShorthands() const;

    /** \return True if input file name should be used as anchor name */
    bool useFNamesAsAnchors() const;

    /** \return Data directory*/
    const string &getDataDir()const;

    /** \return Additional data directory*/
    const string &getAdditionalDataDir()const;

    /** \return Additional config data directory*/
    const string &getAdditionalConfDir()const;

    /** \return  path of user config file*/
    const string &getConfigFilePath() const;

    /** \return True if language syntax is given*/
    bool syntaxGiven() const;

    /** \return True if quiet mode is active*/
    bool quietMode() const;

    /** \return True if progress bar should be printed in batch mode */
    bool printProgress() const;

    /** \return True if line numbers are filled with leading zeroes */
    bool fillLineNrZeroes() const;

    /** \return name of help message file*/
    const string getHelpLang() const;

    /** \return programming syntax */
    const string &getLanguage()const ;

    /** \return Wrapping style*/
    highlight::WrapMode getWrappingStyle() const;

    /** \return List of input file names*/
    const vector <string> & getInputFileNames() const;

    /** \return Map of marked lines*/
    const map <int,string> &getMarkLines();

    /** \return Name of indentation scheme file */
    const string &getIndentScheme() const;

    /** \return RTF page size */
    const string &getPageSize() const;

    /** \return Output file format */
    highlight::OutputType getOutputType() const;

    /** \return True if chosen output format supports referenced style files */
    bool formatSupportsExtStyle();

    /** \return True if style output path was defined by user*/
    bool styleOutPathDefined() const{
        return opt_stylepath_explicit;
    }

    /** \return True if encoding specification should be omitted in output*/
    bool omitEncoding() const;

    /** \return True if output should be generated if languege type is unknown*/
    bool forceOutput() const;

    /** \return True if line numbers should be replaced by ordered list (HTML) */
    bool orderedList() const;

    /** \return True if a base font has been given */
    bool hasBaseFont() const ;

    /** \return True if input should be validated */
    bool validateInput() const ;

    /** \return True if CSS should be outputted within tag elements */
    bool inlineCSS() const ;

    /** \return True if fragmented html output should be enclosed with pre tags */
    bool enclosePreTag() const ;

    /** \return The given base font, empty string by default */
    const string& getBaseFont() const ;

    /** \return Document title */
    const string& getDocumentTitle() const ;

    /** \return anchor prefix */
    const string& getAnchorPrefix() const ;

    /** \return class name */
    const string& getClassName() const ;

    /** \return True if a base font size has been given */
    bool hasBaseFontSize() const ;

    /** \return The given base font size, empty string by default */
    const string& getBaseFontSize() const ;

    /** \return line number width */
    int getNumberWidth();

    /** \return line length */
    int getLineLength();

    /** \return Line number start count */
    int getNumberStart();

    /** \return Keyword Case (upper, lower, unchanged) */
    highlight::KeywordCase getKeywordCase() const;

  private:

    int numberSpaces;   // number of spaces which replace a tab
    int lineNrWidth;    // width of line number (left padding)
    int lineLength;    // length of line before wrapping
    int lineNrStart;    // line number start count
    highlight::WrapMode wrappingStyle; // line wrapping mode
    highlight::OutputType outputType;
    highlight::KeywordCase keywordCase;

    // name of single output file
    string outFilename,
    // output directory
    outDirectory,
    // programming syntax which will be loaded
    syntax,
    // name of colour theme
    styleName,
    // name of external style file
    styleOutFilename,
    // name of file to be included in external style file
    styleInFilename,
    // used to define data directories at runtime
    dataDir, additionalDataDir, additionalConfigDir;
    // name of indenation scheme
    string indentScheme,
           pageSize;

    string baseFont, baseFontSize;
    string docTitle, className;
    string markLinesArg;

    bool opt_syntax;
    bool opt_include_style;
    bool opt_help;
    bool opt_version ;
    bool opt_verbose;
    bool opt_print_config;
    bool opt_linenumbers;
    bool opt_style;
    bool opt_batch_mode;
    bool opt_fragment;
    bool opt_attach_line_anchors;
    bool opt_show_themes;
    bool opt_show_langdefs;
    bool opt_asformat_output;
    bool opt_printindex;
    bool opt_quiet;
    bool opt_replacequotes;
    bool opt_babel;
    bool opt_print_progress;
    bool opt_fill_zeroes;
    bool opt_stylepath_explicit;
    bool opt_force_output;
    bool opt_ordered_list;
    bool opt_fnames_as_anchors;
    bool opt_validate;
    bool opt_inlineCSS;
    bool opt_enclosePreTag;

    bool configFileRead;

    string anchorPrefix;

    string helpLang, encodingName;
    string configFilePath;

    /** list of all input file names */
    vector <string> inputFileNames;

    /** list lines which should be marked and supplied with help string */
    map <int, string> markLines;

    /** load highlight configuration file */
    void loadConfigurationFile();

    /** \return file suffix */
    string getFileSuffix( const string & fileName) const;

    /** \return directory name of path */
    string getDirName( const string & path);

    /** get all entries in the directory defined by wildcard */
    void readDirectory(const string & wildcard);

    /** \return Boolean value of paramVal */
    bool getFlag(const string& paramVal);

    /** \return Valid path name */
    string validateDirPath(const string & path);
  };

#endif
