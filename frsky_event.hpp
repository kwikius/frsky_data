#ifndef FRSKY_DATA_FRSKY_EVENT_HPP_INCLUDED
#define FRSKY_DATA_FRSKY_EVENT_HPP_INCLUDED

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

#include <wx/defs.h>
#include <wx/event.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_FRSKY_EVENT_LATITUDE, -1)
DECLARE_EVENT_TYPE(wxEVT_FRSKY_EVENT_LONGTITUDE, -1)
DECLARE_EVENT_TYPE(wxEVT_FRSKY_EVENT_ALTITUDE, -1)
DECLARE_EVENT_TYPE(wxEVT_FRSKY_EVENT_AIRSPEED, -1)
DECLARE_EVENT_TYPE(wxEVT_FRSKY_EVENT_RVLQ, -1)
DECLARE_EVENT_TYPE(wxEVT_SP_THREAD_ENDED,-1)
DECLARE_EVENT_TYPE(wxEVT_TENSOR_THREAD_ENDED,-1)
END_DECLARE_EVENT_TYPES()


struct frskyEvent : wxEvent
{
   frskyEvent(wxEventType EventType) : wxEvent(wxID_ANY, EventType){}
   wxEvent* Clone() const ;
};


#endif // FRSKY_DATA_FRSKY_EVENT_HPP_INCLUDED
