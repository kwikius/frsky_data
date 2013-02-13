#ifndef FRSKY_DATA_SP_THREAD_HPP_INCLUDED
#define FRSKY_DATA_SP_THREAD_HPP_INCLUDED

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

#include <stdexcept>
#include <functional>
#include <wx/thread.h>
#include <string>
#include <quan/uav/frsky2wsp/protocol.hpp>

class frsky_dataDialog;

struct SP_thread : wxThread
{

   SP_thread(frsky_dataDialog* wnd,const char* filename);
   ~SP_thread();

   void * Entry();

   typedef void(*data_fun_ptr)(unsigned char (& buf)[9]) ;

   void set_fun(uint8_t fun_id,data_fun_ptr fun);
 
   private :

      void  call(unsigned char (& buf)[9])const;
     
        static int frsky_proto_map(uint8_t id) 
        {
        if( (id < 0xF8 ) || (id > 0xFE) ) {
            throw std::out_of_range("FrSky2WSP PRIM id out of range");
        }
        return 254 - id;
    };
    //use this function to init unused callbacks
    static void sink( unsigned char (& buf)[9]) {}
 
    static constexpr uint32_t num_functions = FrSky2WSP::CommandID::RVLQ - FrSky2WSP::CommandID::ThrReq;
    data_fun_ptr data_functions[num_functions];

    frsky_dataDialog* m_frsky_dataDialog;
    std::string m_filename;
};


#endif // FRSKY_DATA_SP_THREAD_HPP_INCLUDED
