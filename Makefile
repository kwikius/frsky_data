

# Copyright (c) 2012-2013 Andy Little 

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>

sources = frsky_dataApp.cpp frsky_dataMain.cpp frsky_event.cpp frsky_serial_port.cpp serial_port.cpp \
sp_thread.cpp tensor_data.cpp tensor_proto.cpp

QUAN_INCLUDE_PATH = /home/andy/website/quan-trunk/

INCLUDES = -I$(QUAN_INCLUDE_PATH)

objects = $(patsubst %.cpp,%.o,$(sources)) 

CC = g++
LD = g++

CFLAGS = -Wall -std=c++11

LFLAGS =

.PHONY : clean all

all : frsky_data.exe

protocol.o : $(QUAN_INCLUDE_PATH)quan_matters/src/uav/frsky2wsp/protocol.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 

$(objects) : %.o : %.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ `wx-config --cppflags`

frsky_data.exe : $(objects) protocol.o
	$(LD) $(CFLAGS) $(LFLAGS) -o frsky_data.exe $(objects) protocol.o `wx-config --libs`

clean:
	-rm -rf *.o *.exe
