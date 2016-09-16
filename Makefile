

# Copyright (c) 2012-2016 Andy Little 

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

# You need to specify the path on the command line
# or as an environment variable
# using this variable name
# see the info below for more details
QUAN_INCLUDE_PATH =

define quan_path_not_defined_help_string

--------------------------------HELP----------------------------

To build this app requires QUAN_INCLUDE_PATH to be defined to the path to the quan library.

You can download quan as a zip from:

   https://github.com/kwikius/quan-trunk/archive/master.zip  

Unzip it to your favourite libs dir and then do: ( where my_path/to/quan-trunk is replced by your path)

   "$$ make QUAN_INCLUDE_PATH=my_path/to/quan-trunk"

 or do:

   "$$ export QUAN_INCLUDE_PATH=my_path/to/quan-trunk"
   "$$ make"
 
 or open the Makefile in this directory and define it there.

 or try write a script to achieve the above

--------------------------------HELP---------------------------


endef

local_sources = frsky_dataApp.cpp frsky_dataMain.cpp frsky_event.cpp frsky_serial_port.cpp  \
sp_thread.cpp tensor_data.cpp tensor_proto.cpp

INCLUDES = -I$(QUAN_INCLUDE_PATH) 

local_objects = $(patsubst %.cpp,%.o,$(local_sources)) 

objects = $(local_objects) serial_port.o

CC = g++
LD = g++

CFLAGS = -Wall -std=c++11

LFLAGS =

fun_check_includes = $(if $(QUAN_INCLUDE_PATH),,$(error $(quan_path_not_defined_help_string)))
   
.PHONY : clean all

all :  check_includes frsky_data.exe

check_includes:
	@:$(call fun_check_includes)

$(local_objects) : %.o : %.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ `wx-config --cppflags`

serial_port.o : $(QUAN_INCLUDE_PATH)/quan_matters/src/serial_port.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ `wx-config --cppflags`

frsky_data.exe : $(objects)
	$(LD) $(CFLAGS) $(LFLAGS) -o frsky_data.exe $(objects)  `wx-config --libs`

clean:
	-rm -rf *.o *.exe
