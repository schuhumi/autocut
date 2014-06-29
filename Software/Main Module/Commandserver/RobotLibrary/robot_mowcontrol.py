from RobotLibrary import i2c as i2c

I2C_STATUS              = 0
I2C_CMD                 = 1
I2C_CMD_ARG1            = 2
I2C_CMD_ARG2            = 3
I2C_CMD_ARG3            = 4
I2C_CMD_ARG4            = 5
I2C_FET1power           = 6
I2C_FET1speed           = 7
I2C_FET1spdmeas         = 8
I2C_FET2power           = 9
I2C_FET2speed           = 10
I2C_FET2spdmeas         = 11
I2C_FET3power           = 12
I2C_FET3speed           = 13
I2C_FET3spdmeas         = 14
I2C_FET4power           = 15
I2C_FET4speed           = 16
I2C_FET4spdmeas         = 17
I2C_Apower              = 18
I2C_Aspeed              = 19
I2C_Aspdmeas            = 20
I2C_Bpower              = 21
I2C_Bspeed              = 22
I2C_Bspdmeas            = 23

NOCMD                   = 0
CMD_STOP                = 1
CMD_FET1SPD             = 2
CMD_FET2SPD             = 3
CMD_FET3SPD             = 4
CMD_FET4SPD             = 5

SLAVE_ADDRESS = 0x32

def STOP():
        i2c.SETSLAVE(SLAVE_ADDRESS)
        i2c.WRITECMDWAIT(CMD_STOP)
        
def SET_FET1(speed):
        i2c.SETSLAVE(SLAVE_ADDRESS)
        i2c.WRITECMDWAIT(CMD_FET1SPD, speed)

def SET_FET2(speed):
        i2c.SETSLAVE(SLAVE_ADDRESS)
        i2c.WRITECMDWAIT(CMD_FET2SPD, speed)
        
def SET_FET3(speed):
        i2c.SETSLAVE(SLAVE_ADDRESS)
        i2c.WRITECMDWAIT(CMD_FET3SPD, speed)
        
def SET_FET4(speed):
        i2c.SETSLAVE(SLAVE_ADDRESS)
        i2c.WRITECMDWAIT(CMD_FET4SPD, speed)
        
def GET_STATUS():
        i2c.SETSLAVE(SLAVE_ADDRESS)
        status  = {}
        status["FET1power"      ] = int(i2c.READ(I2C_FET1power   ))-100
        status["FET1speed"      ] = int(i2c.READ(I2C_FET1speed   ))
        status["FET1spdmeas"    ] = int(i2c.READ(I2C_FET1spdmeas ))-100
        status["FET2power"      ] = int(i2c.READ(I2C_FET2power   ))-100
        status["FET2speed"      ] = int(i2c.READ(I2C_FET2speed   ))
        status["FET2spdmeas"    ] = int(i2c.READ(I2C_FET2spdmeas ))-100
        status["FET3power"      ] = int(i2c.READ(I2C_FET3power   ))-100
        status["FET3speed"      ] = int(i2c.READ(I2C_FET3speed   ))
        status["FET3spdmeas"    ] = int(i2c.READ(I2C_FET3spdmeas ))-100
        status["FET4power"      ] = int(i2c.READ(I2C_FET4power   ))-100
        status["FET4speed"      ] = int(i2c.READ(I2C_FET4speed   ))
        status["FET4spdmeas"    ] = int(i2c.READ(I2C_FET4spdmeas ))-100
        status["Apower"         ] = int(i2c.READ(I2C_Apower      ))-100
        status["Aspeed"         ] = int(i2c.READ(I2C_Aspeed      ))-100
        status["Aspdmeas"       ] = int(i2c.READ(I2C_Aspdmeas    ))-100
        status["Bpower"         ] = int(i2c.READ(I2C_Bpower      ))-100
        status["Bspeed"         ] = int(i2c.READ(I2C_Bspeed      ))-100
        status["Bspdmeas"       ] = int(i2c.READ(I2C_Bspdmeas    ))-100
        
        return status
        
        
