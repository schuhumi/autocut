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
        x_tmp = parseInt(mygamepad.axes[0]*30)
        y_tmp = parseInt(-mygamepad.axes[1]*30)

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
