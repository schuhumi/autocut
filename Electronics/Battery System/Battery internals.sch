EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:Battery internals-cache
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_2 P3
U 1 1 53FC93C6
P 3400 4950
F 0 "P3" V 3350 4950 40  0000 C CNN
F 1 "BattPower" V 3550 4900 40  0000 C CNN
F 2 "Battery System:PowerConnect_Handsolder" H 3400 4950 60  0001 C CNN
F 3 "" H 3400 4950 60  0000 C CNN
	1    3400 4950
	-1   0    0    1   
$EndComp
$Comp
L CONN_5 P4
U 1 1 53FC9493
P 3400 5400
F 0 "P4" V 3350 5400 50  0000 C CNN
F 1 "BattBalance" V 3550 5400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 3400 5400 60  0001 C CNN
F 3 "" H 3400 5400 60  0000 C CNN
	1    3400 5400
	-1   0    0    1   
$EndComp
$Comp
L CONN_6 P6
U 1 1 53FC9565
P 5600 7100
F 0 "P6" V 5550 7100 60  0000 C CNN
F 1 "ExtPower" V 5650 6400 60  0000 C CNN
F 2 "Battery System:Connector_MPX_6Pin" H 5600 7100 60  0001 C CNN
F 3 "" H 5600 7100 60  0000 C CNN
	1    5600 7100
	0    -1   1    0   
$EndComp
$Comp
L CONN_6 P5
U 1 1 53FC995E
P 4900 7100
F 0 "P5" V 4850 7100 60  0000 C CNN
F 1 "ExtBalance" V 4950 7850 60  0000 C CNN
F 2 "Battery System:Connector_MPX_6Pin" H 4900 7100 60  0001 C CNN
F 3 "" H 4900 7100 60  0000 C CNN
	1    4900 7100
	0    -1   1    0   
$EndComp
Text Label 3900 4850 0    60   ~ 0
Power-
Text Label 3900 5050 0    60   ~ 0
Power+
Wire Wire Line
	4650 4350 4650 6750
Wire Wire Line
	5150 4350 5150 6750
Wire Wire Line
	4750 4350 4750 6750
Wire Wire Line
	4850 4350 4850 6750
Wire Wire Line
	4950 4350 4950 6750
Wire Wire Line
	5050 4350 5050 6750
Wire Wire Line
	5350 6750 5350 6700
Wire Wire Line
	5350 6700 5550 6700
Wire Wire Line
	5450 4400 5450 6750
Wire Wire Line
	5550 6700 5550 6750
Connection ~ 5450 6700
Wire Wire Line
	5650 6750 5650 6700
Wire Wire Line
	5650 6700 5850 6700
Wire Wire Line
	5750 4400 5750 6750
Wire Wire Line
	5850 6700 5850 6750
Connection ~ 5750 6700
Wire Wire Line
	3750 4850 5450 4850
Connection ~ 5450 4850
Wire Wire Line
	3750 5050 5750 5050
Connection ~ 5750 5050
Wire Wire Line
	3800 5200 4650 5200
Connection ~ 4650 5200
Wire Wire Line
	3800 5300 4750 5300
Connection ~ 4750 5300
Wire Wire Line
	3800 5400 4850 5400
Connection ~ 4850 5400
Wire Wire Line
	3800 5500 4950 5500
Connection ~ 4950 5500
Wire Wire Line
	3800 5600 5050 5600
Connection ~ 5050 5600
$Comp
L CONN_6 P1
U 1 1 53FCA28B
P 4900 4000
F 0 "P1" V 4850 4000 60  0000 C CNN
F 1 "InternBalance" V 4950 4800 60  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x06" H 4900 4000 60  0001 C CNN
F 3 "" H 4900 4000 60  0000 C CNN
	1    4900 4000
	0    -1   -1   0   
$EndComp
$Comp
L CONN_2 P2
U 1 1 53FCA460
P 5600 4000
F 0 "P2" V 5550 4000 40  0000 C CNN
F 1 "InternPower" V 5650 3500 40  0000 C CNN
F 2 "Battery System:PowerConnect_Handsolder" H 5600 4000 60  0001 C CNN
F 3 "" H 5600 4000 60  0000 C CNN
	1    5600 4000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5450 4400 5500 4400
Wire Wire Line
	5500 4400 5500 4350
Wire Wire Line
	5750 4400 5700 4400
Wire Wire Line
	5700 4400 5700 4350
$EndSCHEMATC
