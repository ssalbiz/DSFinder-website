/***************************************************************************
                          gui.cpp  -  description
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

#include "gui.h"
#include <wx/version.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/ffile.h>
#include <wx/fileconf.h>
#include <wx/clipbrd.h>

#include "gui_failure_dlg.h"
#include "gui_showfile_dlg.h"
#include "highlight.xpm"

#define MAX_LINE__WIDTH       80

using namespace std;

// Main Application

bool HighlightApp::OnInit()
{
   const wxCmdLineEntryDesc cmdLineDesc[] ={
    { wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, wxT("u"), wxT("user-profile"),  wxT("user profile directory"),
        wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_OPTIONAL },
    {wxCMD_LINE_NONE}
   };


   wxCmdLineParser parser(argc, argv);
   parser.SetDesc(cmdLineDesc);
   parser.Parse();

   HighlightFrame *win = new HighlightFrame(HIGHLIGHT_VERSION2);

   wxString userProfileDir;
   if (parser.Found(wxT("user-profile"), &userProfileDir)){
        win->setUserProfilePath(userProfileDir.c_str());
   } else {
        win->setUserProfilePath(wxStandardPathsBase::Get().GetUserDataDir().c_str());
   }

   win->initInterface();

   if (win->getFatalError()) return false;

   if (parser.GetParamCount()){
     wxArrayString inputFileList;
     for (unsigned int i=0;i<parser.GetParamCount();i++){
       inputFileList.Add(parser.GetParam(i));
     }
     win->addInputFiles(inputFileList, wxT(""), true);
   }
   win->Show(true);
   SetTopWindow(win);
   win->showStartupTips();

   /* Do not return FALSE or the app will terminate immediately */
   return true;
}

void HighlightFrame::setUserProfilePath(const char* path){
      userPrefBaseDir=string(path);
}

// Main Window
HighlightFrame::HighlightFrame(const wxString &title)
    : wxFrame((wxFrame *) NULL, -1, title, wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_FRAME_STYLE),
     styleOutFileName(NULL),
     language(wxT("English")),
     fcStrings(NULL)
{
	dataDir.searchDataDir("");
}

void HighlightFrame::initInterface(){

   preferencesPath=userPrefBaseDir+Platform::pathSeparator+"highlight.conf";
   defaultProjectPath=userPrefBaseDir+Platform::pathSeparator+"highlight.hpr";

   currentProjectPath=defaultProjectPath;

   wxString lastProject;
   wxFileConfig projectConfig ("Highlight", wxEmptyString, preferencesPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
   projectConfig.Read("language",&language, wxT("English"));
   projectConfig.Read("showTips",&showTips, true);
   projectConfig.Read("lastTip",&lastTip, 1);
   projectConfig.Read("lastProject",&lastProject,"");

   int x=0, y=0, width=0, height=0;
   projectConfig.Read(wxT("winXPos"),&x, 0);
   projectConfig.Read(wxT("winYPos"),&y, 0);
   projectConfig.Read(wxT("winWidth"),&width, 0);
   projectConfig.Read(wxT("winHeight"),&height, 0);

   if (!lastProject.empty() && lastProject != defaultProjectPath)
          currentProjectPath=lastProject;

   fatalError=!loadLanguage(language);

   SetIcon(wxIcon(highlight_xpm));

   menuFile = new wxMenu(),
   menuHelp = new wxMenu();
   menuLang = new wxMenu();
   menuPrj = new wxMenu();

   MenuBar = new wxMenuBar();
   menuFile->Append(ID_MENU_OPENFILES,fcStrings->Read(wxT("m001"), wxEmptyString));
   menuFile->AppendSeparator();
   menuFile->Append(PROGRAM_QUIT,fcStrings->Read(wxT("m002"), wxEmptyString));
   menuHelp->Append(ID_MENU_HELP, fcStrings->Read(wxT("m003"), wxEmptyString));
   menuHelp->Append(ID_MENU_TIPS, fcStrings->Read(wxT("m009"), wxEmptyString));
   menuHelp->AppendSeparator();
   menuHelp->Append(ID_MENU_LICENSE, fcStrings->Read(wxT("m004"), wxEmptyString));
   menuHelp->Append(ID_MENU_CHANGELOG, fcStrings->Read(wxT("m005"), wxEmptyString));
   menuHelp->AppendSeparator();
   menuHelp->Append(ID_MENU_ABOUT, fcStrings->Read(wxT("m006"), wxEmptyString));
   menuPrj->Append(ID_MENU_PRJ_LOAD, fcStrings->Read(wxT("m011"), wxEmptyString));
   menuPrj->Append(ID_MENU_PRJ_SAVE, fcStrings->Read(wxT("m012"), wxEmptyString));
   menuPrj->AppendSeparator();
   menuPrj->Append(ID_MENU_PRJ_LOAD_DEF, fcStrings->Read(wxT("m013"), wxEmptyString));


   wxFileName i18nFileName;
   wxArrayString i18nFiles;
   wxDir::GetAllFiles(wxString(dataDir.getI18nDir().c_str()),
                      &i18nFiles, wxString(wxT("*.txt")), (int)wxDIR_FILES);
   i18nFiles.Sort();
   if ( i18nFiles.IsEmpty()) {
       wxMessageBox(wxString(wxT("i18n folder not found. Check installation.")),
                    wxString(wxT("Error.")),
                    wxICON_EXCLAMATION);
       return;
   }
   // apart from English.txt, more translation files exist
   bool translationsAvailable=i18nFiles.GetCount()>1;
   if (translationsAvailable){
     for (unsigned int i=0; i<i18nFiles.GetCount(); i++){
       Connect(ID_MENU_LANGUAGE_DYN + i,  -1, wxEVT_COMMAND_MENU_SELECTED,
               (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
               &HighlightFrame::OnCheckLanguage);
       i18nFileName.Assign(wxString(i18nFiles[i].c_str()));
       menuLang->AppendRadioItem(ID_MENU_LANGUAGE_DYN + i, i18nFileName.GetName());
     }
     menuLang->Check(menuLang->FindItem(wxString(language.c_str())), true);
  }

   MenuBar->Append(menuFile, fcStrings->Read(wxT("m007"), wxEmptyString));
   MenuBar->Append(menuPrj, fcStrings->Read(wxT("m010"), wxEmptyString));
   MenuBar->Append(menuLang, fcStrings->Read(wxT("m008"), wxEmptyString));
   MenuBar->Append(menuHelp, fcStrings->Read(wxT("m003"), wxEmptyString));

   SetMenuBar(MenuBar);
   MenuBar->EnableTop(1, translationsAvailable);

   panel =new wxPanel(this, ID_PANEL);
   butSource = new wxButton(panel, ID_BUTTON_OPENFILES,
                            fcStrings->Read(wxT("l001"), wxEmptyString),
                            wxDefaultPosition, wxDefaultSize, 0 );

   listBox = new wxListBox(panel, ID_LISTBOX, wxDefaultPosition,
                           wxSize(200,300), 0, NULL,
                           wxLB_NEEDED_SB |wxLB_EXTENDED /*| wxLB_MULTIPLE*/ | wxLB_HSCROLL);

   butRemoveSelection=new wxButton(panel, ID_BUTTON_REMOVE,
                                    fcStrings->Read(wxT("l033"), wxEmptyString),
                                    wxDefaultPosition, wxDefaultSize, 0 );

   butClearList = new wxButton(panel, ID_BUTTON_CLEAR,
                                    fcStrings->Read(wxT("l003"), wxEmptyString),
                                    wxDefaultPosition, wxDefaultSize, 0 );

   // Spanish out directory label is very long, split in 2 lines
   wxString outDirLbl=wxString::Format(fcStrings->Read(wxT("l002"), wxEmptyString),wxT("\n"));
   lblOutDir = new wxStaticText(panel, -1, outDirLbl);

   outDir = new  wxTextCtrl(panel, ID_OUTDIRTEXT, wxT(""));

   butOutDir = new wxButton( panel, ID_BUTTON_OUTDIR,wxT( "...") , wxDefaultPosition,
                             wxSize(25,-1), 0 );

   writeToSourceDir= new wxCheckBox(panel, ID_WRITE_TO_SRCDIR,
                                 fcStrings->Read(wxT("l032"), wxEmptyString));

   lblOutputFormat = new wxStaticText(panel, -1, fcStrings->Read(wxT("l004"), wxEmptyString));

   wxString formatNames[] = {wxT("HTML"),wxT("XHTML"), wxT("LaTeX"),wxT("TeX"), wxT("RTF"), wxT("XML")};
   outputFormatChoice=new wxChoice(panel, ID_FORMAT_CHOICE, wxDefaultPosition,
                              wxDefaultSize, wxArrayString(NUMBER_FORMATS, formatNames));
   outputFormatChoice->SetSelection(0);

   linenumbers = new wxCheckBox(panel, ID_INCLUDELINENUMBERS,
                                fcStrings->Read(wxT("l013"), wxEmptyString));

   attachAnchors= new wxCheckBox(panel, ID_ATTACHANCHORS,
                                 fcStrings->Read(wxT("l006"), wxEmptyString));

   useFileAsAnchorName= new wxCheckBox(panel, ID_USEFILEASANCHORNAME,
                                 fcStrings->Read(wxT("l042"), wxEmptyString));


   orderedList= new wxCheckBox(panel, ID_ORDERED_LIST,
                                 fcStrings->Read(wxT("l037"), wxEmptyString));

   lnFillZeroes = new wxCheckBox(panel, ID_FILL_ZEROES,
                                 fcStrings->Read(wxT("l031"), wxEmptyString));

   wrapLines= new wxCheckBox(panel, ID_WRAP,
                                 fcStrings->Read(wxT("l027"), wxEmptyString));

   wrapLinesIntelligent= new wxCheckBox(panel, ID_WRAP_CAUTIOUS,
                                 fcStrings->Read(wxT("l028"), wxEmptyString));

   fragment = new wxCheckBox(panel, ID_FRAGMENT,
                             fcStrings->Read(wxT("l014"), wxEmptyString));

   enclosePreTag = new wxCheckBox(panel, ID_ENCLOSEPRETAG,
                             fcStrings->Read(wxT("l050"), "Enclose in <pre>"));

   includeStyle = new wxCheckBox(panel, ID_INCLUDECSS,
                               fcStrings->Read(wxT("l005"), wxEmptyString));
   inlineCSS = new wxCheckBox(panel, ID_INLINECSS,
                               fcStrings->Read(wxT("l048"), "Inline CSS"));

   generateHtmlIndex=new wxCheckBox(panel, ID_GENERATEINDEX,
                                    fcStrings->Read(wxT("l008"), wxEmptyString));

   replaceLatexQuotes=new wxCheckBox(panel, ID_REPLACE_QUOTES,
                                    fcStrings->Read(wxT("l029"), wxEmptyString));
   enableLaTeXBabelCompat=new wxCheckBox(panel, ID_BABEL_COMPAT,
                                    fcStrings->Read(wxT("l038"), wxEmptyString));

   lblColourTheme= new wxStaticText(panel, -1, fcStrings->Read(wxT("l010"), wxEmptyString));

   themeChoice=new wxChoice(panel, ID_THEMECHOICE, wxDefaultPosition,
                             wxSize(100,-1), 0, 0, wxCB_SORT);

   reformatCode= new wxCheckBox(panel, ID_REFORMATCODE,
                                fcStrings->Read(wxT("l007"), wxEmptyString));

    changeCase= new wxCheckBox(panel, ID_CHANGECASE,
                               fcStrings->Read(wxT("l047"), wxEmptyString));

   reformatChoice=new wxChoice(panel, ID_REFORMATCHOICE, wxDefaultPosition,
                             wxSize(100,-1), 0, 0, wxCB_SORT);


   lblTabWidth=new wxStaticText(panel, ID_TAB_WIDTH, fcStrings->Read(wxT("l030"), wxEmptyString));
   tabWidth= new wxSpinCtrl(panel, ID_TAB_WIDTH,wxT("4"), wxDefaultPosition,
                            wxSize(60,-1), wxSP_ARROW_KEYS, 0, 8 , 4);

   lblLineLength=new wxStaticText(panel, ID_LINE_LEN, fcStrings->Read(wxT("l046"), wxEmptyString));
   lineLength= new wxSpinCtrl(panel, ID_LINE_LEN,wxT("80"), wxDefaultPosition,
                            wxSize(60,-1), wxSP_ARROW_KEYS, 60, 160, 80 );

   useEncoding= new wxCheckBox(panel, ID_USE_ENCODING,
                                fcStrings->Read(wxT("l036"), wxEmptyString));

   wxString encodingNames[] = { wxT("ISO-8859-1"), wxT("ISO-8859-2"), wxT("ISO-8859-3"),
                                wxT("ISO-8859-4"), wxT("ISO-8859-5"), wxT("ISO-8859-6"),
                                wxT("ISO-8859-7"), wxT("ISO-8859-8"), wxT("ISO-8859-9"),
                                wxT("ISO-8859-10"), wxT("ISO-8859-11"), wxT("ISO-8859-13"),
                                wxT("ISO-8859-14"), wxT("ISO-8859-15"), wxT("UTF-8")};

   encodingChoice=new wxComboBox(panel, ID_ENCODING_LIST, wxT(""), wxDefaultPosition,
                                 wxSize(100,-1),
                                 wxArrayString (NUMBER_ENCODINGS, encodingNames),
                                 wxCB_DROPDOWN );
   encodingChoice->SetSelection(0);

   wxString caseNames[] = { wxT("UPPER"), wxT("lower"),wxT("Capitalize") };
   caseChoice=new wxChoice(panel, ID_CASECHOICE, wxDefaultPosition,
                                 wxSize(100,-1),
                                 wxArrayString (3, caseNames));
   caseChoice->SetSelection(0);

   lblPageSize= new wxStaticText(panel, -1, fcStrings->Read(wxT("l039"), wxEmptyString));
   wxString pageSizes[] = {wxT("A3"),wxT("A4"),wxT("A5"),wxT("B4"), wxT("B5"), wxT("B6"),
   wxT("Letter"), wxT("Legal")};
   pageSizeChoice=new wxChoice(panel, ID_FORMAT_CHOICE, wxDefaultPosition,
                            wxSize(100,-1), wxArrayString(8, pageSizes));
   pageSizeChoice->SetSelection(1);

   wxString str=wxString::Format(wxT("%s %s"), fcStrings->Read(wxT("l011"), wxEmptyString).c_str(),
                   ((styleOutFileName!=NULL)?(styleOutFileName->GetValue()).c_str():
                      wxT(CSS_DEFAULT_NAME)));

   lblStyleIn= new wxStaticText(panel, -1,str);
   styleInFileName = new wxTextCtrl(panel, ID_CSS_INFILE, wxT(""));
   butChooseStyleInfile = new wxButton( panel, ID_BUTTON_OPEN_CSS_INFILE, wxT("..."),
                                     wxDefaultPosition,  wxSize(25,-1), 0);

   lblStyleOut= new wxStaticText(panel, -1, fcStrings->Read(wxT("l009"), wxEmptyString));

   styleOutFileName = new wxTextCtrl(panel, ID_CSS_OUTFILE, CSS_DEFAULT_NAME);

   butStart = new wxButton(panel, ID_START, fcStrings->Read(wxT("l015"), wxEmptyString),
                           wxDefaultPosition, wxDefaultSize, 0 );
   butStart->SetFont(wxFont(11, wxDEFAULT , wxNORMAL,wxBOLD));

   butClipboard = new wxButton(panel, ID_CLIPBRD, fcStrings->Read(wxT("l049"), "Clipboard"),
                           wxDefaultPosition, wxDefaultSize, 0 );

   progress= new wxGauge(panel, ID_PROGRESS, 100, wxDefaultPosition,
                         wxDefaultSize,wxGA_SMOOTH );

  // Layout ***************************************************************

  wxBoxSizer *leftsizer = new wxBoxSizer( wxVERTICAL );
  leftsizer->Add( butSource, 0, wxTOP | wxGROW | wxALIGN_CENTER_HORIZONTAL, 5 );
  leftsizer->Add(
    listBox,
    1,            // make vertically stretchable
    wxGROW |    // make horizontally stretchable
    wxTOP|wxBOTTOM,        //   and make border all around
    10 );         // set border width to 10

  leftsizer->Add( butRemoveSelection, 0, wxTOP | wxGROW | wxALIGN_CENTER_HORIZONTAL, 5 );
  leftsizer->Add(butClearList, 0, wxTOP | wxGROW | wxALIGN_CENTER_HORIZONTAL, 5 );
  //leftsizer->Add(0,5);

   wxStaticBox *outDirBox= new wxStaticBox(panel, -1, wxT("") );
   wxStaticBoxSizer *outDirBoxSizer = new wxStaticBoxSizer( outDirBox, wxVERTICAL );
   wxBoxSizer* outDirSelSizer = new wxBoxSizer(wxHORIZONTAL);
   outDirSelSizer->Add( outDir, 1, wxALIGN_CENTRE|wxALL|wxGROW , 5 );
   outDirSelSizer->Add( butOutDir, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 0 );
   outDirBoxSizer->Add( lblOutDir, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );
   outDirBoxSizer->Add( outDirSelSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM|wxGROW, 5);
   outDirBoxSizer->Add( writeToSourceDir, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
   leftsizer->Add(outDirBoxSizer,0, wxALIGN_CENTER_HORIZONTAL | wxTOP |wxGROW  | wxBOTTOM, 5);


    wxBoxSizer *rightsizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *formatChoiceBox= new wxStaticBox(panel, -1, wxT(""));
    wxStaticBoxSizer *formatChoiceBoxSizer = new wxStaticBoxSizer( formatChoiceBox, wxVERTICAL );

    wxBoxSizer *formatSizer = new wxBoxSizer(wxHORIZONTAL );
    formatSizer->Add( lblOutputFormat, 0,  wxTOP | wxALIGN_CENTER_VERTICAL , 3 );
    formatSizer->Add(5, 0, 1);
    formatSizer->Add( outputFormatChoice, 0,  wxTOP , 5 );
    formatChoiceBoxSizer->Add( formatSizer, 0,  wxTOP| wxBOTTOM |wxGROW );

    rightsizer->Add(formatChoiceBoxSizer,0, wxGROW|wxTOP, 0);

    wxStaticBox *optBox= new wxStaticBox(panel, -1,wxT(""));
    wxStaticBoxSizer *optBoxSizer = new wxStaticBoxSizer( optBox, wxVERTICAL );
    optBoxSizer->Add(0, 5, 1);
    optBoxSizer->Add(linenumbers, 0, wxALIGN_LEFT );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( attachAnchors, 0, wxALIGN_LEFT | wxLEFT, 10 );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( useFileAsAnchorName, 0, wxALIGN_LEFT | wxLEFT, 10 );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( orderedList, 0, wxALIGN_LEFT | wxLEFT, 10 );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( lnFillZeroes, 0,  wxALIGN_LEFT | wxLEFT, 10 );
#ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( wrapLines, 0,   wxALIGN_LEFT );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( wrapLinesIntelligent, 0,  wxALIGN_LEFT | wxLEFT, 10 );
#ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( fragment, 0, wxALIGN_LEFT );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( enclosePreTag, 0,  wxALIGN_LEFT | wxLEFT, 10 );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( includeStyle, 0,  wxALIGN_LEFT);
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( inlineCSS, 0, wxALIGN_LEFT | wxLEFT, 10 );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( generateHtmlIndex, 0,   wxBOTTOM | wxALIGN_LEFT );
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( replaceLatexQuotes, 0,   wxALIGN_LEFT);
    #ifdef WIN32
    optBoxSizer->AddSpacer(4);
    #endif
    optBoxSizer->Add( enableLaTeXBabelCompat, 0, wxBOTTOM| wxALIGN_LEFT );
    rightsizer->Add(optBoxSizer,0, wxGROW);

    wxStaticBox *choicesBox= new wxStaticBox(panel, -1, wxT(""));
    wxStaticBoxSizer *choicesBoxSizer = new wxStaticBoxSizer( choicesBox, wxVERTICAL );

    wxBoxSizer *themeSizer = new wxBoxSizer(wxHORIZONTAL );
    themeSizer->Add( lblColourTheme, 0,  wxTOP | wxALIGN_CENTER_VERTICAL , 3 );
    themeSizer->Add(5, 0, 1);
    themeSizer->Add( themeChoice, 0,  wxTOP , 5 );
    choicesBoxSizer->Add( themeSizer, 0, wxBOTTOM |wxGROW );

    wxBoxSizer *reformatSizer = new wxBoxSizer(wxHORIZONTAL );
    reformatSizer->Add(reformatCode,0, wxTOP | wxALIGN_CENTER_VERTICAL, 5);
    reformatSizer->Add(5, 0, 1);
    reformatSizer->Add(reformatChoice,0, wxTOP, 5);
    choicesBoxSizer->Add( reformatSizer, 0, wxGROW);

    wxBoxSizer *caseSizer = new wxBoxSizer(wxHORIZONTAL );
    caseSizer->Add(changeCase,0, wxTOP | wxALIGN_CENTER_VERTICAL, 5);
    caseSizer->Add(5, 0, 1);
    caseSizer->Add(caseChoice,0, wxTOP, 5);
    choicesBoxSizer->Add( caseSizer, 0, wxGROW);

    wxBoxSizer *tabSizer = new wxBoxSizer(wxHORIZONTAL );
    tabSizer->Add(lblTabWidth, 0, wxTOP|wxALIGN_CENTER_VERTICAL , 5);
    tabSizer->Add(5, 0, 1);
    tabSizer->Add(tabWidth, 0, wxTOP, 3);
    wxBoxSizer *lenSizer = new wxBoxSizer(wxHORIZONTAL );
    lenSizer->Add(lblLineLength, 0, wxTOP|wxALIGN_CENTER_VERTICAL , 5);
    lenSizer->Add(5, 0, 1);
    lenSizer->Add(lineLength, 0, wxTOP, 3);
    choicesBoxSizer->Add( tabSizer, 0, wxTOP| wxBOTTOM| wxGROW);
    choicesBoxSizer->Add( lenSizer, 0, wxTOP| wxBOTTOM| wxGROW);

    wxBoxSizer *encodingSizer = new wxBoxSizer(wxHORIZONTAL );
    encodingSizer->Add(useEncoding,0, wxTOP | wxALIGN_CENTER_VERTICAL, 5);
    encodingSizer->Add(5, 0, 1);
    encodingSizer->Add(encodingChoice,0, wxTOP, 5);
    choicesBoxSizer->Add( encodingSizer, 0,  wxTOP |wxGROW );

    wxBoxSizer *pageSizeSizer = new wxBoxSizer(wxHORIZONTAL );
    pageSizeSizer->Add( lblPageSize, 0,  wxTOP | wxALIGN_CENTER_VERTICAL , 3 );
    pageSizeSizer->Add(5, 0, 1);
    pageSizeSizer->Add( pageSizeChoice, 0,  wxTOP , 5 );
    choicesBoxSizer->Add( pageSizeSizer, 0,  wxTOP| wxBOTTOM |wxGROW );

    rightsizer->Add(choicesBoxSizer,0, wxGROW);

    wxStaticBox *styleOptBox= new wxStaticBox(panel, -1, wxT(""));
    wxStaticBoxSizer *styleOptBoxBoxSizer = new wxStaticBoxSizer( styleOptBox, wxVERTICAL );
    styleOptBoxBoxSizer->Add( lblStyleOut, 0 );
    styleOptBoxBoxSizer->Add( styleOutFileName, 0,  wxGROW | wxALIGN_LEFT);
    styleOptBoxBoxSizer->Add( lblStyleIn, 0,  wxTOP | wxALIGN_LEFT, 10 );
    wxBoxSizer *styleInSizer = new wxBoxSizer(wxHORIZONTAL );
    styleInSizer->Add( styleInFileName, 1,   wxALIGN_LEFT );
    styleInSizer->Add( butChooseStyleInfile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
    styleOptBoxBoxSizer->Add( styleInSizer, 0,wxGROW, 5);
    rightsizer->Add(styleOptBoxBoxSizer,0, wxBOTTOM|wxGROW, 5);

    rightsizer->Add(0, 20, 1);

    wxBoxSizer *previewsizer = new wxBoxSizer( wxVERTICAL );
    wxString pvLblText ;
    pvLblText.Printf(fcStrings->Read(wxT("l043"), wxEmptyString).c_str(), fcStrings->Read(wxT("l045"), wxEmptyString).c_str());
    lblPreview= new wxStaticText(panel, -1,  pvLblText);
    previewsizer->Add( lblPreview, 0,  wxTOP, 10 );
    preview = new wxHtmlWindow(panel, wxID_ANY,wxDefaultPosition,wxSize(350, 200),wxHW_NO_SELECTION|wxHW_SCROLLBAR_AUTO);
    previewsizer->Add( preview, 1,  wxTOP | wxGROW , 10 );

    wxBoxSizer *convertsizer = new wxBoxSizer( wxHORIZONTAL );
    convertsizer->Add(butStart, 0,  wxTOP, 5 );
    convertsizer->Add(butClipboard, 0,  wxTOP | wxLEFT | wxGROW, 5 );
    convertsizer->Add(5, 0, 0);
    convertsizer->Add(progress, 1, wxGROW | wxTOP , 5 );
    previewsizer->Add(convertsizer, 0,  wxTOP |wxBOTTOM |wxGROW, 5 );

    wxBoxSizer *panelsizer = new wxBoxSizer( wxHORIZONTAL );

    panelsizer->Add(leftsizer, 0, wxGROW|wxRIGHT|wxLEFT, 10);
    panelsizer->Add(rightsizer, 0,wxGROW|wxRIGHT|wxLEFT, 10);
    panelsizer->Add(previewsizer, 1,wxGROW|wxRIGHT|wxLEFT, 10);

   panelsizer->SetSizeHints( this );   // set size hints to honour minimum size
   panel->SetSizer( panelsizer );      // use the sizer for layout

   CreateStatusBar(2);
   if (!lastProject.IsEmpty() && lastProject != defaultProjectPath){
     wxString pName, pExt;
     wxFileName::SplitPath(lastProject.c_str(), NULL, &pName, &pExt);
     SetStatusText(wxString::Format(wxT("%s.%s"), pName.c_str(), pExt.c_str()).Upper(),0);
   }else {
     SetStatusText(fcStrings->Read(wxT("l041"), wxEmptyString).Upper(),0);
   }
   SetStatusText(fcStrings->Read(wxT("l024"), wxEmptyString),1);

   fillChoiceCtrl(themeChoice, wxString(dataDir.getThemePath().c_str()), wxT("*.style"));
   fillChoiceCtrl(reformatChoice, wxString(dataDir.getIndentSchemesPath().c_str()), wxT("*.indent"));
   rightsizer->Layout();Refresh(); Update();

   assignToolTip(butSource,wxT("t001"));
   assignToolTip(butOutDir,wxT("t002"));
   assignToolTip(butChooseStyleInfile,wxT("t003"));
   assignToolTip(butClearList,wxT("t004"));
   assignToolTip(styleOutFileName,wxT("t005"));
   assignToolTip(styleInFileName,wxT("t006"));
   assignToolTip(outDir,wxT("t007"));
   assignToolTip(includeStyle,wxT("t008"));
   assignToolTip(inlineCSS,wxT("t033"));
   assignToolTip(fragment,wxT("t009"));
   assignToolTip(enclosePreTag,wxT("t035"));
   assignToolTip(linenumbers,wxT("t010"));
   assignToolTip(attachAnchors,wxT("t011"));
   assignToolTip(useFileAsAnchorName,wxT("t029"));
   assignToolTip(reformatCode,wxT("t012"));
   assignToolTip(outputFormatChoice,wxT("t013"));
   assignToolTip(butStart,wxT("t014"));
   assignToolTip(generateHtmlIndex,wxT("t015"));
   assignToolTip(reformatChoice,wxT("t016"));
   assignToolTip(themeChoice,wxT("t017"));
   assignToolTip(wrapLines,wxT("t018"));
   assignToolTip(wrapLinesIntelligent,wxT("t019"));
   assignToolTip(replaceLatexQuotes,wxT("t020"));
   assignToolTip(enableLaTeXBabelCompat,wxT("t027"));
   assignToolTip(tabWidth,wxT("t021"));
   assignToolTip(lineLength,wxT("t031"));
   assignToolTip(lnFillZeroes,wxT("t022"));
   assignToolTip(writeToSourceDir,wxT("t023"));
   assignToolTip(butRemoveSelection,wxT("t024"));
   assignToolTip(useEncoding,wxT("t025"));
   assignToolTip(orderedList,wxT("t026"));
   assignToolTip(pageSizeChoice,wxT("t028"));
   assignToolTip(changeCase,wxT("t032"));
   assignToolTip(butClipboard,wxT("t034"));
   assignToolTip(preview,wxT("t030"));

   wxFFile txtFile (wxT(string(dataDir.getExtDir()+wxT("fileopenfilter.conf")).c_str()), wxT("r"));
   txtFile.ReadAll(&fileFilter);
   fileFilter.Replace(wxT("\n"),wxT("")); // wegen GTK

   if (fileFilter.IsEmpty()){
     fileFilter=wxT("All source code (*.*)|*.*|");
   }

   bool extensionsFound=loadFileTypeConfig(&extensions, &shebangs);
   if (!extensionsFound){
     wxMessageBox(fcStrings->Read(wxT("e008"), wxEmptyString),
                           fcStrings->Read(wxT("l025"), wxEmptyString),
                           wxICON_EXCLAMATION);
   }

   if (x>0 && y > 0) {
          //this->Move(x,y);
          this-> SetSize( x, y, width, height, 0);
   } else {
          this->Centre();
   }

   loadProject(currentProjectPath);

#ifndef __WXMAC__
   // initialize drag and drop
   listBox->SetDropTarget (new DropFiles (this));
#endif

}

bool HighlightFrame::loadLanguage(const wxString &newLanguage){
    wxString i18nPath=wxString::Format(wxT("%s%s.txt"), dataDir.getI18nDir().c_str(),
                                                   newLanguage.c_str() );

   fcStrings= new wxFileConfig (wxT("highlight-gui"), wxEmptyString, i18nPath,
				wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS
				#ifdef WIN32
				| wxCONFIG_USE_RELATIVE_PATH
				#endif
);
   if (!fcStrings || !fcStrings->GetNumberOfEntries()){
      wxString errMsg=wxString::Format(wxT("Could not load GUI language file (%s). Check installation."),
                    i18nPath.c_str());
      wxMessageBox(errMsg, wxT("Highlight Fatal Error"), wxICON_EXCLAMATION);
      return false;
    }
    language=newLanguage;
    return true;
}

bool HighlightFrame::loadFileTypeConfig(StringMap* extMap, StringMap* shebangMap) {
  if (!extMap || !shebangMap) return false;

  ConfigurationReader config(dataDir.getConfDir() + "filetypes.conf");
  if (config.found())
  {
        stringstream values;
        string paramName, paramVal;
        for (unsigned int i=0;i<config.getParameterNames().size();i++){
            paramName = config.getParameterNames()[i];

            if (paramName.find("ext") != string::npos){
                values.str(StringTools::lowerCase(config.getParameter(paramName))) ;
		paramName = StringTools::getParantheseVal(paramName);
            	while (values >> paramVal) {
	   		extMap->insert(make_pair( paramVal,  paramName));
            	}
            	values.clear();
            }
	    else if (paramName.find("shebang") != string::npos){
                values.str(config.getParameter(paramName)) ;
		paramName = StringTools::getParantheseVal(paramName);
            	while (values >> paramVal) {
	   		shebangMap->insert(make_pair( paramVal,  paramName));
            	}
            	values.clear();
            }

        }
        return true;
    }
  return false;
}

void HighlightFrame::OnClickButtonSource(wxCommandEvent & WXUNUSED(event))
{
  wxArrayString chosenFiles;
  wxFileDialog openFiles(this,fcStrings->Read(wxT("l001"), wxEmptyString),
                         openFileDir,  wxT(""),
                         fileFilter,

                         wxMULTIPLE | wxFILE_MUST_EXIST);
  openFiles.ShowModal();
  openFiles.GetFilenames(chosenFiles);
  openFileDir=openFiles.GetDirectory();
  if (openFileDir[openFileDir.length()-1] != Platform::pathSeparator ) {
    openFileDir += Platform::pathSeparator;
  }
  if (outDir->GetValue().IsEmpty()){
      outDir->SetValue(openFileDir);
  }
  addInputFiles(chosenFiles, openFileDir, false);
}

void HighlightFrame::OnTips(wxCommandEvent & WXUNUSED(event)){
  showStartupTips(true);
}

void HighlightFrame::addInputFiles(const wxArrayString& inputFiles,
                               const wxString &prefix, bool clear){
  wxString absInFilePath;
  if (clear) listBox->Clear();
  for (unsigned int i=0; i< inputFiles.GetCount(); i++){
     absInFilePath.Printf(wxT("%s%s"), prefix.c_str(), inputFiles[i].GetData());
     if (listBox->FindString(absInFilePath) == -1) {
       listBox->Append(absInFilePath);
     }
  }
}

void HighlightFrame::OnClickButtonTargetDir(wxCommandEvent & WXUNUSED(event))
{
  wxDirDialog targetDir(this,fcStrings->Read(wxT("l002"), wxEmptyString), wxT(""));
  targetDir.ShowModal();
  outDir->SetValue(targetDir.GetPath());
}

void HighlightFrame::OnClickButtonClear(wxCommandEvent & WXUNUSED(event)){
  listBox->Clear();
}

void HighlightFrame::OnClickButtonRemSelection(wxCommandEvent &event){
  wxArrayInt selectedItems;
  listBox->GetSelections (selectedItems);
  size_t cnt = selectedItems.GetCount();
  if (cnt){
    for (size_t i=0;i<cnt;i++){
      listBox->Delete(selectedItems[i]-i);
    }
  }
}

void HighlightFrame::OnHelp(wxCommandEvent & WXUNUSED(event)){
   showTextFile("README");
}
void HighlightFrame::OnLicense(wxCommandEvent & WXUNUSED(event)){
   showTextFile("COPYING");
}
void HighlightFrame::OnChangelog(wxCommandEvent & WXUNUSED(event)){
   showTextFile("ChangeLog");
}

void HighlightFrame::OnCheckLanguage(wxCommandEvent & event){
  bool langFound=loadLanguage(menuLang->GetLabel( event.GetId()).c_str());
  if (langFound){
    wxMessageBox(fcStrings->Read(wxT("l026"), wxEmptyString),
                 fcStrings->Read(wxT("l019"), wxEmptyString),
                 wxICON_INFORMATION);
  }
}

void HighlightFrame::OnStyleOutfileChanged(wxCommandEvent & WXUNUSED(event)){
    setStyleInLabel();
}

void HighlightFrame::OnClickButtonOpenStyleFile(wxCommandEvent & WXUNUSED(event))
{
  wxFileDialog openFiles(this,fcStrings->Read(wxT("l001"), wxEmptyString),wxT(""), wxT(""),
                         (outputFormatChoice->GetSelection()<2) ? wxT("CSS input file (*.css)|*.css|"):
                                                                  wxT("LaTeX style (*.sty)|*.sty|"),
                         wxFILE_MUST_EXIST );
  openFiles.ShowModal();
  if (!openFiles.GetFilename().IsEmpty()){
    wxString chosenFile=openFiles.GetDirectory();
    chosenFile.Append(Platform::pathSeparator);
    chosenFile.Append(openFiles.GetFilename());
    styleInFileName->SetValue(chosenFile);
}
}

void HighlightFrame::OnClickSaveProject(wxCommandEvent & WXUNUSED(event))
{
  wxFileDialog openFiles(this,fcStrings->Read(wxT("l040"), wxEmptyString),wxT(""),wxT( ""),
                         wxT("Highlight Project (*.hpr)|*.hpr|") , wxSAVE | wxOVERWRITE_PROMPT );
  openFiles.ShowModal();
  if (!openFiles.GetFilename().IsEmpty()){
    wxString chosenFile=openFiles.GetDirectory();
    chosenFile.Append(Platform::pathSeparator);
    chosenFile.Append(openFiles.GetFilename());
    bool res = writeProject(chosenFile.c_str());
    if (res){
        SetStatusText(openFiles.GetFilename().Upper(),0);
        currentProjectPath=chosenFile;
    }
  }
}

void HighlightFrame::OnClickOpenProject(wxCommandEvent & WXUNUSED(event))
{
  wxFileDialog openFiles(this,fcStrings->Read(wxT("l001"), wxEmptyString), wxT(""),  wxT(""),
                          wxT("Highlight Project (*.hpr)|*.hpr|"),
                         wxFILE_MUST_EXIST );
  openFiles.ShowModal();
  if (!openFiles.GetFilename().IsEmpty()){
    wxString chosenFile=openFiles.GetDirectory();
    chosenFile.Append(Platform::pathSeparator);
    chosenFile.Append(openFiles.GetFilename());
    bool res = loadProject(chosenFile);
    if (res){
        SetStatusText(openFiles.GetFilename().Upper(),0);
        currentProjectPath=chosenFile;
    }
  }
}

void HighlightFrame::OnClickDefaultProject(wxCommandEvent & WXUNUSED(event))
{
     bool res = loadProject(defaultProjectPath);
    if (res){
        SetStatusText(fcStrings->Read(wxT("l041"), wxEmptyString).Upper(),0);
        currentProjectPath=defaultProjectPath;
    }
}

void HighlightFrame::OnQuit2(wxCommandEvent & WXUNUSED(event)){
     Close(true); // ruft OnQuit auf
}

void HighlightFrame::OnClickElement(wxCommandEvent & event){
     plausiChecks();
     event.Skip();
}

void HighlightFrame::OnUpdatePreview(wxCommandEvent & event){
     updatePreview();
     event.Skip();
}

void HighlightFrame::OnQuit(wxCloseEvent & WXUNUSED(event))
{

  wxString userDataDir =  wxString(userPrefBaseDir.c_str());
  if (!wxDirExists(userDataDir)) wxMkdir(userDataDir);
  if (!writePreferences()) {
    wxMessageBox (fcStrings->Read(wxT("e006"), wxEmptyString),
                  fcStrings->Read(wxT("l025"), wxEmptyString), wxICON_EXCLAMATION);
  }
  writeProject(currentProjectPath);
  if (fcStrings) delete fcStrings;
  this->Destroy();
}

void HighlightFrame::OnAbout(wxCommandEvent & WXUNUSED(event)){
   wxString translatorsList;
   wxFFile txtFile (string(dataDir.getI18nDir()+"Translators.list").c_str(), wxT("r"));
   txtFile.ReadAll(&translatorsList);

   if (translatorsList.IsEmpty()){
     translatorsList=wxT("Error: File Translators.list not found.\n");
   }
   wxString formatMsg;
   formatMsg.Printf(MSG_INFO_TXT, HIGHLIGHT_VERSION, wxMAJOR_VERSION,
                    wxMINOR_VERSION, wxRELEASE_NUMBER,
                    translatorsList.c_str(),HIGHLIGHT_URL);
   wxMessageBox(formatMsg,fcStrings->Read(wxT("l019"), wxEmptyString),
                wxICON_INFORMATION);
}

void HighlightFrame::showTextFile(const wxString &file){
   wxString cmd;
   cmd.Printf(wxT("%s%s"),dataDir.getDocDir().c_str(),file.c_str());
   ShowFileDlg dlg(this, -1,file, cmd );
   if (dlg.fileFound()){
      dlg.ShowModal();
   }
}

void HighlightFrame::assignToolTip(wxWindow* widget, const char* tipName){
   wxString tip = fcStrings->Read(tipName, wxEmptyString);
   tip.Replace(wxT("\\n"), wxT("\n"));
   widget ->SetToolTip(tip);
}

void HighlightFrame::fillChoiceCtrl(wxChoice* choice, const wxString &dir,
                                    const wxString &ext){
   if (!choice) return;

   wxFileName fName;
   wxArrayString files;
   wxDir::GetAllFiles(dir, &files, ext);
   for (unsigned int i=0;i<files.GetCount(); i++){
     fName.Assign(files[i]);
     choice->Append(fName.GetName());
   }
   choice->SetSelection(0);
}

void HighlightFrame::showStartupTips(bool forceTips){
  if (showTips || forceTips ){
     wxString tipsFile;
     const char *i18nPath = dataDir.getI18nDir().c_str();
     tipsFile.Printf(wxT("%s%s.tips"),i18nPath, language.c_str());
     if (!::wxFileExists(tipsFile)){
       tipsFile.Printf(wxT("%s%s.tips"), i18nPath, "English");
     }
     wxTipProvider *tipProvider = wxCreateFileTipProvider(tipsFile, lastTip);
     showTips=wxShowTip(this, tipProvider,showTips);
     lastTip=tipProvider->GetCurrentTip();
     delete tipProvider;
  }
}

void HighlightFrame::plausiChecks(){

  bool webOutput=(outputFormatChoice->GetSelection()<2);
  bool extStyleEnabled=(outputFormatChoice->GetSelection()<4);
  bool write2outdir = !writeToSourceDir ->GetValue();
  includeStyle->Enable(extStyleEnabled || !write2outdir);
  inlineCSS->Enable(includeStyle->GetValue() && webOutput);
  enclosePreTag->Enable(fragment->GetValue() && webOutput);
  styleOutFileName->Enable(extStyleEnabled && !includeStyle->GetValue()&&write2outdir);
  styleInFileName->Enable(extStyleEnabled);
  lblStyleOut->Enable(extStyleEnabled&&!includeStyle->GetValue()&&write2outdir);
  lblStyleIn->Enable(extStyleEnabled);
  butChooseStyleInfile->Enable(extStyleEnabled);
  attachAnchors->Enable(linenumbers->GetValue() && webOutput);
  useFileAsAnchorName->Enable(attachAnchors->IsEnabled() && attachAnchors->GetValue());
  orderedList->Enable(linenumbers->GetValue() && webOutput);
  lnFillZeroes->Enable(linenumbers->GetValue()&&!(orderedList->GetValue()));
  reformatChoice->Enable(reformatCode->GetValue());
  caseChoice->Enable(changeCase->GetValue());
  wrapLinesIntelligent->Enable(wrapLines->GetValue());
  replaceLatexQuotes->Enable(outputFormatChoice->GetSelection()==2
                             && fragment->GetValue());
  enableLaTeXBabelCompat->Enable(outputFormatChoice->GetSelection()==2 );
  lblTabWidth->Enable(!reformatCode->GetValue());
  tabWidth->Enable(!reformatCode->GetValue());
  lineLength->Enable(wrapLines->GetValue());
  generateHtmlIndex->Enable(webOutput && write2outdir);

  lblOutDir ->Enable(write2outdir);
  butOutDir->Enable(write2outdir);
  outDir->Enable(write2outdir);
  if (!write2outdir)  includeStyle->SetValue(extStyleEnabled);
/*
  if (tabWidth->GetValue()<0)
    tabWidth->SetValue(0);
  else if (tabWidth->GetValue()>8)
    tabWidth->SetValue(8);
*/
  if (extStyleEnabled){
    checkStyleReferences(styleOutFileName, webOutput);
    checkStyleReferences(styleInFileName, webOutput);
  }
  bool enableEncoding =    outputFormatChoice->GetSelection()<3
                        || outputFormatChoice->GetSelection()>4;
  useEncoding->Enable(enableEncoding);
  encodingChoice->Enable(enableEncoding && useEncoding->GetValue());

  pageSizeChoice->Enable(outputFormatChoice->GetSelection()==4);
  lblPageSize->Enable(outputFormatChoice->GetSelection()==4);

  setStyleInLabel();
}

void HighlightFrame::checkStyleReferences(wxTextCtrl *styleCtrl, bool webOutput){
    wxString stylePath = styleCtrl->GetValue();
    if (stylePath.Matches( (webOutput)? wxT("*.sty") : wxT("*.css"))) {
      stylePath.Printf((webOutput)? wxT("%s.css"):wxT("%s.sty"),
                       stylePath.Mid(0, stylePath.Length()-4).c_str());
      styleCtrl->SetValue (stylePath);
    }
}

void HighlightFrame::setStyleInLabel(){
    wxString str=wxString::Format(wxT("%s %s"),fcStrings->Read(wxT("l011"), wxEmptyString).c_str(),
              ( (includeStyle->GetValue()) ? fcStrings->Read(wxT("l035"), wxEmptyString).c_str():
                  ((styleOutFileName!=NULL)?(styleOutFileName->GetValue()).c_str():
                                           wxT(CSS_DEFAULT_NAME) )));
    lblStyleIn->SetLabel(str);
}

bool HighlightFrame::writePreferences(){
    wxFileConfig test (wxT("highlight-gui"), wxEmptyString, preferencesPath,
			wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    test.Write(wxT("language"),language);
    test.Write(wxT("showTips"),showTips);
    test.Write(wxT("lastTip"),lastTip);
    test.Write(wxT("lastProject"),currentProjectPath);
    int x=0, y=0, width=0, height=0;
    this->GetPosition(&x,&y);
    this->GetSize(&width,&height);
    test.Write(wxT("winXPos"),x);
    test.Write(wxT("winYPos"),y);
    test.Write(wxT("winWidth"),width);
    test.Write(wxT("winHeight"),height);
    return test.Flush();
}

bool HighlightFrame::writeProject(const wxString& projectFile){
    wxFileConfig prjFile (wxT("highlight-gui"), wxEmptyString, projectFile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    prjFile.Write(wxT("outDir"),outDir->GetValue());
    prjFile.Write(wxT("outputFormatChoice"),outputFormatChoice->GetSelection());
    prjFile.Write(wxT("styleOutFileName"),styleOutFileName->GetValue());
    prjFile.Write(wxT("styleInFileName"),styleInFileName->GetValue());
    prjFile.Write(wxT("themeChoice"), themeChoice->GetSelection());
    prjFile.Write(wxT("openFileDir"), openFileDir.c_str());
    prjFile.Write(wxT("includeStyle"), includeStyle->GetValue());
    prjFile.Write(wxT("inlineCSS"), inlineCSS->GetValue());
    prjFile.Write(wxT("fragment"), fragment->GetValue());
    prjFile.Write(wxT("enclosepretag"), enclosePreTag->GetValue());
    prjFile.Write(wxT("linenumbers"), linenumbers->GetValue());
    prjFile.Write(wxT("attachAnchors"), attachAnchors->GetValue());
    prjFile.Write(wxT("useFileAsAnchorName"), useFileAsAnchorName->GetValue());
    prjFile.Write(wxT("reformatCode"), reformatCode->GetValue());
    prjFile.Write(wxT("reformatChoice"), reformatChoice->GetSelection());
    prjFile.Write(wxT("changeCase"), changeCase->GetValue());
    prjFile.Write(wxT("caseChoice"), caseChoice->GetSelection());
    prjFile.Write(wxT("encodingChoice"), encodingChoice->GetValue());
    prjFile.Write(wxT("useEncoding"), useEncoding->GetValue());
    prjFile.Write(wxT("generateHtmlIndex"), generateHtmlIndex->GetValue());
    prjFile.Write(wxT("wrapLines"), wrapLines->GetValue());
    prjFile.Write(wxT("wrapLinesIntelligent"), wrapLinesIntelligent->GetValue());
    prjFile.Write(wxT("replaceLatexQuotes"), replaceLatexQuotes->GetValue());
    prjFile.Write(wxT("enableLaTeXBabelCompat"), enableLaTeXBabelCompat->GetValue());
    prjFile.Write(wxT("tabWidth"), tabWidth->GetValue());
    prjFile.Write(wxT("lineLength"), lineLength->GetValue());
    prjFile.Write(wxT("lnFillZeroes"), lnFillZeroes->GetValue());
    prjFile.Write(wxT("writeToSourceDir"), writeToSourceDir->GetValue());
    prjFile.Write(wxT("orderedList"), orderedList->GetValue());
    prjFile.Write(wxT("pageSizeChoice"), pageSizeChoice->GetSelection());

    for (int fileCount=0; fileCount < listBox->GetCount(); fileCount++)
      {
         prjFile.Write(wxString::Format(wxT("inputfile_%d"), fileCount), listBox->GetString(fileCount));
      }

    return prjFile.Flush();
}


bool HighlightFrame::loadProject(const wxString& projectFile){
    wxFileConfig prjFile ("highlight-gui", wxEmptyString, projectFile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    long lVal=0L;
    bool bVal=false;

    outDir->SetValue(prjFile.Read(wxT("outDir"), ""));
    openFileDir=prjFile.Read(wxT("openFileDir"), "").c_str();
    tabWidth->SetValue(prjFile.Read(wxT("tabWidth"), wxT("4")));
    lineLength->SetValue(prjFile.Read(wxT("lineLength"), wxT("80")));
    styleOutFileName->SetValue(prjFile.Read(wxT("styleOutFileName"), wxT("highlight.css") ));
    styleInFileName->SetValue(prjFile.Read(wxT("styleInFileName"), ""));
    encodingChoice->SetValue(prjFile.Read(wxT("encodingChoice"), wxT("ISO-8859-1")) );

    prjFile.Read(wxT("outputFormatChoice"), &lVal, 0L); outputFormatChoice->SetSelection(lVal);
    prjFile.Read(wxT("themeChoice"), &lVal, 0L); themeChoice->SetSelection(lVal);
    prjFile.Read(wxT("reformatChoice"), &lVal, 0L); reformatChoice->SetSelection(lVal);
    prjFile.Read(wxT("caseChoice"), &lVal, 0L); caseChoice->SetSelection(lVal);
    prjFile.Read(wxT("pageSizeChoice"), &lVal, 1L); pageSizeChoice->SetSelection(lVal);
    prjFile.Read(wxT("includeStyle"), &bVal, false); includeStyle->SetValue(bVal);
    prjFile.Read(wxT("inlineCSS"), &bVal, false); inlineCSS->SetValue(bVal);
    prjFile.Read(wxT("fragment"), &bVal, false); fragment->SetValue(bVal);
    prjFile.Read(wxT("enclosepretag"), &bVal, false); enclosePreTag->SetValue(bVal);
    prjFile.Read(wxT("linenumbers"), &bVal, false); linenumbers->SetValue(bVal);
    prjFile.Read(wxT("attachAnchors"), &bVal, false); attachAnchors->SetValue(bVal);
    prjFile.Read(wxT("useFileAsAnchorName"), &bVal, false); useFileAsAnchorName->SetValue(bVal);
    prjFile.Read(wxT("reformatCode"), &bVal, false); reformatCode->SetValue(bVal);
    prjFile.Read(wxT("changeCase"), &bVal, false); changeCase->SetValue(bVal);
    prjFile.Read(wxT("generateHtmlIndex"), &bVal, false); generateHtmlIndex->SetValue(bVal);
    prjFile.Read(wxT("wrapLines"), &bVal, false); wrapLines->SetValue(bVal);
    prjFile.Read(wxT("wrapLinesIntelligent"), &bVal, false); wrapLinesIntelligent->SetValue(bVal);
    prjFile.Read(wxT("replaceLatexQuotes"), &bVal, false); replaceLatexQuotes->SetValue(bVal);
    prjFile.Read(wxT("enableLaTeXBabelCompat"), &bVal, false); enableLaTeXBabelCompat->SetValue(bVal);
    prjFile.Read(wxT("lnFillZeroes"), &bVal, false); lnFillZeroes->SetValue(bVal);
    prjFile.Read(wxT("useEncoding"), &bVal, false); useEncoding->SetValue(bVal);
    prjFile.Read(wxT("orderedList"), &bVal, false); orderedList->SetValue(bVal);

    int fileCount=0;
    wxString infile;
    bool done=false;
    listBox->Clear();
    while (!done){
        infile = prjFile.Read(wxString::Format(wxT("inputfile_%d"), fileCount),"");
        fileCount++;
        if (!infile.empty()){
             if (listBox->FindString(infile) == -1) {
                listBox->Append(infile);
             }
        } else {
             done = true;
        }
    }
    setStyleInLabel();
    plausiChecks();
    return true;
}

void HighlightFrame::applyCtrlValues(highlight::CodeGenerator* generator, bool previewMode){
    if (!generator) return;

    if (!previewMode) {
	generator->setStyleInputPath((styleInFileName->GetValue()).GetData());
	generator->setStyleOutputPath((styleOutFileName->GetValue()).GetData());
	generator->setIncludeStyle(includeStyle->GetValue());
	generator->setFragmentCode(fragment->GetValue());
        generator->setSpecialOptions(attachAnchors->GetValue(),
                                 orderedList->GetValue(),
                                 inlineCSS->GetValue(),
                                 enclosePreTag->GetValue(),
                                 replaceLatexQuotes->GetValue(),
                                 enableLaTeXBabelCompat->GetValue(),
                                 pageSizeChoice->GetStringSelection().Lower().c_str(),
                                 wxT("l"));
    }

    generator->setPrintLineNumbers(linenumbers->GetValue());
    generator->setPrintZeroes(lnFillZeroes->GetValue());

    wxString stylePath=wxString::Format(wxT("%s%s.style"), dataDir.getThemePath().c_str(),
                                   themeChoice->GetStringSelection().c_str());
    generator->initTheme(stylePath.c_str());

    generator->setPreformatting(getWrappingStyle(),
                               (generator->getPrintLineNumbers()) ?
                                lineLength->GetValue() - 5 : lineLength->GetValue(),
                                tabWidth->GetValue());

    if (useEncoding->GetValue()) {
        generator->setEncoding((encodingChoice->GetValue()).GetData());
    } else {
        generator->setEncoding("none");
    }
    if (changeCase->GetValue()) {
	highlight::KeywordCase kwCase=highlight::CASE_UNCHANGED;
	switch (caseChoice->GetSelection()){
		case 0: kwCase=highlight::CASE_UPPER; break;
		case 1: kwCase=highlight::CASE_LOWER; break;
		case 2: kwCase=highlight::CASE_CAPITALIZE; break;
	}
        generator->setKeyWordCase(kwCase);
    }
    // TODO
    //generator->setBaseFont(options.getBaseFont()) ;
    //generator->setBaseFontSize(options.getBaseFontSize()) ;
    generator->setLineNumberWidth(5);  /*options.getNumberWidth()*/

    if (reformatCode->GetValue()){
       wxString indentSchemePath=wxString::Format(wxT("%s%s.indent"),
                                  dataDir.getIndentSchemesPath().c_str(),
                                  reformatChoice->GetStringSelection().c_str());
      generator->initIndentationScheme(indentSchemePath.c_str());
    }
}


void HighlightFrame::OnClickButtonClipboard(wxCommandEvent & WXUNUSED(event)){
    wxArrayInt selections;
    listBox->GetSelections(selections);
    if (!selections.GetCount()) return;

    wxBeginBusyCursor();
    butStart->Disable();
    butClipboard->Disable();
    SetStatusText(fcStrings->Read(wxT("l044"), wxEmptyString),1);
    auto_ptr<highlight::CodeGenerator> generator(highlight::CodeGenerator::getInstance(getOutputType()));

    applyCtrlValues(generator.get());

    wxString currentInputFile= listBox->GetString(selections.Item(0));
    string suffix = getFileType(getFileSuffix(currentInputFile.c_str()), currentInputFile.c_str());
    string langDefPath = dataDir.getLangPath() + suffix + ".lang";
    string::size_type pos=currentInputFile.Find(Platform::pathSeparator, true);
    wxString inFileName = currentInputFile.Mid(pos+1);

    if (useFileAsAnchorName->GetValue()){
       generator->setHTMLAnchorPrefix(inFileName.c_str());
    }
    generator->setTitle(inFileName.c_str());

    wxString *clipboardData=NULL;
    if (generator->initLanguage(langDefPath)!=highlight::LOAD_FAILED) {
          clipboardData =new wxString(generator->generateStringFromFile(currentInputFile.c_str()).c_str());
    }
    if (clipboardData && wxTheClipboard->Open()) {
      // This data objects are held by the clipboard,
      // so do not delete them in the app.
      // wxTextDataObject* pDataObject = new wxTextDataObject(clipboardData);
      wxCustomDataObject* pDataObject = new wxCustomDataObject(wxDF_TEXT);

      wxDataFormat f (wxDF_TEXT);
      switch (outputFormatChoice->GetSelection()){
       // case 0: //HTML
       // case 1: //XHTML
                //f.SetId(wxT("text/html")); //w32 nok
       //         break;
       // case 2: //LATEX;
       // case 3: //TEX;
                //f.SetId(wxT("text/plain"));
           //     break;
        case 4: //RTF;
#ifdef WIN32
                f.SetId(wxT("Rich Text Format")); // w32 OK
#else
                f.SetId(wxT("text/rtf")); // gtk OK
#endif
                break;
       // case 5:  //XML
                //f.SetId(wxT("text/xml")); // w32 nok
         //       break;
      }
      pDataObject->SetFormat(f);
      pDataObject->SetData(clipboardData->Length(), clipboardData->c_str());

      wxTheClipboard->SetData(pDataObject); // do not delete pDataObject
      wxTheClipboard->Close();
    }

    wxEndBusyCursor();
    butStart->Enable();
    butClipboard->Enable();
    SetStatusText(fcStrings->Read(wxT("l024"), wxEmptyString),1);
}

void HighlightFrame::OnClickButtonStart(wxCommandEvent & WXUNUSED(event)){

    if (!listBox->GetCount() )
        return;

    if (!writeToSourceDir->GetValue() && !wxDir::Exists(outDir->GetValue()))
    {
      wxMessageBox(fcStrings->Read(wxT("l020"), wxEmptyString),
                   fcStrings->Read(wxT("l025"), wxEmptyString), wxICON_EXCLAMATION);
      outDir->SetFocus();
      return;
    }
    if (outputFormatChoice->GetSelection()<2
        && (includeStyle->GetValue()==false)
        &&(styleOutFileName->GetValue().IsEmpty()))
    {
      wxMessageBox(fcStrings->Read(wxT("l021"), wxEmptyString),
                   fcStrings->Read(wxT("l025"), wxEmptyString), wxICON_EXCLAMATION);
      styleOutFileName->SetFocus();
      return;
    }

    wxStopWatch stopWatch;

    auto_ptr<highlight::CodeGenerator> generator(highlight::CodeGenerator::getInstance(getOutputType()));

    applyCtrlValues(generator.get());

    string outfileName;
    string langDefPath;
    string suffix;
    string outfileSuffix = getOutFileSuffix();
    int fileCount;
    highlight::ParseError error;
    highlight::LoadResult loadRes;
    wxArrayString reformatFailures, outputFailures, inputFailures;
    wxString currentInputFile;

    SetStatusText(fcStrings->Read(wxT("l017"), wxEmptyString),1);

    wxBeginBusyCursor();
    butStart->Disable();
    butClipboard->Disable();

    string outPath((outDir->GetValue()).c_str());
    if (outPath[outPath.length()-1] != Platform::pathSeparator ) {
      outPath +=Platform::pathSeparator;
    }

    for (fileCount=0; fileCount < listBox->GetCount(); fileCount++){

       currentInputFile= listBox->GetString(fileCount);
       suffix = getFileType(getFileSuffix(currentInputFile.c_str()), currentInputFile.c_str());
       langDefPath = dataDir.getLangPath() + suffix + ".lang";
       loadRes=generator->initLanguage(langDefPath);
       if (loadRes==highlight::LOAD_FAILED) {
          wxString formatMsg;
          formatMsg.Printf(fcStrings->Read(wxT("e002"), wxEmptyString), suffix.c_str());
          wxMessageBox(formatMsg, fcStrings->Read(wxT("l025"), wxEmptyString),
                       wxICON_EXCLAMATION);
       } else {
          if (reformatCode->GetValue()&& !generator->formattingIsPossible()){
             reformatFailures.Add(currentInputFile);
          }
          if (writeToSourceDir->GetValue()) { //write output to source directories?
            outfileName = string(currentInputFile.c_str());
          } else {
            outfileName = outPath;
            outfileName += wxFileName(currentInputFile).GetFullName().c_str();
          }
          outfileName += outfileSuffix;

          string::size_type pos=currentInputFile.Find(Platform::pathSeparator, true);
          wxString inFileName = currentInputFile.Mid(pos+1);

          if (useFileAsAnchorName->GetValue()){
            generator->setHTMLAnchorPrefix(inFileName.c_str());
          }
          generator->setTitle(inFileName.c_str());

          error = generator->generateFile(currentInputFile.c_str(),
                                         outfileName.c_str() );
          if (error != highlight::PARSE_OK){
            stopWatch.Pause();
            if (error == highlight::BAD_INPUT) {
              inputFailures.Add(currentInputFile);
            }
            else {
              outputFailures.Add(wxString(outfileName.c_str()));
            }
            stopWatch.Resume();
          }
          progress->SetValue(100 * fileCount / listBox->GetCount());
	  wxYield();
       }
   }
   // print external style definition file
   if (includeStyle->IsEnabled() && !includeStyle->GetValue() &&!inlineCSS->GetValue()){
      string css_name = (styleOutFileName->GetValue()).GetData();
      string csspath = outPath;
      unsigned int pos = css_name.find_last_of(Platform::pathSeparator);
      if (pos == string::npos) {
         pos = css_name.find_last_of('/');
      }
      if ((pos != string::npos) && (pos < css_name.length())) {
         csspath += css_name.substr(pos+1, css_name.length());
      } else {
         csspath += css_name;
      }
      bool styleFileOK=generator -> printExternalStyle(csspath);

      if (!styleFileOK) {
         outputFailures.Add(wxString(csspath.c_str()));
      }
    }

    // print index file
    if (generateHtmlIndex->GetValue() && !writeToSourceDir->GetValue()) {
      vector <string>  fileList;
      for (int i=0; i < listBox->GetCount(); i++){
          fileList.push_back(string(listBox->GetString(i)));
       }
       bool indexFileOK=generator->printIndexFile(fileList,  outPath);
       if (!indexFileOK) {
          wxMessageBox(fcStrings->Read(wxT("e007"), wxEmptyString),
                       fcStrings->Read(wxT("l025"), wxEmptyString),
                       wxICON_EXCLAMATION);
       }
    }

   progress->SetValue(0);
   wxEndBusyCursor();
   butStart->Enable();
   butClipboard->Enable();

   wxString formatMsg;
   formatMsg.Printf(fcStrings->Read(wxT("l023"), wxEmptyString), fileCount, stopWatch.Time());
   SetStatusText(formatMsg, 1);

   if (!reformatFailures.IsEmpty()||
       !inputFailures.IsEmpty() ||
       !outputFailures.IsEmpty()) {
       FailureDlg dlg(this, -1,fcStrings->Read(wxT("l034"), wxEmptyString),
                      fcStrings->Read(wxT("e010"), wxEmptyString), reformatFailures,
                      fcStrings->Read(wxT("e003"), wxEmptyString), inputFailures,
                      fcStrings->Read(wxT("e004"), wxEmptyString), outputFailures );
       dlg.ShowModal() ;
   }
}

highlight::WrapMode HighlightFrame::getWrappingStyle(){
  if (!wrapLinesIntelligent->GetValue() && wrapLinesIntelligent->IsEnabled())
     return highlight::WRAP_SIMPLE;
  return (wrapLines->GetValue())?highlight::WRAP_DEFAULT:highlight::WRAP_DISABLED;
}

string HighlightFrame::analyzeFile(const string& file){
    ifstream inFile(file.c_str());
    string firstLine;
    getline (inFile, firstLine);
    StringMap::iterator it;
     for (it=shebangs.begin(); it!=shebangs.end();it++){
       if (Pattern::matches(it->first, firstLine)) return it->second;
     }
    return "";
}

string HighlightFrame::getFileType(const string& suffix, const string &inputFile)
{
    string lcSuffix = StringTools::lowerCase(suffix);
    string fileType = (extensions.count(lcSuffix)) ? extensions[lcSuffix] : lcSuffix ;
    if (!fileType.empty()) return fileType;
    return analyzeFile(inputFile);
}

string HighlightFrame::getFileSuffix(const string& fileName)
{
  unsigned int ptPos=fileName.rfind(".");
  return (ptPos == string::npos) ? "" : fileName.substr(ptPos+1,
                                        fileName.length());
}

string HighlightFrame::getOutFileSuffix(){
  switch (outputFormatChoice->GetSelection()) {
    case 0: return ".html";
    case 1: return ".xhtml";
    case 2:
    case 3: return ".tex";
    case 4: return ".rtf";
    case 5: return ".xml";
  }
  return ".html";
}

highlight::OutputType HighlightFrame::getOutputType(){
    switch (outputFormatChoice->GetSelection()){
     case 0: return highlight::HTML;
     case 1: return highlight::XHTML;
     case 2: return highlight::LATEX;
     case 3: return highlight::TEX;
     case 4: return highlight::RTF;
     case 5: return highlight::XML;
    }
    return highlight::HTML;
}

void HighlightFrame::updatePreview(){
    wxArrayInt selections;
    listBox->GetSelections(selections);
    if (!selections.GetCount()) return;

    int x=0, y=0;
    preview-> GetViewStart(&x, &y);
    wxBeginBusyCursor();
    SetStatusText(fcStrings->Read(wxT("l044"), wxEmptyString),1);
    highlight::Html32Generator pwgenerator;

    applyCtrlValues(&pwgenerator, true);
    pwgenerator.setMaxInputLineCnt(500);
    pwgenerator.setBaseFont("Times");

    wxString previewData;
    wxString currentInputFile= listBox->GetString(selections.Item(0));
    string suffix = getFileType(getFileSuffix(currentInputFile.c_str()), currentInputFile.c_str());
    string langDefPath = dataDir.getLangPath() + suffix + ".lang";
    if (pwgenerator.initLanguage(langDefPath)!=highlight::LOAD_FAILED) {
          previewData = wxString(pwgenerator.generateStringFromFile(currentInputFile.c_str()).c_str());
    }
    if (!previewData.IsEmpty()) {
        wxString pwLabel;
        pwLabel.Printf(fcStrings->Read(wxT("l043"), wxEmptyString).c_str(), currentInputFile.c_str());
        lblPreview->SetLabel(pwLabel);
        preview->SetPage(previewData);
    }
    wxEndBusyCursor();
    preview->Scroll( x, y);
    SetStatusText(fcStrings->Read(wxT("l024"), wxEmptyString),1);
}

#ifndef __WXMAC__
bool DropFiles::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames) {
  for (size_t n = 0; n < filenames.Count(); n++) {
    if (wxFileName(filenames[n]).IsDir()) {
        wxMessageBox (wxT("Directories are not allowed!"),
                      wxT("Error"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
  }
  m_frame->addInputFiles(filenames, "", false);
  return true;
}
#endif
