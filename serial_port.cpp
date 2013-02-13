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


#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdexcept>
#include "serial_port.hpp"

serial_port::serial_port( const char* filename)
: m_filename{filename},
m_fd{0},
m_old_termios_saved(false), 
m_good_state(false)
{

}

bool serial_port::good()const
{
  return m_good_state == true;
}

bool serial_port::is_deleteable() const
{
   return true;
}

void serial_port::init()
{

    this->m_fd = open(this->m_filename.c_str(),O_RDWR | O_NOCTTY );
  
    if (this->m_fd < 0) {
        perror(this->m_filename.c_str());
        this->m_good_state = false;
       return;
    }

    bzero(&m_old_termios,sizeof(m_old_termios));

    termios new_termios; // new serial port settings
    bzero(&new_termios,sizeof(new_termios)); //set all serial port settings to zero
 
    new_termios.c_cflag |= CS8;      // set 8 data bits
    new_termios.c_cflag |= CLOCAL;   // doesnt own port
    new_termios.c_cflag |= CREAD;    // enable reading serial port also
 
    new_termios.c_cc[VTIME] =5;      // wait 5 tenths of a second for between characters
    new_termios.c_cc[VMIN] = 1;      // min number of characters to read before return
 
    // set baud rate for i/o
    if( cfsetospeed(&new_termios,B9600) < 0) {
        perror("failed to set output baudrate");
        this->cleanup();
        throw std::logic_error("open serial port failed");
 
    }
    if(cfsetispeed(&new_termios,B9600) < 0) {
        perror("failed to set input baudrate");
        this->cleanup();
        throw std::logic_error("open serial port failed");
    }
 
    if(tcgetattr(this->m_fd,&this->m_old_termios) < 0) {
        perror(this->m_filename.c_str());
        this->cleanup();
        throw std::logic_error("open serial port failed");
    }
    this->m_old_termios_saved = true;
 
    tcflush(this->m_fd, TCIOFLUSH);
    tcsetattr(this->m_fd,TCSANOW,&new_termios);
    this->m_good_state = true;
}

size_t serial_port::in_avail()
{
   int num_in_buffer;
   if ( ::ioctl(this->m_fd, FIONREAD, &num_in_buffer) >=0){
      return num_in_buffer;
   }else{
      perror("failed to get serial port num in buffer status");
      this->cleanup();
      throw std::logic_error("get serial port num in buffer failed");
   }
}



ssize_t serial_port::read( data_type* buf,size_t num)
{
   return ::read(this->m_fd,buf,num);
}

ssize_t serial_port::write(const data_type* buf, size_t num) 
{
    return ::write(this->m_fd,buf,num);
}

serial_port::~serial_port()
{
  this->cleanup();
}
void serial_port::cleanup()
{
   if (this->m_fd > 0) {
     if (this->m_old_termios_saved) {
         tcflush(this->m_fd,TCIFLUSH);
         tcsetattr(this->m_fd,TCSANOW,&m_old_termios);
     }
     close(this->m_fd);
     this->m_fd = 0;
   }
}





