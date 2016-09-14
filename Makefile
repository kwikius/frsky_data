

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

QUAN_INCLUDE_PATH = quan-trunk

local_sources = frsky_dataApp.cpp frsky_dataMain.cpp frsky_event.cpp frsky_serial_port.cpp  \
sp_thread.cpp tensor_data.cpp tensor_proto.cpp

INCLUDES = -I$(QUAN_INCLUDE_PATH) 

local_objects = $(patsubst %.cpp,%.o,$(local_sources)) 

objects = $(local_objects) serial_port.o

CC = g++
LD = g++

CFLAGS = -Wall -std=c++11

LFLAGS =

.PHONY : clean all quan-trunk

all : quan-trunk frsky_data.exe

quan-trunk:
	@if [ ! "`ls -A quan-trunk`" ] ; then \
		printf "############## ERROR ##############\n"; \
		printf "    quan-trunk is not initialized.\n"; \
		printf "    Please run:\n"; \
		printf "        $$ git submodule init\n"; \
		printf "        $$ git submodule update\n"; \
		printf "    before running make.\n"; \
		printf "############## ERROR ##############\n"; \
		exit 1; \
		fi

$(local_objects) : %.o : %.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ `wx-config --cppflags`

serial_port.o : $(QUAN_INCLUDE_PATH)/quan_matters/src/serial_port.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ `wx-config --cppflags`

frsky_data.exe : $(objects)
	$(LD) $(CFLAGS) $(LFLAGS) -o frsky_data.exe $(objects)  `wx-config --libs`

clean:
	-rm -rf *.o *.exe
