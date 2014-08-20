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

from RobotLibrary import i2c as i2c

I2C_STATUS		= 0
I2C_CMD			= 1
I2C_CMD_ARG1	= 2
I2C_CMD_ARG2	= 3
I2C_CMD_ARG3	= 4
I2C_CMD_ARG4	= 5
I2C_ANGLE		= 6
I2C_FLpower		= 7
I2C_FLspeed		= 8
I2C_FLspdmeas	= 9
I2C_FLdistance	= 10
I2C_FRpower		= 11
I2C_FRspeed		= 12
I2C_FRspdmeas	= 13
I2C_FRdistance	= 14
I2C_BLpower		= 15
I2C_BLspeed		= 16
I2C_BLspdmeas	= 17
I2C_BLdistance	= 18
I2C_BRpower		= 19
I2C_BRspeed		= 20
I2C_BRspdmeas	= 21
I2C_BRdistance	= 22
I2C_MoveMode	= 23

NOCMD			= 0
CMD_STOP		= 1
CMD_WRITEREG	= 2
CMD_USERANGLE	= 3
CMD_USERSPEED	= 4
CMD_MOTORL		= 5
CMD_MOTORR		= 6
CMD_MOVEMODE	= 7

MoveMode_Torque = 1
MoveMode_Speed	= 2

SLAVE_ADDRESS = 0x28

def STOP():
	i2c.SETSLAVE(SLAVE_ADDRESS)
	i2c.WRITECMDWAIT(CMD_STOP)
	
def MOVEMODE(movemode):
	i2c.SETSLAVE(SLAVE_ADDRESS)
	i2c.WRITECMDWAIT(CMD_MOVEMODE, movemode)

def SET_ANGLE(angle):
	i2c.SETSLAVE(SLAVE_ADDRESS)
	i2c.WRITECMDWAIT(CMD_USERANGLE, angle+100)

def SET_SPEED(speed):
	i2c.SETSLAVE(SLAVE_ADDRESS)
	i2c.WRITECMDWAIT(CMD_USERSPEED, speed+100)

def SET_MOTORS(left, right):
	i2c.SETSLAVE(SLAVE_ADDRESS)
	i2c.WRITECMDWAIT(CMD_MOTORL, left+100)
	i2c.WRITECMDWAIT(CMD_MOTORR, right+100)
	
def GET_STATUS():
	i2c.SETSLAVE(SLAVE_ADDRESS)
	status  = {}
	status["angle"] = int(i2c.READ(I2C_ANGLE))-100
	status["FLpower"]   = int(i2c.READ(I2C_FLpower))-100
	status["FLspeed"]   = int(i2c.READ(I2C_FLspeed))-100
	status["FLspdmeas"] = int(i2c.READ(I2C_FLspdmeas))-100
	status["FRpower"]   = int(i2c.READ(I2C_FRpower))-100
	status["FRspeed"]   = int(i2c.READ(I2C_FRspeed))-100
	status["FRspdmeas"] = int(i2c.READ(I2C_FRspdmeas))-100
	status["BLpower"]   = int(i2c.READ(I2C_BLpower))-100
	status["BLspeed"]   = int(i2c.READ(I2C_BLspeed))-100
	status["BLspdmeas"] = int(i2c.READ(I2C_BLspdmeas))-100
	status["BRpower"]   = int(i2c.READ(I2C_BRpower))-100
	status["BRspeed"]   = int(i2c.READ(I2C_BRspeed))-100
	status["BRspdmeas"] = int(i2c.READ(I2C_BRspdmeas))-100
	MoveMode = int(i2c.READ(I2C_MoveMode))
	if MoveMode == MoveMode_Speed:
		status["MoveMode"] = "Speed"
	elif MoveMode == MoveMode_Torque:
		status["MoveMode"] = "Torque"
	return status
	
	
