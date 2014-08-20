/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */


var pingCtr = 1;
function PingWatchdog()
{
    pingCtr -= 1;
    if( pingCtr == 0)
    {
        pingCtr = 1;
        document.getElementById("pingSymbol").innerHTML = "Error";
    }
    else
        document.getElementById("pingSymbol").innerHTML = "Ok";
}
setInterval(PingWatchdog, 300);

function parseResponse (requestlist)
{
    mylog(requestlist)
    for (var i=0; i<requestlist.length; i++)
    {
        var requestsplit = requestlist[i].strip().split(':')
        requestsplit[requestsplit.length] = "dummy"
        command = requestsplit[0]
        value = requestsplit[1]
        //mylog(command+"::::"+value)
        if( command == "ping" )
            if( value == "ok" )
                pingCtr = 5;
        if( command == "get.battery.cell1.percentage" )
            document.getElementById('CELL1perc').value=value;
        if( command == "get.battery.cell2.percentage" )
            document.getElementById('CELL2perc').value=value;
        if( command == "get.battery.cell3.percentage" )
            document.getElementById('CELL3perc').value=value;
        if( command == "get.battery.cell4.percentage" )
            document.getElementById('CELL4perc').value=value;
        if( command == "get.battery.cell1.volt" )
            document.getElementById("CELL1volt").innerHTML = value+"V";
        if( command == "get.battery.cell2.volt" )
            document.getElementById("CELL2volt").innerHTML = value+"V";
        if( command == "get.battery.cell3.volt" )
            document.getElementById("CELL3volt").innerHTML = value+"V";
        if( command == "get.battery.cell4.volt" )
            document.getElementById("CELL4volt").innerHTML = value+"V";
        if( command == "get.motor.status.angle" )
        {
            document.getElementById("DriveAngleangle").innerHTML = value+"°";
            document.getElementById("DriveAngleperc").value=50-parseInt(value);
        }
        
        if( command == "get.motor.status.FLpower" )
        {
            document.getElementById("FLpowernum").innerHTML = value;
            document.getElementById("FLpowerperc").value=50+parseInt(value)/2;
        }
        if( command == "get.motor.status.FLspeed" )
        {
            document.getElementById("FLspeednum").innerHTML = value;
            document.getElementById("FLspeedperc").value=50+parseInt(value);
        }
        if( command == "get.motor.status.FLspdmeas" )
        {
            document.getElementById("FLspdmeasnum").innerHTML = value;
            document.getElementById("FLspdmeasperc").value=50+parseInt(value);
        }
        
        if( command == "get.motor.status.FRpower" )
        {
            document.getElementById("FRpowernum").innerHTML = value;
            document.getElementById("FRpowerperc").value=50+parseInt(value)/2;
        }
        if( command == "get.motor.status.FRspeed" )
        {
            document.getElementById("FRspeednum").innerHTML = value;
            document.getElementById("FRspeedperc").value=50+parseInt(value);
        }
        if( command == "get.motor.status.FRspdmeas" )
        {
            document.getElementById("FRspdmeasnum").innerHTML = value;
            document.getElementById("FRspdmeasperc").value=50+parseInt(value);
        }
        
        if( command == "get.motor.status.BLpower" )
        {
            document.getElementById("BLpowernum").innerHTML = value;
            document.getElementById("BLpowerperc").value=50+parseInt(value)/2;
        }
        if( command == "get.motor.status.BLspeed" )
        {
            document.getElementById("BLspeednum").innerHTML = value;
            document.getElementById("BLspeedperc").value=50+parseInt(value);
        }
        if( command == "get.motor.status.BLspdmeas" )
        {
            document.getElementById("BLspdmeasnum").innerHTML = value;
            document.getElementById("BLspdmeasperc").value=50+parseInt(value);
        }
        
        if( command == "get.motor.status.BRpower" )
        {
            document.getElementById("BRpowernum").innerHTML = value;
            document.getElementById("BRpowerperc").value=50+parseInt(value)/2;
        }
        if( command == "get.motor.status.BRspeed" )
        {
            document.getElementById("BRspeednum").innerHTML = value;
            document.getElementById("BRspeedperc").value=50+parseInt(value);
        }
        if( command == "get.motor.status.BRspdmeas" )
        {
            document.getElementById("BRspdmeasnum").innerHTML = value;
            document.getElementById("BRspdmeasperc").value=50+parseInt(value);
        }
        
        if( command == "get.mower.status.FET1power" )
        {
            document.getElementById("FET1powernum").innerHTML = value;
            document.getElementById("FET1powerperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET1speed" )
        {
            document.getElementById("FET1speednum").innerHTML = value;
            document.getElementById("FET1speedperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET1spdmeas" )
        {
            document.getElementById("FET1spdmeasnum").innerHTML = value;
            document.getElementById("FET1spdmeasperc").value=parseInt(value);
        }
        
        if( command == "get.mower.status.FET2power" )
        {
            document.getElementById("FET2powernum").innerHTML = value;
            document.getElementById("FET2powerperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET2speed" )
        {
            document.getElementById("FET2speednum").innerHTML = value;
            document.getElementById("FET2speedperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET2spdmeas" )
        {
            document.getElementById("FET2spdmeasnum").innerHTML = value;
            document.getElementById("FET2spdmeasperc").value=parseInt(value);
        }
        
        if( command == "get.mower.status.FET3power" )
        {
            document.getElementById("FET3powernum").innerHTML = value;
            document.getElementById("FET3powerperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET3speed" )
        {
            document.getElementById("FET3speednum").innerHTML = value;
            document.getElementById("FET3speedperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET3spdmeas" )
        {
            document.getElementById("FET3spdmeasnum").innerHTML = value;
            document.getElementById("FET3spdmeasperc").value=parseInt(value);
        }
        
        if( command == "get.mower.status.FET4power" )
        {
            document.getElementById("FET4powernum").innerHTML = value;
            document.getElementById("FET4powerperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET4speed" )
        {
            document.getElementById("FET4speednum").innerHTML = value;
            document.getElementById("FET4speedperc").value=parseInt(value);
        }
        if( command == "get.mower.status.FET4spdmeas" )
        {
            document.getElementById("FET4spdmeasnum").innerHTML = value;
            document.getElementById("FET4spdmeasperc").value=parseInt(value);
        }
        
        
        if( command == "get.system.loadavg" )
        {
            document.getElementById("LoadAVGnum").innerHTML = value;
            document.getElementById("LoadAVGperc").value=parseFloat(value)*100;
        }
        if( command == "get.system.ram" )
        {
            document.getElementById("RAMnum").innerHTML = parseFloat(value)*100+"%";
            document.getElementById("RAMperc").value=parseFloat(value)*100;
        }
        if( command == "get.system.uptime" )
        {
            document.getElementById("uptime").innerHTML = requestsplit[1]+":"+requestsplit[2]+":"+requestsplit[3];
        }
    }
    
}
if(typeof(String.prototype.strip) === "undefined")
{
    String.prototype.strip = function() 
    {
        return String(this).replace(/^\s+|\s+$/g, '');
    };
}


function Robot_move(direction)
{
    if(direction == "forwards")
        WebSocket_Send('set.motor.left:20 \n set.motor.right:20')
    if(direction == "backwards")
        WebSocket_Send('set.motor.left:-20 \n set.motor.right:-20')
    if(direction == "left")
        WebSocket_Send('set.motor.left:-20 \n set.motor.right:20')
    if(direction == "right")
        WebSocket_Send('set.motor.left:20 \n set.motor.right:-20')
    if(direction == "stop")
        WebSocket_Send('set.motor.left:0 \n set.motor.right:0')
}


function verifyKey(e)
{
    var keycode;
    if (window.event)
        keycode = window.event.keyCode;
    else if (e)
        keycode = e.which;

    if      (keycode == 38)
        Robot_move('forwards')
    else if (keycode == 40)
        Robot_move('backwards')
    else if (keycode == 37)
        Robot_move('left')
    else if (keycode == 39)
        Robot_move('right')
    else if (keycode == 32)
        Robot_move('stop');
    else
        mylog(keycode)

    return false;
}

setInterval(Robot_ping, 300);
function Robot_ping()
{
    WebSocket_Send('ping')
}

setInterval(Robot_BatUpdate, 2000);
function Robot_BatUpdate()
{
    WebSocket_Send('get.battery.cell1.percentage \n get.battery.cell2.percentage \n get.battery.cell3.percentage \n get.battery.cell4.percentage \n \
    get.battery.cell1.volt \n get.battery.cell2.volt \n get.battery.cell3.volt \n get.battery.cell4.volt')
}

setInterval(Robot_DriveStatusUpdate, 1000);
function Robot_DriveStatusUpdate ()
{
    WebSocket_Send('get.motor.status.angle \
    \n get.motor.status.FLpower \n get.motor.status.FLspeed \n get.motor.status.FLspdmeas \
    \n get.motor.status.FRpower \n get.motor.status.FRspeed \n get.motor.status.FRspdmeas \
    \n get.motor.status.BLpower \n get.motor.status.BLspeed \n get.motor.status.BLspdmeas \
    \n get.motor.status.BRpower \n get.motor.status.BRspeed \n get.motor.status.BRspdmeas')
}

setInterval(Robot_MowerStatusUpdate, 1000);
function Robot_MowerStatusUpdate ()
{
    WebSocket_Send('\
    \n get.mower.status.FET1power \n get.mower.status.FET1speed \n get.mower.status.FET1spdmeas \
    \n get.mower.status.FET2power \n get.mower.status.FET2speed \n get.mower.status.FET2spdmeas \
    \n get.mower.status.FET3power \n get.mower.status.FET3speed \n get.mower.status.FET3spdmeas \
    \n get.mower.status.FET4power \n get.mower.status.FET4speed \n get.mower.status.FET4spdmeas')
    
}

setInterval(Robot_SystemStatusUpdate, 2000);
function Robot_SystemStatusUpdate ()
{
    WebSocket_Send('get.system.loadavg \n get.system.uptime \n get.system.ram')
}


function Robot_SetMowerSpeed ()
{
    var mowspeed;
    mowspeed = document.getElementById("MowSpeedSelector").value.toString();
    WebSocket_Send('set.mower.FET1:'+mowspeed+' \n set.mower.FET2:'+mowspeed);
    
}
