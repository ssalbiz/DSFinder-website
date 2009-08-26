/***************************************************************************
                   gui_showfile_dlg.cpp  -  description
                             -------------------
    begin                : 09.01.07
    copyright            : (C) 2007 by Andr� Simon
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


#include "gui_options_dlg.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/ffile.h>

AdvancedOptionsDlg::AdvancedOptionsDlg(wxWindow* parent, wxWindowID id,  const wxString& title, 
             wxFileConfig *strings):
wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
fcStrings(strings)

{
   wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

   linenumbers = new wxCheckBox(this, ID_INCLUDELINENUMBERS,
                                "dcsa");

   sizer ->Add(linenumbers, 1, wxALIGN_CENTRE | wxALL|wxGROW, 5 );

   wxButton* butOK = new wxButton( this, wxID_OK , "OK" , wxDefaultPosition,
                             wxDefaultSize, 0 );
   sizer ->Add(butOK, 0, wxALIGN_CENTRE | wxALL, 5 );
   sizer->SetSizeHints( this );
   SetSizer(sizer);
   Centre();
}

