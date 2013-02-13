
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

#include "frsky_event.hpp"

DEFINE_EVENT_TYPE(wxEVT_FRSKY_EVENT_LATITUDE)
DEFINE_EVENT_TYPE(wxEVT_FRSKY_EVENT_LONGTITUDE)
DEFINE_EVENT_TYPE(wxEVT_FRSKY_EVENT_ALTITUDE)
DEFINE_EVENT_TYPE(wxEVT_FRSKY_EVENT_AIRSPEED)
DEFINE_EVENT_TYPE(wxEVT_FRSKY_EVENT_RVLQ)
DEFINE_EVENT_TYPE(wxEVT_SP_THREAD_ENDED)
DEFINE_EVENT_TYPE(wxEVT_TENSOR_THREAD_ENDED)

wxEvent* frskyEvent::Clone() const 
{ return new frskyEvent(*this);}
