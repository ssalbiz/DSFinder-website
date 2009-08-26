/***************************************************************************
                          gui.h  -  description
                             -------------------
    begin                : 06.08.02
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


#ifndef __gui_H__
#define __gui_H__

#ifdef WIN32
    #include <windows.h>
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifndef __WXMAC__
#include <wx/dnd.h>
#endif

#include <wx/dir.h>
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/cmdline.h>
#include <wx/utils.h>
#include <wx/tipdlg.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/combobox.h>
#include <wx/fileconf.h>
#include <wx/html/htmlwin.h>

#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>

#include "version.h"
#include "codegenerator.h"
#include "html32generator.h"
#include "configurationreader.h"
#include "datadir.h"
#include "enums.h"
#include "platform_fs.h"

#define MSG_INFO_TXT "Highlight GUI %s\n(C) 2002-2008 Andre Simon\
 <andre.simon1 at gmx.de>\n\nArtistic Style Classes\n(C) 1998-2007 Tal Davidson, Jim Pattee\
 <jimp03 at email.com>\
 \n\nRegex library\n(C) 2003-2006 Jeffery Stuart <stuart at cs.unr.edu>\
 \n\nBuilt with WxWidgets %d.%d.%d\n\nTranslators:\n\n\
%s\
\nReleased under the terms of the GNU GPL license.\
\n\nUpdates and source code: %s"

#define HIGHLIGHT_VERSION2 " Highlight "HIGHLIGHT_VERSION

#define CSS_DEFAULT_NAME "highlight.css"

#define NUMBER_FORMATS   6
#define NUMBER_ENCODINGS 15


/// Main application class of the graphical interface

class HighlightApp: public wxApp
{
  public:
      virtual bool OnInit();
};

typedef map<string, string> StringMap;

/// The main window frame
class HighlightFrame : public wxFrame
{
 public:
   HighlightFrame(const wxString &title);

  // Event handler (should *NOT* be virtual!)
   void OnQuit(wxCloseEvent &event);
   void OnQuit2(wxCommandEvent &event);

   void OnClickButtonSource(wxCommandEvent &event);
   void OnClickButtonTargetDir(wxCommandEvent &event);
   void OnClickButtonClear(wxCommandEvent &event);
   void OnClickButtonRemSelection(wxCommandEvent &event);
   void OnClickButtonOpenStyleFile(wxCommandEvent &event);
   void OnClickButtonStart(wxCommandEvent &event);
   void OnClickButtonClipboard(wxCommandEvent &event);
   void OnHelp(wxCommandEvent &event);
   void OnAbout(wxCommandEvent &event);
   void OnLicense(wxCommandEvent &event);
   void OnChangelog(wxCommandEvent &event);
   void OnTips (wxCommandEvent &event);
   void OnCheckLanguage(wxCommandEvent &event);
   void OnStyleOutfileChanged  (wxCommandEvent &event);
   void OnClickElement(wxCommandEvent &event);
   void OnClickSaveProject(wxCommandEvent &event);
   void OnClickOpenProject(wxCommandEvent &event);
   void OnClickDefaultProject(wxCommandEvent &event);
   void OnUpdatePreview(wxCommandEvent &event);

   void addInputFiles(const wxArrayString& inputFiles, const wxString& prefix,
        bool clear);
   void showStartupTips(bool forceTips=false);

   void showPopupMenu(const wxPoint & pos);

   bool getFatalError(){ return fatalError; }
   void setUserProfilePath(const char* path);

   void initInterface();

   void updatePreview();

 private:
    bool fatalError;
    bool writePreferences();
    bool loadLanguage(const wxString &lang);
    bool loadFileTypeConfig(StringMap* extMap, StringMap* shebangMap);
    bool loadProject(const wxString& projectFile);
    bool writeProject(const wxString& projectFile);
    void plausiChecks();
    void showTextFile(const wxString &file);

    string getOutFileSuffix();
    string getFileType(const string& suffix, const string &inputFile);
    string getFileSuffix(const string& fileName);
    string analyzeFile(const string& file);

    highlight::WrapMode getWrappingStyle();
    highlight::OutputType getOutputType();

    void setStyleInLabel();
    void assignToolTip(wxWindow* widget, const char* tipName);
    void fillChoiceCtrl(wxChoice* choice, const wxString &dir, const wxString &ext);
    void checkStyleReferences(wxTextCtrl *styleCtrl, bool webOutput);
    void applyCtrlValues(highlight::CodeGenerator* generator, bool previewMode=false);

    StringMap extensions;
    StringMap shebangs;

    wxString userPrefBaseDir;
    wxString preferencesPath;
    wxString defaultProjectPath;
    wxString currentProjectPath;
    wxString openFileDir, fileFilter;

    wxPanel    *panel;
    wxButton   *butSource, *butClearList, *butRemoveSelection;
    wxButton   *butOutDir;
    wxButton   *butStart;
    wxButton   *butClipboard;
    wxButton   *butChooseStyleInfile;
    wxListBox  *listBox ;
    wxChoice   *themeChoice,
               *reformatChoice,
               *caseChoice,
               *outputFormatChoice,
               *pageSizeChoice;
    wxComboBox *encodingChoice;
    wxTextCtrl *outDir,
               *styleOutFileName,
               *styleInFileName;
    wxCheckBox *includeStyle,
               *inlineCSS,
               *fragment,
               *enclosePreTag,
               *linenumbers,
               *attachAnchors,
               *useFileAsAnchorName,
               *reformatCode,
               *changeCase,
               *generateHtmlIndex,
               *wrapLines,
               *wrapLinesIntelligent,
               *replaceLatexQuotes,
               *enableLaTeXBabelCompat,
               *lnFillZeroes,
               *writeToSourceDir,
               *useEncoding,
               *orderedList;

    wxSpinCtrl   *tabWidth, *lineLength;
    wxStaticText *lblStyleOut, *lblColourTheme, *lblStyleIn, *lblOutDir,
                 *lblTabWidth, *lblLineLength, *lblOutputFormat, *lblPageSize;
    wxMenu       *menuFile, *menuHelp, *menuLang, *menuPrj;
    wxMenuBar    *MenuBar;

    wxHtmlWindow* preview;
    wxStaticText* lblPreview;

    bool showTips;
    int lastTip;

    wxGauge *progress;
    wxString language;
    wxFileConfig *fcStrings;

    DataDir dataDir;

    // Any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

   enum
   {
    ID_MENU_ABOUT = wxID_ABOUT,    // wegen MacOS
    PROGRAM_QUIT = wxID_EXIT,

    ID_MENU_OPENFILES = wxID_HIGHEST+1,
    ID_BUTTON_OPENFILES,
    ID_BUTTON_OPEN_CSS_INFILE,
    ID_MENU_HELP,
    ID_MENU_CHANGELOG,
    ID_MENU_LICENSE,
    ID_MENU_TIPS,
    ID_MENU_PRJ_LOAD,
    ID_MENU_PRJ_SAVE,
    ID_MENU_PRJ_LOAD_DEF,
    ID_BUTTON_OUTDIR,
    ID_LISTBOX,
    ID_OUTDIRTEXT,
    ID_INCLUDECSS,
    ID_INLINECSS,
    ID_START,
    ID_INCLUDELINENUMBERS,
    ID_FRAGMENT,
    ID_ENCLOSEPRETAG,
    ID_STYLE,
    ID_BUTTON_CLEAR,
    ID_PROGRESS,
    ID_THEMECHOICE,
    ID_ATTACHANCHORS,
    ID_USEFILEASANCHORNAME,
    ID_CSS_INFILE,
    ID_CSS_OUTFILE,
    ID_REFORMATCODE,
    ID_REFORMATCHOICE,
    ID_CHANGECASE,
    ID_CASECHOICE,
    ID_GENERATEINDEX,
    ID_WRAP,
    ID_WRAP_CAUTIOUS,
    ID_REPLACE_QUOTES,
    ID_BABEL_COMPAT,
    ID_TAB_WIDTH,
    ID_FILL_ZEROES,
    ID_WRITE_TO_SRCDIR,
    ID_PANEL,
    ID_BUTTON_REMOVE,
    ID_USE_ENCODING,
    ID_ENCODING_LIST,
    ID_FORMAT_CHOICE,
    ID_ORDERED_LIST,
    ID_RICHTEXT_CTRL,
    ID_LINE_LEN,
    ID_CLIPBRD,
    // This id is used to calculate menu ids at runtime:
    ID_MENU_LANGUAGE_DYN = wxID_HIGHEST + 100
    };

BEGIN_EVENT_TABLE(HighlightFrame, wxFrame)
   EVT_MENU(PROGRAM_QUIT, HighlightFrame::OnQuit2)
   EVT_CLOSE(HighlightFrame::OnQuit)
   EVT_MENU(ID_MENU_OPENFILES, HighlightFrame::OnClickButtonSource)
   EVT_MENU(ID_MENU_HELP, HighlightFrame::OnHelp)
   EVT_MENU(ID_MENU_ABOUT, HighlightFrame::OnAbout)
   EVT_MENU(ID_MENU_LICENSE, HighlightFrame::OnLicense)
   EVT_MENU(ID_MENU_CHANGELOG, HighlightFrame::OnChangelog)
   EVT_MENU(ID_MENU_TIPS, HighlightFrame::OnTips)
   EVT_MENU(ID_MENU_PRJ_SAVE, HighlightFrame::OnClickSaveProject)
   EVT_MENU(ID_MENU_PRJ_LOAD, HighlightFrame::OnClickOpenProject)
   EVT_MENU(ID_MENU_PRJ_LOAD_DEF, HighlightFrame::OnClickDefaultProject)
   EVT_BUTTON(ID_BUTTON_OPENFILES, HighlightFrame::OnClickButtonSource)
   EVT_BUTTON(ID_BUTTON_OUTDIR, HighlightFrame::OnClickButtonTargetDir)
   EVT_BUTTON(ID_START, HighlightFrame::OnClickButtonStart)
   EVT_BUTTON(ID_CLIPBRD, HighlightFrame::OnClickButtonClipboard)
   EVT_BUTTON(ID_BUTTON_OPEN_CSS_INFILE, HighlightFrame::OnClickButtonOpenStyleFile)
   EVT_BUTTON(ID_BUTTON_CLEAR, HighlightFrame::OnClickButtonClear)
   EVT_BUTTON(ID_BUTTON_REMOVE , HighlightFrame::OnClickButtonRemSelection)
   EVT_CHECKBOX(ID_INCLUDECSS, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_INLINECSS, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_INCLUDELINENUMBERS, HighlightFrame::OnClickElement)
   EVT_CHOICE(ID_FORMAT_CHOICE, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_REFORMATCODE, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_CHANGECASE, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_WRAP, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_WRAP_CAUTIOUS, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_FRAGMENT, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_WRITE_TO_SRCDIR, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_USE_ENCODING, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_ORDERED_LIST, HighlightFrame::OnClickElement)
   EVT_CHECKBOX(ID_ATTACHANCHORS, HighlightFrame::OnClickElement)
   EVT_TEXT(ID_CSS_OUTFILE, HighlightFrame::OnStyleOutfileChanged)

   EVT_LISTBOX(ID_LISTBOX, HighlightFrame::OnUpdatePreview)
   EVT_CHOICE(ID_THEMECHOICE, HighlightFrame::OnUpdatePreview)
   EVT_CHOICE(ID_REFORMATCHOICE, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_REFORMATCODE, HighlightFrame::OnUpdatePreview)
   EVT_CHOICE(ID_CASECHOICE, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_CHANGECASE, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_INCLUDELINENUMBERS, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_FILL_ZEROES, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_WRAP, HighlightFrame::OnUpdatePreview)
   EVT_CHECKBOX(ID_WRAP_CAUTIOUS, HighlightFrame::OnUpdatePreview)
   EVT_TEXT(ID_TAB_WIDTH, HighlightFrame::OnUpdatePreview)
//   EVT_TEXT(ID_LINE_LEN, HighlightFrame::OnUpdatePreview)

END_EVENT_TABLE()

IMPLEMENT_APP(HighlightApp)

#ifndef __WXMAC__
/* Drag and Drop Class*/
class DropFiles: public wxFileDropTarget {
public:
    DropFiles (HighlightFrame *frame) {m_frame = frame;}
    virtual bool OnDropFiles (wxCoord x, wxCoord y,
                              const wxArrayString& filenames);
private:
    HighlightFrame *m_frame;
};
#endif


#endif

