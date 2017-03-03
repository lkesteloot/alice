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
LIBS:touchscreen_controller
LIBS:DABO-cache
EELAYER 25 0
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
L CONN_02X20 P3
U 1 1 58B8FF25
P 6525 2525
F 0 "P3" H 6525 3575 50  0000 C CNN
F 1 "CONN_02X20" V 6525 2525 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x20" H 6525 1575 60  0001 C CNN
F 3 "" H 6525 1575 60  0000 C CNN
	1    6525 2525
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X20 P1
U 1 1 58B8FFBA
P 1950 2425
F 0 "P1" H 1950 3475 50  0000 C CNN
F 1 "CONN_01X20" V 2050 2425 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x20" H 1950 2425 60  0001 C CNN
F 3 "" H 1950 2425 60  0000 C CNN
	1    1950 2425
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X20 P2
U 1 1 58B90009
P 2350 2425
F 0 "P2" H 2350 3475 50  0000 C CNN
F 1 "CONN_01X20" V 2450 2425 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x20" H 2350 2425 60  0001 C CNN
F 3 "" H 2350 2425 60  0000 C CNN
	1    2350 2425
	-1   0    0    1   
$EndComp
$Comp
L touchscreen_controller U1
U 1 1 58B90B31
P 4300 2025
F 0 "U1" H 4300 2025 60  0000 C CNN
F 1 "touchscreen_controller" H 4350 1375 60  0000 C CNN
F 2 "SMPTE610:Adafruit_SMPTE610" H 4300 2025 60  0001 C CNN
F 3 "" H 4300 2025 60  0000 C CNN
	1    4300 2025
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR01
U 1 1 58B91D13
P 6275 2075
F 0 "#PWR01" H 6275 1925 50  0001 C CNN
F 1 "+5V" H 6275 2215 50  0000 C CNN
F 2 "" H 6275 2075 60  0000 C CNN
F 3 "" H 6275 2075 60  0000 C CNN
	1    6275 2075
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR03
U 1 1 58B91DC3
P 6775 2075
F 0 "#PWR03" H 6775 1825 50  0001 C CNN
F 1 "GND" H 6775 1925 50  0000 C CNN
F 2 "" H 6775 2075 60  0000 C CNN
F 3 "" H 6775 2075 60  0000 C CNN
	1    6775 2075
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR04
U 1 1 58B91DE1
P 6775 2975
F 0 "#PWR04" H 6775 2725 50  0001 C CNN
F 1 "GND" H 6775 2825 50  0000 C CNN
F 2 "" H 6775 2975 60  0000 C CNN
F 3 "" H 6775 2975 60  0000 C CNN
	1    6775 2975
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR05
U 1 1 58B91E02
P 4900 1525
F 0 "#PWR05" H 4900 1375 50  0001 C CNN
F 1 "+5V" H 4900 1665 50  0000 C CNN
F 2 "" H 4900 1525 60  0000 C CNN
F 3 "" H 4900 1525 60  0000 C CNN
	1    4900 1525
	0    1    1    0   
$EndComp
$Comp
L GND #PWR06
U 1 1 58B91E22
P 4900 1625
F 0 "#PWR06" H 4900 1375 50  0001 C CNN
F 1 "GND" H 4900 1475 50  0000 C CNN
F 2 "" H 4900 1625 60  0000 C CNN
F 3 "" H 4900 1625 60  0000 C CNN
	1    4900 1625
	0    -1   -1   0   
$EndComp
Text GLabel 1750 1475 0    60   Input ~ 0
R0
Text GLabel 1750 1575 0    60   Input ~ 0
R1
Text GLabel 1750 1675 0    60   Input ~ 0
R2
Text GLabel 1750 1775 0    60   Input ~ 0
R3
Text GLabel 1750 1875 0    60   Input ~ 0
R4
Text GLabel 1750 1975 0    60   Input ~ 0
R5
Text GLabel 1750 2075 0    60   Input ~ 0
R6
Text GLabel 1750 2175 0    60   Input ~ 0
R7
Text GLabel 1750 2275 0    60   Input ~ 0
B0
Text GLabel 1750 2375 0    60   Input ~ 0
B1
Text GLabel 1750 2475 0    60   Input ~ 0
B2
Text GLabel 1750 2575 0    60   Input ~ 0
B3
Text GLabel 1750 2675 0    60   Input ~ 0
B4
Text GLabel 1750 2775 0    60   Input ~ 0
B5
Text GLabel 1750 2875 0    60   Input ~ 0
B6
Text GLabel 1750 2975 0    60   Input ~ 0
B7
Text GLabel 1750 3075 0    60   BiDi ~ 0
X+
Text GLabel 1750 3175 0    60   BiDi ~ 0
Y-
Text GLabel 1750 3275 0    60   BiDi ~ 0
X-
Text GLabel 1750 3375 0    60   BiDi ~ 0
Y+
$Comp
L +5V #PWR?
U 1 1 58B92C0F
P 2550 1475
F 0 "#PWR?" H 2550 1325 50  0001 C CNN
F 1 "+5V" H 2550 1615 50  0000 C CNN
F 2 "" H 2550 1475 60  0000 C CNN
F 3 "" H 2550 1475 60  0000 C CNN
	1    2550 1475
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 58B92C2F
P 2550 1575
F 0 "#PWR?" H 2550 1325 50  0001 C CNN
F 1 "GND" H 2550 1425 50  0000 C CNN
F 2 "" H 2550 1575 60  0000 C CNN
F 3 "" H 2550 1575 60  0000 C CNN
	1    2550 1575
	0    -1   -1   0   
$EndComp
Text GLabel 2550 2075 2    60   Input ~ 0
G0
Text GLabel 2550 2175 2    60   Input ~ 0
G1
Text GLabel 2550 2275 2    60   Input ~ 0
G2
Text GLabel 2550 2375 2    60   Input ~ 0
G3
Text GLabel 2550 2475 2    60   Input ~ 0
G4
Text GLabel 2550 2575 2    60   Input ~ 0
G5
Text GLabel 2550 2675 2    60   Input ~ 0
G6
Text GLabel 2550 2775 2    60   Input ~ 0
G7
$Comp
L GND #PWR?
U 1 1 58B92D1D
P 2550 2875
F 0 "#PWR?" H 2550 2625 50  0001 C CNN
F 1 "GND" H 2550 2725 50  0000 C CNN
F 2 "" H 2550 2875 60  0000 C CNN
F 3 "" H 2550 2875 60  0000 C CNN
	1    2550 2875
	0    -1   -1   0   
$EndComp
Text GLabel 2550 3075 2    60   Input ~ 0
DISPLAY_ON
Text GLabel 2550 3175 2    60   Input ~ 0
HSYNC
Text GLabel 2550 3275 2    60   Input ~ 0
VSYNC
Text GLabel 2550 3375 2    60   Input ~ 0
DISPLAY_ENABLE
Text GLabel 3800 1875 0    60   BiDi ~ 0
X+
Text GLabel 3800 1975 0    60   BiDi ~ 0
Y-
Text GLabel 3800 2075 0    60   BiDi ~ 0
X-
Text GLabel 3800 2175 0    60   BiDi ~ 0
Y+
Text GLabel 6275 1575 0    60   Input ~ 0
R0
Text GLabel 6275 1675 0    60   Input ~ 0
R1
Text GLabel 6275 1775 0    60   Input ~ 0
R2
Text GLabel 6275 1875 0    60   Input ~ 0
R3
Text GLabel 6275 1975 0    60   Input ~ 0
R4
Text GLabel 6275 2175 0    60   Input ~ 0
R5
Text GLabel 6275 2275 0    60   Input ~ 0
R6
Text GLabel 6275 2375 0    60   Input ~ 0
R7
Text GLabel 6275 2475 0    60   Input ~ 0
B0
Text GLabel 6275 2575 0    60   Input ~ 0
B1
Text GLabel 6275 2675 0    60   Input ~ 0
B2
Text GLabel 6275 2775 0    60   Input ~ 0
B3
Text GLabel 6275 2875 0    60   Input ~ 0
B4
Text GLabel 6275 3075 0    60   Input ~ 0
B5
NoConn ~ 6275 2975
NoConn ~ 2550 1675
NoConn ~ 2550 1775
NoConn ~ 2550 1875
NoConn ~ 2550 1975
Text GLabel 6275 3175 0    60   Input ~ 0
B6
Text GLabel 6275 3275 0    60   Input ~ 0
B7
Text GLabel 4900 2425 2    60   Input ~ 0
SCL
Text GLabel 4900 2525 2    60   BiDi ~ 0
SDA
Text GLabel 6775 3475 2    60   BiDi ~ 0
SDA
Text GLabel 6275 3475 0    60   Output ~ 0
SCL
Text GLabel 2550 2975 2    60   Input ~ 0
VGA_CLK
Text GLabel 6775 1575 2    60   Input ~ 0
G0
Text GLabel 6775 1675 2    60   Input ~ 0
G1
Text GLabel 6775 1775 2    60   Input ~ 0
G2
Text GLabel 6775 1875 2    60   Input ~ 0
G3
Text GLabel 6775 1975 2    60   Input ~ 0
G4
Text GLabel 6775 2175 2    60   Input ~ 0
G5
Text GLabel 6775 2275 2    60   Input ~ 0
G6
Text GLabel 6775 2375 2    60   Input ~ 0
G7
Text GLabel 6775 2475 2    60   Input ~ 0
VGA_CLK
Text GLabel 6775 2575 2    60   Input ~ 0
DISPLAY_ON
Text GLabel 6775 2675 2    60   Input ~ 0
HSYNC
Text GLabel 6775 2775 2    60   Input ~ 0
VSYNC
Text GLabel 6775 2875 2    60   Input ~ 0
DISPLAY_ENABLE
NoConn ~ 4900 1725
NoConn ~ 4900 1825
NoConn ~ 4900 1925
NoConn ~ 4900 2125
$Comp
L GND #PWR?
U 1 1 58B93293
P 4900 2225
F 0 "#PWR?" H 4900 1975 50  0001 C CNN
F 1 "GND" H 4900 2075 50  0000 C CNN
F 2 "" H 4900 2225 60  0000 C CNN
F 3 "" H 4900 2225 60  0000 C CNN
	1    4900 2225
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 58B932B9
P 4900 2025
F 0 "#PWR?" H 4900 1775 50  0001 C CNN
F 1 "GND" H 4900 1875 50  0000 C CNN
F 2 "" H 4900 2025 60  0000 C CNN
F 3 "" H 4900 2025 60  0000 C CNN
	1    4900 2025
	0    -1   -1   0   
$EndComp
Text GLabel 4900 2325 2    60   Output ~ 0
INT
Text GLabel 6275 3375 0    60   Input ~ 0
INT
NoConn ~ 6775 3075
NoConn ~ 6775 3175
NoConn ~ 6775 3275
NoConn ~ 6775 3375
$EndSCHEMATC
