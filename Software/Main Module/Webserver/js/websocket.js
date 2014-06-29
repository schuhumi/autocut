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
