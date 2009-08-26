/***************************************************************************
                     gui_showfile_dlg.h  -  description
                             -------------------
    begin                : 09.01.07
    copyright            : (C) 2007 by André Simon
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

#ifndef GUI_SHOWFILE_DLG
#define GUI_SHOWFILE_DLG

#include <wx/dialog.h>

class ShowFileDlg : public wxDialog {

public:
  ShowFileDlg(wxWindow* parent, wxWindowID id, const wxString& title,
              const wxString& path);
  bool fileFound() { return found; }

private:
    bool found;
};

#endif
