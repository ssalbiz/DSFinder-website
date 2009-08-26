/***************************************************************************
                     gui_failure_dlg.h  -  description
                             -------------------
    begin                : 19.02.05
    copyright            : (C) 2005 by Andr�Simon
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

#ifndef GUI_FAILURE_DLG
#define GUI_FAILURE_DLG

#include <wx/dialog.h>

class FailureDlg : public wxDialog {

public:
  FailureDlg(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxString& msgFailedFormat, const wxArrayString& failedFormatFiles,
    const wxString& msgFailedOpen, const wxArrayString& failedOpenFiles,
    const wxString& msgFailedWrite, const wxArrayString& failedWriteFiles);

private:
  void addErrorFileList(wxSizer* sizer, const wxString& msg, const wxArrayString&fileList);

};

#endif
