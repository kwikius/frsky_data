/*
 Copyright (c) 2012 - 2013 Andy Little 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#ifndef FRSKY_DATAMAIN_H
#define FRSKY_DATAMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/textctrl.h>

#include "frsky_dataApp.h"
#include "sp_thread.hpp"
#include "tensor_data.hpp"

class frsky_dataDialog: public wxDialog
{
    public:
        frsky_dataDialog(wxDialog *dlg, const wxString& title);
        ~frsky_dataDialog();

        bool Destroy();
    protected:
        enum
        {
            idBtnQuit = 1000,
            idBtnAbout,
            idBtnConnect
        };

        wxStaticText* m_port_label;
        wxTextCtrl* m_port_text;
    
        wxStaticText* m_lat_label;
        wxTextCtrl* m_lat_text;

        wxStaticText* m_lon_label;
        wxTextCtrl* m_lon_text;

        wxStaticText* m_alt_label;
        wxTextCtrl* m_alt_text;

        wxStaticText* m_airspeed_label;
        wxTextCtrl* m_airspeed_text;

        wxStaticText* m_uplink_label;
        wxTextCtrl* m_uplink_text;

        wxStaticText* m_downlink_label;
        wxTextCtrl* m_downlink_text;
       
        wxButton* BtnConnect;
        wxButton* BtnAbout;
        wxButton* BtnQuit;

    private:
        friend struct SP_thread;
        friend struct tensor_data;

        SP_thread* m_sp_thread;
        wxCriticalSection m_sp_threadCS;

        tensor_data* m_tensor_thread;
        wxCriticalSection m_tensor_threadCS;

        void delete_tensor_thread();
        void delete_sp_thread();

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnConnectDisconnect(wxCommandEvent& event);
        void OnLatitude(wxEvent & event);
        void OnLongtitude(wxEvent & event);
        void OnAltitude(wxEvent & event);
        void OnAirspeed(wxEvent & event);
        void OnRVLQ(wxEvent & event);

        DECLARE_EVENT_TABLE()
};

#endif // FRSKY_DATAMAIN_H
