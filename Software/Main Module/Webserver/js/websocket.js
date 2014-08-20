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


var ws;
var ws_status = "closed";
function set_ws_status(status)
{
    ws_status = status;
    document.getElementById("connectionStatus").innerHTML = status;
    
}

function WebSocket_Close()
{
    ws.close();
}
function WebSocket_Open()
{
    ws = new WebSocket("ws://"+location.host+":8888");
    mylog(ws)
    ws.onerror = function(error)
    {
        mylog('Error detected: ' + error);
    }
    ws.onopen = function()
    {
        mylog('Connection opened!');
        set_ws_status("opened")
    }
    ws.onclose = function()
    {
        mylog('Connection closed!');
        set_ws_status("closed")
    }
    ws.onmessage = function(e)
    {
        var message = e.data;
        mylog('Received message: >>>'+message+'<<<');
        parseResponse(message.split("\n"))
    }
}
function WebSocket_Send(data)
{
    ws.send(data);
    mylog('Sent data: >>>'+data+'<<<')
}
