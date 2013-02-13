#ifndef TENSOR_PROTO_HPP_INCLUDED
#define TENSOR_PROTO_HPP_INCLUDED

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

#include <cstdint>
#include <wx/thread.h>
#include <quan/utility/fifo.hpp>
#include <quan/out/angle.hpp>
#include <quan/out/length.hpp>
#include <quan/out/velocity.hpp>

struct tensor_protocol{

   static void onRVLQ (unsigned char (& buf)[9]);
   static void onUserData(unsigned char (& buf)[9]);

   static wxCriticalSection critsect_input_buffer;
   static quan::fifo<unsigned char,1000> input_buffer;
   static bool synclost;
   
   static wxCriticalSection critsect_telemetry;
   static uint8_t A1;
   static uint8_t A2;
   static uint8_t ULQ; // ( min 40 max 110)?
   static uint8_t DLQ; // 2 * RSSI in DB ?

   static quan::angle::deg    latitude ;
   static quan::angle::deg   longtitude ; 
   static quan::length::m     altitude;
   static quan::velocity::m_per_s airspeed;
};

#endif // FRSKY_PROTO_HPP_INCLUDED
 