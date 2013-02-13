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

#include <wx/window.h>
#include <wx/event.h>
#include "frsky_event.hpp"
#include "tensor_data.hpp"
#include "tensor_proto.hpp"
#include "sp_thread.hpp"
#include "frsky_serial_port.hpp"
#include "frsky_dataMain.h"


SP_thread::SP_thread(frsky_dataDialog* wnd,const char* filename)
:wxThread{wxTHREAD_DETACHED},m_frsky_dataDialog{wnd},m_filename{filename}
{
   for( uint32_t i = 0; i < num_functions;++i){
         data_functions[i] = sink;
   }
}
SP_thread::~SP_thread()
{
}

void* SP_thread::Entry()
{
  auto sp = new serial_port(m_filename.c_str());
  sp->init();
  

  auto frsky_sp = new frsky_serial_port(sp);
  
  while ( !this->TestDestroy()){

     unsigned char buffer[9];
     if(frsky_sp->get_frame(buffer) == true)
     {
         call(buffer); // exec the fn in the buffer
     }
  };
 
  delete frsky_sp;
  wxCriticalSectionLocker lock(m_frsky_dataDialog->m_sp_threadCS);
  m_frsky_dataDialog->m_sp_thread = nullptr;
  return 0;
}


void SP_thread::set_fun(uint8_t fun_id,data_fun_ptr fun)
{
     // check's fun_id valid b4 assigning
     data_functions[frsky_proto_map(fun_id)] = fun;
}

void SP_thread::call(unsigned char (& buf)[9])const
{
    data_functions[ frsky_proto_map(buf[0])] (buf);
    // signal here only when there is Remote Volatage and link quality
    // The threads processing the other data will signal themselves
    if ( buf[0] == FrSky2WSP::CommandID::RVLQ){
         frskyEvent x{wxEVT_FRSKY_EVENT_RVLQ};
         m_frsky_dataDialog->AddPendingEvent(x);
    }
}


