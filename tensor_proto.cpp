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

#include <cstdio>
#include "tensor_proto.hpp"

quan::fifo<unsigned char,1000> tensor_protocol::input_buffer;
bool tensor_protocol::synclost;

uint8_t tensor_protocol::A1;
uint8_t tensor_protocol::A2;
uint8_t tensor_protocol::ULQ;
uint8_t tensor_protocol::DLQ;

quan::angle::deg        tensor_protocol::latitude ;
quan::angle::deg        tensor_protocol::longtitude ; 
quan::length::m         tensor_protocol::altitude;
quan::velocity::m_per_s tensor_protocol::airspeed;

wxCriticalSection tensor_protocol::critsect_input_buffer;
wxCriticalSection tensor_protocol::critsect_telemetry;
/*
http://www.rcgroups.com/forums/showthread.php?t=1200443&page=15
(see Chase Wu of FrSky  post #221, #223 in thread down page)
*/
void tensor_protocol::onRVLQ (unsigned char (& buf)[9])
{
		A1 = buf[1];
      A2 = buf[2];
      DLQ = buf[3];
      ULQ = buf[4] / 2;
     
}
/*
 put the User data in the fifo
*/
void tensor_protocol::onUserData(unsigned char (& buf)[9])
{
   int numbytes = buf[1];
   if (numbytes > 6){
      fprintf(stderr,"bytes too big in user");
      numbytes = 6;
   }
    // block for a lock
  // rctx_inbuf_mutex.lock(); 
   tensor_protocol::critsect_input_buffer.Enter();
   auto num_in_buf = input_buffer.num_free();
   tensor_protocol::critsect_input_buffer.Leave();
   if ( num_in_buf < numbytes){
    synclost = true;
      // TODO work out what happens here
     //look for header byte and go back to it?
   }else{
     
      for (int i = 0; i < numbytes; ++i){
         wxCriticalSectionLocker lock(critsect_input_buffer);
         input_buffer.put(buf[3+i]);
      }
   }
}
