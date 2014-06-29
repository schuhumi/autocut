// Gamepad support:
var mygamepad;
var gamepadconnected = false;
var x = 0;
var y = 0;
var x_tmp = 0;
var y_tmp = 0;
function gamepadConnected(e) {
    gamepadconnected = true;
    mygamepad = e.gamepad

    function pollAxis() {
        //mylog(" Axis " + mygamepad.axes);
        x_tmp = parseInt(mygamepad.axes[3]*30)
        y_tmp = parseInt(-mygamepad.axes[2]*30)

        x = x_tmp;
        y = y_tmp;
        //mylog(x+"  :  "+y)
    }
    setInterval(pollAxis, 30);


    function GamepadRobotControl()
    {
        if ( gamepadconnected == true)
        {
            left = y+x;
            right = y-x;
            if(left>30)
                left = 30;
            if(left<-30)
                left = -30;
            if(right>30)
                right = 30;
            if(right<-30)
                right = -30;
            WebSocket_Send('set.motor.left:'+(left)+' \n set.motor.right:'+(right))

        }

    }
    setInterval(GamepadRobotControl, 400);

}

function gamepadDisconnected() {
    gamepadconnected = false;
}

window.addEventListener("gamepadconnected", gamepadConnected, false);
window.addEventListener("gamepaddisconnected", gamepadDisconnected, false);
