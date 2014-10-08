
autoCut
=======

autoCut is an automatic lawn mower robot. It features:
 - Four weel drive with kink steering
 - Motorized mowing height leveling
 - Two 85 watt mowing motors, each equipped with two very sharp blades, able to snap back on hard obstacles
 - On the fly interchangeable LiPo-battery with voltage monitoring
 - Wifi network connection for web interface which includes gamepad remote control
 - Raspberry Pi as its main computer and head of the microcontroller i2c-network

progress
========

What it can do already:
-----------------------
 - [drive system] works pretty much brilliant
 - [power supply] Measures individual cell voltages and rings an audible alarm in case of undervoltage or improper connected battery. Handles battery changes without losing power for the control system
 - [main module] Raspberry works quite well with ArchLinux ARM and provides a webinterface on the network. Will be replaced by Cubieboard 2 since RasPi has hardware I2C issues
 - [mowing module] Runs the mowing motors with a reasonably decent rpm regulation algorithm. Electrical mowing height adjustment through web interface is implemented.

What it can't do yet:
---------------------
 - [power supply] Measure the current as well as the total battery voltage
 - [general] No environmental sensoring for automatic movement yet, so manual control is needed

Things to improve:
------------------
 - [power supply] design a proper pcb (its on a prototyping board so far)
 - [mowing module] Change FETs for ones with lower heat dissipation and enhance the rpm regulation algorithm

Preserved for the future:
-------------------------
 - Build an automatic battery changing and charging station
