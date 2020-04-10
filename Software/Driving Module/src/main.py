from pyb import I2C
from pyb import Pin

i2c = I2C(3, I2C.MASTER)
#i2c.scan()
#[39]
mcpAddr = 39

i2c.mem_write(0b0, mcpAddr, 0x01)

enable1 = Pin.cpu.C6
enable1.init(mode=Pin.OUT)

enable2 = Pin.cpu.C7
enable2.init(mode=Pin.OUT)

enable3 = Pin.cpu.C8
enable3.init(mode=Pin.OUT)

enable4 = Pin.cpu.B1
enable4.init(mode=Pin.OUT)

print("use 'enableN.value(<True/False>)' and")
print("'i2c.mem_write(0b<bits>, mcpAddr, 0x13)'")

