#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  

import fcntl
import time
import sys
import struct

I2C_STATUS              = 0
I2C_CMD                 = 1
I2C_CMD_ARG1    = 2
I2C_CMD_ARG2    = 3
I2C_CMD_ARG3    = 4
I2C_CMD_ARG4    = 5

IOCTL_I2C_SLAVE = 0x0703

devi2c = None

def OPEN(device):
        global devi2c
        #devi2c = open(device, 'r+', 1)
        devi2c = open(device,'wb+', buffering=0)
        
def SETSLAVE(address):
        global devi2c
        fcntl.ioctl(devi2c, IOCTL_I2C_SLAVE, address)
        
def CLOSE():
        global devi2c
        devi2c.close()

def WRITE(register, value):
        global devi2c
        devi2c.write(bytes(chr(register)+chr(value), 'UTF-8'))

def WRITECMD(cmd, arg1=0, arg2=0, arg3=0, arg4=0):
        global devi2c
        #devi2c.write(chr(I2C_CMD))
        #while ord(devi2c.read(1)) != NOCMD:     # Altes Kommando noch nicht abgearbeitet
                #devi2c.write(chr(I2C_CMD))
        while READ(I2C_CMD) != 0:
                dummy = 1
        WRITE(I2C_CMD_ARG1, arg1)
        WRITE(I2C_CMD_ARG2, arg2)
        WRITE(I2C_CMD_ARG3, arg3)
        WRITE(I2C_CMD_ARG4, arg4)
        WRITE(I2C_CMD, cmd)
        #devi2c.write(bytes(chr(I2C_CMD_ARG1)+chr(arg1)+chr(arg2)+chr(arg3)+chr(arg4), 'UTF-8'))
        #devi2c.write(bytes(chr(I2C_CMD)+chr(cmd), 'UTF-8'))
        
def WRITECMDWAIT(cmd, arg1=0, arg2=0, arg3=0, arg4=0):
        WRITECMD(cmd, arg1, arg2, arg3, arg4)
        while READ(I2C_CMD) != 0:
                dummy = 1

def READ(register):
        global devi2c
        try:
                devi2c.write(bytes(str(chr(register)), 'UTF-8'))
                #return ord(devi2c.read(1))
                tmp = devi2c.read(1)
                #print(tmp)
                return struct.unpack('B', tmp)[0]
        except:
                return 0
