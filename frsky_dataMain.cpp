
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

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <quan/gx/wxwidgets/from_wxString.hpp>
#include "frsky_dataMain.h"
#include <quan/serial_port.hpp>
#include "sp_thread.hpp"
#include "frsky_event.hpp"
#include "tensor_proto.hpp"
#include "tensor_data.hpp"

//helper functions
enum wxbuildinfoformat { short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(frsky_dataDialog, wxDialog)
   
    EVT_CUSTOM(wxEVT_FRSKY_EVENT_LATITUDE,wxID_ANY,frsky_dataDialog::OnLatitude)
    EVT_CUSTOM(wxEVT_FRSKY_EVENT_LONGTITUDE,wxID_ANY,frsky_dataDialog::OnLongtitude)
    EVT_CUSTOM(wxEVT_FRSKY_EVENT_ALTITUDE,wxID_ANY,frsky_dataDialog::OnAltitude)
    EVT_CUSTOM(wxEVT_FRSKY_EVENT_AIRSPEED,wxID_ANY,frsky_dataDialog::OnAirspeed)
    EVT_CUSTOM(wxEVT_FRSKY_EVENT_RVLQ,wxID_ANY,frsky_dataDialog::OnRVLQ) 

    EVT_CLOSE(frsky_dataDialog::OnClose)
    EVT_BUTTON(idBtnQuit, frsky_dataDialog::OnQuit)
    EVT_BUTTON(idBtnAbout, frsky_dataDialog::OnAbout)
    EVT_BUTTON(idBtnConnect, frsky_dataDialog::OnConnectDisconnect)

END_EVENT_TABLE()

frsky_dataDialog::frsky_dataDialog(wxDialog *dlg, const wxString &title)
    : wxDialog{dlg, -1, title},
     m_port_label{nullptr},
     m_port_text{nullptr},
     m_lat_label{nullptr},
     m_lat_text{nullptr},
     m_lon_label{nullptr},
     m_lon_text{nullptr},
     m_alt_label{nullptr},
     m_alt_text{nullptr},
     m_airspeed_label{nullptr},
     m_airspeed_text{nullptr},
     BtnAbout{nullptr}, 
     BtnQuit{nullptr},
     m_sp_thread{nullptr},
     m_tensor_thread{nullptr}
{
    
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    auto latlonsizer = new wxFlexGridSizer{10,2,0,0};

    //line 1 port text
    
    m_port_label = new wxStaticText{this,wxID_ANY, wxT("serial port"),wxDefaultPosition, wxDefaultSize, 0};
    m_port_text = new wxTextCtrl{this,wxID_ANY, wxT("/dev/ttyUSB0"), wxDefaultPosition,wxSize(200,wxDefaultSize.GetHeight())};
    latlonsizer->Add(m_port_label,0,wxALL,5);
    latlonsizer->Add(m_port_text,0,wxALL,5);

    // line 2 connect button
    auto spacer1 = new wxPanel{this,wxID_ANY,{0,0},{1,1}};
    BtnConnect = new wxButton(this, idBtnConnect, wxT("&Connect"), wxDefaultPosition, wxSize(100,wxDefaultSize.GetHeight()), 0);

    latlonsizer->Add(spacer1,0,wxALL,5);
    latlonsizer->Add(BtnConnect,0,wxALL,5);
    
   // line 3 lat
    m_lat_label = new wxStaticText{this,wxID_ANY, wxT("Latitude"),wxDefaultPosition, wxDefaultSize, 0};
    m_lat_text = new wxTextCtrl{this,wxID_ANY, wxT("0.000000N"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};
    latlonsizer->Add(m_lat_label,0,wxALL,5);
    latlonsizer->Add(m_lat_text,0,wxALL,5);

    //line 4 lon
    m_lon_label = new wxStaticText{this,wxID_ANY, wxT("Longtitude"),wxDefaultPosition, wxDefaultSize, 0};
    m_lon_text = new wxTextCtrl{this,wxID_ANY, wxT("0.000000W"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};

    latlonsizer->Add(m_lon_label,0,wxALL,5);
    latlonsizer->Add(m_lon_text,0,wxALL,5);

    //line 5 alt
    m_alt_label = new wxStaticText{this,wxID_ANY, wxT("Altitude"),wxDefaultPosition, wxDefaultSize, 0};
    m_alt_text = new wxTextCtrl{this,wxID_ANY, wxT("0 m"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};
    latlonsizer->Add(m_alt_label,0,wxALL,5);
    latlonsizer->Add(m_alt_text,0,wxALL,5);

    //line 6 airspeed
    m_airspeed_label = new wxStaticText{this,wxID_ANY, wxT("Airspeed"),wxDefaultPosition, wxDefaultSize, 0};
    m_airspeed_text = new wxTextCtrl{this,wxID_ANY, wxT("0 m/s"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};
    latlonsizer->Add(m_airspeed_label,0,wxALL,5);
    latlonsizer->Add(m_airspeed_text,0,wxALL,5);

    //line 7 uplink
    m_uplink_label = new wxStaticText{this,wxID_ANY, wxT("Uplink quality"),wxDefaultPosition, wxDefaultSize, 0};
    m_uplink_text = new wxTextCtrl{this,wxID_ANY, wxT("0 dB"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};
    latlonsizer->Add(m_uplink_label,0,wxALL,5);
    latlonsizer->Add(m_uplink_text,0,wxALL,5);

    //line 8 downlink
    m_downlink_label = new wxStaticText{this,wxID_ANY, wxT("Downlink quality"),wxDefaultPosition, wxDefaultSize, 0};
    m_downlink_text = new wxTextCtrl{this,wxID_ANY, wxT("0 dB"), wxDefaultPosition,wxSize(100,wxDefaultSize.GetHeight()), wxTE_READONLY | wxTE_RIGHT};
    latlonsizer->Add(m_downlink_label,0,wxALL,5);
    latlonsizer->Add(m_downlink_text,0,wxALL,5);

    //line 9 spacer
    auto spacer2 = new wxPanel{this,wxID_ANY,{0,0},{1,1}};
    auto spacer3 = new wxPanel{this,wxID_ANY,{0,0},{1,1}};
    latlonsizer->Add(spacer2,0,wxALL,10);
    latlonsizer->Add(spacer3,0,wxALL,10);

     //line 10 about quit
    BtnAbout = new wxButton(this, idBtnAbout, wxT("&About"), wxDefaultPosition, wxDefaultSize, 0);
    BtnQuit = new wxButton(this, idBtnQuit, wxT("&Quit"), wxDefaultPosition, wxDefaultSize, 0);
    latlonsizer->Add(BtnAbout, 0, wxALL, 5);
    latlonsizer->Add(BtnQuit,0,wxALL,5);
    
    this->SetSizer(latlonsizer);
    this->Layout();
    latlonsizer->Fit(this);
}

frsky_dataDialog::~frsky_dataDialog()
{
  
}

/*
  Both threads null their pointers in this class
  avter being notified to quit and
  before returning from their Entry function
*/
void frsky_dataDialog::delete_tensor_thread()
{
   {
      wxCriticalSectionLocker lock(m_tensor_threadCS);
      if(m_tensor_thread){
         m_tensor_thread->Delete();
      }
   }

   for (;;)
   {
      wxCriticalSectionLocker lock(m_tensor_threadCS);
      if ( m_tensor_thread == nullptr){
         break;
      }
   }
}
void frsky_dataDialog::delete_sp_thread()
{
   {
      wxCriticalSectionLocker lock(m_sp_threadCS);
      if(m_sp_thread){
         m_sp_thread->Delete();
      }
   }

   for (;;)
   {
      wxCriticalSectionLocker lock(m_sp_threadCS);
      if ( m_sp_thread == nullptr){
         break;
      }
   }
}

bool frsky_dataDialog::Destroy()
{
   delete_tensor_thread();
   delete_sp_thread();

   return wxDialog::Destroy();
}

void frsky_dataDialog::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void frsky_dataDialog::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void frsky_dataDialog::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxT("Quan Tracker V1.0");
    wxMessageBox(msg, _("About"));
}

void frsky_dataDialog::OnLatitude(wxEvent & )
{
   tensor_protocol::critsect_telemetry.Enter();
   double v = tensor_protocol::latitude.numeric_value();
   tensor_protocol::critsect_telemetry.Leave();
   wxString str = ( v >= 0 )
    ? wxString::Format(wxT("%fN"),v)
    : wxString::Format(wxT("%fS"),-v);
    m_lat_text->ChangeValue(str);
}
void frsky_dataDialog::OnLongtitude(wxEvent &)
{
   tensor_protocol::critsect_telemetry.Enter();
   double v = tensor_protocol::longtitude.numeric_value();
   tensor_protocol::critsect_telemetry.Leave();
   wxString str = ( v >= 0)
   ? wxString::Format(wxT("%fE"),v)
   : wxString::Format(wxT("%fW"),-v);
   m_lon_text->ChangeValue(str);
}
void frsky_dataDialog::OnAltitude(wxEvent & event)
{
   tensor_protocol::critsect_telemetry.Enter();
   double v = tensor_protocol::altitude.numeric_value();
   tensor_protocol::critsect_telemetry.Leave();
   wxString str =  wxString::Format(wxT("%f m"),v);
   m_alt_text->ChangeValue(str);
}
void frsky_dataDialog::OnAirspeed(wxEvent & event)
{
   tensor_protocol::critsect_telemetry.Enter();
   double v = tensor_protocol::airspeed.numeric_value();
   tensor_protocol::critsect_telemetry.Leave();
   wxString str =  wxString::Format(wxT("%f m/s"),v);
   m_airspeed_text->ChangeValue(str);
}
void frsky_dataDialog::OnRVLQ(wxEvent & event)
{
   {
      tensor_protocol::critsect_telemetry.Enter();
      uint8_t uplink = tensor_protocol::ULQ;
      tensor_protocol::critsect_telemetry.Leave();
      wxString upstr =  wxString::Format(wxT("%u dB"),static_cast<unsigned int>(uplink));
      m_uplink_text->ChangeValue(upstr);
   }

   {
      tensor_protocol::critsect_telemetry.Enter();
      uint8_t downlink = tensor_protocol::DLQ;
      tensor_protocol::critsect_telemetry.Leave();
      wxString downstr =  wxString::Format(wxT("%u dB"),static_cast<unsigned int>(downlink));
      m_downlink_text->ChangeValue(downstr);
   }
}


void frsky_dataDialog::OnConnectDisconnect(wxCommandEvent &event)
{

   if ( (m_sp_thread == nullptr) && (m_tensor_thread == nullptr))
   {
      std::string text = quan::gx::wxwidgets::from_wxString<char>(m_port_text->GetValue());
      // do a test open of the port
      auto testsp = new quan::serial_port(text.c_str());
      testsp->init();
      bool good = testsp->good();
      delete testsp; testsp = nullptr;

      if ( good){
         try{
            m_tensor_thread = new tensor_data(this);
            if( ( m_tensor_thread->Create() == wxTHREAD_NO_ERROR)
                  && (  m_tensor_thread->Run() == wxTHREAD_NO_ERROR))
            {
              ;
            }else{
               delete m_tensor_thread; // n.b. dtor clears m_tensor_thread to nullptr
               m_tensor_thread = nullptr;
               wxString msg = wxT("Failed to Create Tensor thread. Please Report");
               wxMessageBox( msg,wxT("Thread Error"));
               return;
            }

            m_sp_thread = new SP_thread(this,text.c_str());
            m_sp_thread->set_fun(FrSky2WSP::CommandID::RVLQ,tensor_protocol::onRVLQ);
            m_sp_thread->set_fun(FrSky2WSP::CommandID::UserData,tensor_protocol::onUserData);
            if(( m_sp_thread->Create() == wxTHREAD_NO_ERROR)
            && ( m_sp_thread->Run() == wxTHREAD_NO_ERROR))
            {
               // now running..
                BtnConnect->SetLabel(wxT("Disconnect"));
            }
            else{
               delete_tensor_thread(); // was running
               delete m_sp_thread; // assume wasnt running // n.b. dtor clears m_sp_thread to nullptr
               m_sp_thread = nullptr;
               wxString msg = wxT("Failed to Create serial port thread. Please Report");
               wxMessageBox( msg,wxT("Thread Error"));
               return;
            }
         }
         catch( std::exception & e){
            // Well Dodgy here!
            delete_tensor_thread();
            delete_sp_thread();  // assume both running
            wxString msg = wxT("Exception: \"") + wxString::FromAscii(e.what()) + wxT("\"");
            wxMessageBox( msg,wxT("Thread Error"));
            BtnConnect->SetLabel(wxT("Connect"));
            return;
         }
      }else{ // must be a  problem with the serial port
         wxString msg = wxT("Open \"") + wxString::FromAscii(text.c_str()) + wxT("\" failed. Check port.");
         wxMessageBox( msg,wxT("Connect Error"));
      }   
   }
   else{ // want disconnect
      delete_tensor_thread();
      delete_sp_thread(); // assume both running
      BtnConnect->SetLabel(wxT("Connect"));
   }
}

