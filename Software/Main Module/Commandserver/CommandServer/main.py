#!/usr/bin/env python3

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


# import the libraries
import tornado.web
import tornado.websocket
import tornado.ioloop

from datetime import timedelta
import subprocess

import threading

from RobotLibrary import i2c as i2c
from RobotLibrary import robot_motorctrl as motor
from RobotLibrary import robot_servoctrl as servo
from RobotLibrary import robot_powersupply as power
from RobotLibrary import robot_mowcontrol as mow

debug = False

def printD (message):
    if debug:
        print(message)


def getUptime ():
    with open('/proc/uptime', 'r') as f:
        uptime_seconds = float(f.readline().split()[0])
        uptime = str(timedelta(seconds = uptime_seconds))
    return uptime
    
def getRAM ():
    stats = open("/proc/meminfo").readlines()
    total = int(stats[0].split(" ")[-2])
    free = int(stats[1].split(" ")[-2])
    return 1-(free/total)

class WebSocketHandler(tornado.websocket.WebSocketHandler):
    # the client connected
    def open(self):
        printD("New client connected")
        self.write_message("You are connected")

    # the client sent the message
    def on_message(self, message):
        printD("Got a message: >>>"+message+"<<<, parsing it..")
        requestlist = message.splitlines()
        self.write_message(RobotRequestHandler(requestlist))

    # client disconnected
    def on_close(self):
        printD("Client disconnected")

motorr = 0
motorl = 0
def RobotRequestHandler(requestlist):
    global motorr
    global motorl
    global watchdogctr
    returnlist = ""
    i2c.OPEN('/dev/i2c-2')
    mybat = False
    motorstatus = False
    mowerstatus = False
    for request in requestlist:
        request =  request.strip()
        requestsplit = request.split(':')
        requestsplit.append("dummy")
        command = requestsplit[0]
        value = requestsplit[1]
        if command == "ping":
            watchdogctr = 1000
            returnlist += "\n ping:ok"
        if "set.motor" in command:
            if command == "set.motor.left":
                motorl = int(value)
                returnlist += "\n set.motor.left:ok"
            if command == "set.motor.right":
                motorr = int(value)
                returnlist += "\n set.motor.right:ok"
            motor.SET_MOTORS(motorl, motorr)
        if command == "set.movemode":
            if value == "torque":
                motor.MOVEMODE(motor.MoveMode_Torque)
            if value == "speed":
                motor.MOVEMODE(motor.MoveMode_Speed)
        if "get.battery" in command:
            if mybat == False:
                mybat = power.GET_BAT()
            if command == "get.battery.cell1.volt":
                returnlist += "\n get.battery.cell1.volt:"+"{:1.3f}".format(mybat["CELL1"]["VOLT"])
            if command == "get.battery.cell2.volt":
                returnlist += "\n get.battery.cell2.volt:"+"{:1.3f}".format(mybat["CELL2"]["VOLT"])
            if command == "get.battery.cell3.volt":
                returnlist += "\n get.battery.cell3.volt:"+"{:1.3f}".format(mybat["CELL3"]["VOLT"])
            if command == "get.battery.cell4.volt":
                returnlist += "\n get.battery.cell4.volt:"+"{:1.3f}".format(mybat["CELL4"]["VOLT"])
            if command == "get.battery.cell1.percentage":
                returnlist += "\n get.battery.cell1.percentage:"+str(mybat["CELL1"]["PERCENTAGE"])
            if command == "get.battery.cell2.percentage":
                returnlist += "\n get.battery.cell2.percentage:"+str(mybat["CELL2"]["PERCENTAGE"])
            if command == "get.battery.cell3.percentage":
                returnlist += "\n get.battery.cell3.percentage:"+str(mybat["CELL3"]["PERCENTAGE"])
            if command == "get.battery.cell4.percentage":
                returnlist += "\n get.battery.cell4.percentage:"+str(mybat["CELL4"]["PERCENTAGE"])
        if "get.motor.status" in command:
            if motorstatus == False:
                motorstatus = motor.GET_STATUS()
            if command == "get.motor.status.angle":
                returnlist += "\n get.motor.status.angle:"+"{:+03}".format(motorstatus["angle"])
            if command == "get.motor.status.FLpower":
                returnlist += "\n get.motor.status.FLpower:"+"{:+003}".format(motorstatus["FLpower"])
            if command == "get.motor.status.FLspeed":
                returnlist += "\n get.motor.status.FLspeed:"+"{:+003}".format(motorstatus["FLspeed"])
            if command == "get.motor.status.FLspdmeas":
                returnlist += "\n get.motor.status.FLspdmeas:"+"{:+003}".format(motorstatus["FLspdmeas"])
            if command == "get.motor.status.FRpower":
                returnlist += "\n get.motor.status.FRpower:"+"{:+003}".format(motorstatus["FRpower"])
            if command == "get.motor.status.FRspeed":
                returnlist += "\n get.motor.status.FRspeed:"+"{:+003}".format(motorstatus["FRspeed"])
            if command == "get.motor.status.FRspdmeas":
                returnlist += "\n get.motor.status.FRspdmeas:"+"{:+003}".format(motorstatus["FRspdmeas"])
            if command == "get.motor.status.BLpower":
                returnlist += "\n get.motor.status.BLpower:"+"{:+003}".format(motorstatus["BLpower"])
            if command == "get.motor.status.BLspeed":
                returnlist += "\n get.motor.status.BLspeed:"+"{:+003}".format(motorstatus["BLspeed"])
            if command == "get.motor.status.BLspdmeas":
                returnlist += "\n get.motor.status.BLspdmeas:"+"{:+003}".format(motorstatus["BLspdmeas"])
            if command == "get.motor.status.BRpower":
                returnlist += "\n get.motor.status.BRpower:"+"{:+003}".format(motorstatus["BRpower"])
            if command == "get.motor.status.BRspeed":
                returnlist += "\n get.motor.status.BRspeed:"+"{:+003}".format(motorstatus["BRspeed"])
            if command == "get.motor.status.BRspdmeas":
                returnlist += "\n get.motor.status.BRspdmeas:"+"{:+003}".format(motorstatus["BRspdmeas"])   
        if command=="get.system.loadavg":
            returnlist += "\n get.system.loadavg:"+open("/proc/loadavg").readline().split(" ")[:3][0]
        if command=="get.system.uptime":
            returnlist += "\n get.system.uptime:"+str(getUptime()).split(".")[0]
        if command=="get.system.ram":
            returnlist += "\n get.system.ram:"+"{:02.1f}".format(getRAM())
        if command=="set.system.power":
            if value=="off":
                subprocess.Popen(["shutdown","-h","now"])
                returnlist += "\n set.system.power:ok"
        if command=="set.mower.FET1":
                mow.SET_FET1(int(value))
                returnlist += "\n set.mower.FET1:ok"
        if command=="set.mower.FET2":
                mow.SET_FET2(int(value))
                returnlist += "\n set.mower.FET2:ok"
        if command=="set.mower.FET3":
                mow.SET_FET1(int(value))
                returnlist += "\n set.mower.FET3:ok"
        if command=="set.mower.FET4":
                mow.SET_FET2(int(value))
                returnlist += "\n set.mower.FET4:ok"
        if "get.mower.status" in command:
            if mowerstatus == False:
                mowerstatus = mow.GET_STATUS()
            if command == "get.mower.status.FET1power":
                returnlist += "\n get.mower.status.FET1power:"+"{:+003}".format(mowerstatus["FET1power"])
            if command == "get.mower.status.FET1speed":
                returnlist += "\n get.mower.status.FET1speed:"+"{:+003}".format(mowerstatus["FET1speed"])
            if command == "get.mower.status.FET1spdmeas":
                returnlist += "\n get.mower.status.FET1spdmeas:"+"{:+003}".format(mowerstatus["FET1spdmeas"])
            if command == "get.mower.status.FET2power":
                returnlist += "\n get.mower.status.FET2power:"+"{:+003}".format(mowerstatus["FET2power"])
            if command == "get.mower.status.FET2speed":
                returnlist += "\n get.mower.status.FET2speed:"+"{:+003}".format(mowerstatus["FET2speed"])
            if command == "get.mower.status.FET2spdmeas":
                returnlist += "\n get.mower.status.FET2spdmeas:"+"{:+003}".format(mowerstatus["FET2spdmeas"])
            if command == "get.mower.status.FET3power":
                returnlist += "\n get.mower.status.FET3power:"+"{:+003}".format(mowerstatus["FET3power"])
            if command == "get.mower.status.FET3speed":
                returnlist += "\n get.mower.status.FET3speed:"+"{:+003}".format(mowerstatus["FET3speed"])
            if command == "get.mower.status.FET3spdmeas":
                returnlist += "\n get.mower.status.FET3spdmeas:"+"{:+003}".format(mowerstatus["FET3spdmeas"])
            if command == "get.mower.status.FET4power":
                returnlist += "\n get.mower.status.FET4power:"+"{:+003}".format(mowerstatus["FET4power"])
            if command == "get.mower.status.FET4speed":
                returnlist += "\n get.mower.status.FET4speed:"+"{:+003}".format(mowerstatus["FET4speed"])
            if command == "get.mower.status.FET4spdmeas":
                returnlist += "\n get.mower.status.FET4spdmeas:"+"{:+003}".format(mowerstatus["FET4spdmeas"])
    i2c.CLOSE()
    return returnlist

watchdogctr = 1000
def watchdog(): 
    global watchdogctr
    watchdogctr -= 100
    if watchdogctr <= 0:
        RobotRequestHandler('set.motor.left:0 \n set.motor.right:0'.splitlines())
        watchdogctr = 1000
    threading.Timer(0.1, watchdog).start()
watchdog()

# start a new WebSocket Application
# use "/" as the root, and the
# WebSocketHandler as our handler
application = tornado.web.Application([
    (r"/", WebSocketHandler),
])

# start the tornado server on port 8888
if __name__ == "__main__":
    application.listen(8888)
    tornado.ioloop.IOLoop.instance().start()
