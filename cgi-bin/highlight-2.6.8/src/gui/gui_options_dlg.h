/***************************************************************************
                     gui_showfile_dlg.h  -  description
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

#ifndef GUI_OPTIONS_DLG
#define GUI_OPTIONS_DLG

#include <wx/dialog.h>
#include <wx/fileconf.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/fileconf.h>

#include "widget_ids.h"

class AdvancedOptionsDlg : public wxDialog {

public:
  AdvancedOptionsDlg(wxWindow* parent, wxWindowID id,  const wxString& title,wxFileConfig *strings);

 wxCheckBox *linenumbers;
 wxFileConfig *fcStrings;
private:

};

#endif
