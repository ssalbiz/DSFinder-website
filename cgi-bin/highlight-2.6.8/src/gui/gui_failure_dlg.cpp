/***************************************************************************
                   gui_failure_dlg.cpp  -  description
                             -------------------
    begin                : 19.02.05
    copyright            : (C) 2005 by André Simon
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


#include "gui_failure_dlg.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/stattext.h>

FailureDlg::FailureDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxString& msgFailedFormat, const wxArrayString& failedFormatFiles,
                       const wxString& msgFailedOpen, const wxArrayString& failedOpenFiles,
                       const wxString& msgFailedWrite, const wxArrayString& failedWriteFiles):
wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)

{
   wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

   addErrorFileList(sizer, msgFailedOpen, failedOpenFiles);
   addErrorFileList(sizer, msgFailedWrite, failedWriteFiles);
   addErrorFileList(sizer, msgFailedFormat, failedFormatFiles);

   wxButton* butOK = new wxButton( this, wxID_OK , "OK" , wxDefaultPosition,
                             wxDefaultSize, 0 );
   sizer ->Add(butOK, 0, wxALIGN_CENTRE | wxALL, 5 );
   sizer->SetSizeHints( this );
   SetSizer(sizer);
   Centre();
}

void FailureDlg::addErrorFileList(wxSizer* sizer, const wxString& msg, const wxArrayString&fileList){
   if (!fileList.IsEmpty()){
     wxStaticText* msgLabel= new wxStaticText(this, -1, msg);

     wxTextCtrl * failureList = new wxTextCtrl(this, -1, "",
                                         wxDefaultPosition,wxSize(300,75),
                 wxTE_MULTILINE  | wxTE_READONLY);
     wxString fName;
     for (size_t i=0;i<fileList.GetCount(); i++ ){
            fName.Printf("%s\n", fileList[i].c_str());
            failureList->AppendText(fName);
     }
     sizer ->Add(5,0);
     sizer ->Add(msgLabel, 0, wxLEFT | wxTOP, 5);
     sizer ->Add(failureList,1, wxGROW | wxALL, 5 );
   }

}
