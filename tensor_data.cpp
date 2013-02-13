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
#include <iostream>
#include <iomanip>
#include <wx/window.h>
#include <wx/event.h>
#include <quan/uav/do_checksum.hpp>
#include <quan/uav/msg_protocol.hpp>
#include <quan/out/angle.hpp>
#include <quan/out/length.hpp>
#include <quan/out/velocity.hpp>
#include "tensor_data.hpp"
#include "frsky_event.hpp"
#include "tensor_proto.hpp"
#include "frsky_dataMain.h"

tensor_data::tensor_data(frsky_dataDialog* wnd)
:wxThread{wxTHREAD_DETACHED}, m_frsky_dataDialog(wnd)
{
 
}

tensor_data::~tensor_data()
{
}

void * tensor_data::Entry()
{
    while (!this->TestDestroy()) {
        bool quit = false;
        uint8_t  msg_buf[7];
        for(int i =0; i < 7; ++i ) {
            msg_buf[i] =0;
        }

        tensor_protocol::critsect_input_buffer.Enter();
        if( tensor_protocol::input_buffer.is_empty()) {
             tensor_protocol::critsect_input_buffer.Leave();
             this->Yield();
             continue;
        } else {
            tensor_protocol::critsect_input_buffer.Leave();
            typedef  quan::uav::frsky::msgID msgID;
            uint8_t ch = 0;
            //look for the header. know there is one byte in at least
            //Dump data until get the header
            tensor_protocol::critsect_input_buffer.Enter();
            while( tensor_protocol::input_buffer.get(ch)) {
                quit = this->TestDestroy();
                if ( quit){
                  break;
                }
                tensor_protocol::critsect_input_buffer.Leave();
                if( ch == msgID::header_value ) {
                    msg_buf[0] = ch;
                    tensor_protocol::critsect_input_buffer.Enter();
                    break;
                }
                tensor_protocol::critsect_input_buffer.Enter();
            }
            tensor_protocol::critsect_input_buffer.Leave();

            if (quit) {
               break;
            }
            
            if (msg_buf[0] !=  msgID::header_value) { 
               continue;
            } else {
                 // got the header
                //  look for a full frame
                // look for twice as much as need as may be escaped
                tensor_protocol::critsect_input_buffer.Enter();
                while ((tensor_protocol::input_buffer.num_in_buffer() < 12) ) {
                      quit = this->TestDestroy();
                      if ( quit){
                        break;
                      }
                      tensor_protocol::critsect_input_buffer.Leave();
                     
                      this->Yield();
                      tensor_protocol::critsect_input_buffer.Enter();
                }
                tensor_protocol::critsect_input_buffer.Leave();
                if (quit) {
                   break;
                }
                // acquire enough data for a frame. Should be enough in the buffer
                // apply escapes
                int idx = 0;
                while ((idx < 6) ) {
                     quit = this->TestDestroy();
                     if ( quit){
                        break;
                     }
                    uint8_t ch1;

                    tensor_protocol::critsect_input_buffer.Enter();
                    tensor_protocol::input_buffer.get(ch1);
                    tensor_protocol::critsect_input_buffer.Leave();
                    if( ch1 == quan::uav::frsky::msgID::escape_value ){
                        tensor_protocol::critsect_input_buffer.Enter();
                        tensor_protocol::input_buffer.get(ch1);
                        tensor_protocol::critsect_input_buffer.Leave();
                        ch1 ^= 0x60;
                    }
                    msg_buf[++idx] = ch1;
                }
                //done with the buffer
 
                if(quit){
                   break;
                }
                //should now have a frame...
                if( quan::uav::do_checksum(msg_buf) == msg_buf[6]) {

                    uint32_t rawdata = 
                       static_cast<uint32_t>( msg_buf[2])
                        + (static_cast<uint32_t>(msg_buf[3]) << 8U) 
                        + (static_cast<uint32_t>(msg_buf[4]) << 16U) 
                        + (static_cast<uint32_t>(msg_buf[5]) << 24U);
                    switch (msg_buf[1]) {
                    case msgID::GPSLAT: {
                       
                        tensor_protocol::critsect_telemetry.Enter();
                        tensor_protocol::latitude = quan::angle::deg{ ((int32_t)rawdata)/1e7};
                        tensor_protocol::critsect_telemetry.Leave();
                 
                        frskyEvent x{wxEVT_FRSKY_EVENT_LATITUDE};
                        m_frsky_dataDialog->AddPendingEvent(x);
                    }
						  break;
                    case msgID::GPSLON: {

                        tensor_protocol::critsect_telemetry.Enter();
                        tensor_protocol::longtitude = quan::angle::deg { ((int32_t)rawdata)/1e7};
                        tensor_protocol::critsect_telemetry.Leave();

                        frskyEvent x{wxEVT_FRSKY_EVENT_LONGTITUDE};
                        m_frsky_dataDialog->AddPendingEvent(x);
                     
                    }
                    break;
                    case msgID::BAROALT: {
                        tensor_protocol::critsect_telemetry.Enter();
            
                        tensor_protocol::altitude = quan::length::m { ((int32_t)rawdata)};
                        tensor_protocol::critsect_telemetry.Leave();
                        
                        frskyEvent x{wxEVT_FRSKY_EVENT_ALTITUDE};
                        m_frsky_dataDialog->AddPendingEvent(x);
                    }
                    break;
                    case msgID::AIRSPEED: {
               
                       tensor_protocol::critsect_telemetry.Enter();
                       tensor_protocol::airspeed = quan::velocity::m_per_s{(int32_t)rawdata};
                       tensor_protocol::critsect_telemetry.Leave();
                    
                       frskyEvent x{wxEVT_FRSKY_EVENT_AIRSPEED};
                       m_frsky_dataDialog->AddPendingEvent(x);
                    }
                    break;
                    default:
                       
                       std::cerr << "Error Unknown user data : " << (int)msg_buf[1]  << "\n";
                       break;
                    }
                } else {
                    std::cerr << "*****************checksum failed***************\n";
                }
            }
        }
    }
    wxCriticalSectionLocker lock(m_frsky_dataDialog->m_tensor_threadCS);
    m_frsky_dataDialog->m_tensor_thread = nullptr;
    return 0;
}
 