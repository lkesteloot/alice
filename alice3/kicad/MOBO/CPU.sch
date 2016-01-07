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
LIBS:24lc256
LIBS:74hc4050
LIBS:lm2937-3-3
LIBS:propeller_plug
LIBS:propeller_p8x32a-d40
LIBS:dfrobot_ftdi_basic
LIBS:levelconverter
LIBS:minim4-d40
LIBS:vga_daughter_board
LIBS:Zilog
LIBS:MOBO-cache
EELAYER 25 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 4 4
Title "CPU section"
Date "2016/01/06"
Rev ""
Comp "Earwig, Inc."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Z80CPU U?
U 1 1 568DDED4
P 5300 3600
F 0 "U?" H 4750 5000 50  0000 L CNN
F 1 "Z80CPU" H 5550 5000 50  0000 L CNN
F 2 "" H 5300 4000 50  0000 C CNN
F 3 "" H 5300 4000 50  0000 C CNN
	1    5300 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 568DE0B3
P 5300 5200
F 0 "#PWR?" H 5300 4950 50  0001 C CNN
F 1 "GND" H 5300 5050 50  0000 C CNN
F 2 "" H 5300 5200 50  0000 C CNN
F 3 "" H 5300 5200 50  0000 C CNN
	1    5300 5200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 568DE0CB
P 5300 2000
F 0 "#PWR?" H 5300 1850 50  0001 C CNN
F 1 "+5V" H 5300 2140 50  0000 C CNN
F 2 "" H 5300 2000 50  0000 C CNN
F 3 "" H 5300 2000 50  0000 C CNN
	1    5300 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 5200 5300 5100
Wire Wire Line
	5300 2100 5300 2000
Wire Wire Line
	6000 4100 6100 4100
Wire Wire Line
	6000 4200 6100 4200
Wire Wire Line
	6000 4300 6100 4300
Wire Wire Line
	6000 4400 6100 4400
Wire Wire Line
	6000 4500 6100 4500
Wire Wire Line
	6000 4600 6100 4600
Wire Wire Line
	6000 4700 6100 4700
Wire Wire Line
	6000 4800 6100 4800
Wire Wire Line
	6000 2400 6100 2400
Wire Wire Line
	6000 2500 6100 2500
Wire Wire Line
	6000 2600 6100 2600
Wire Wire Line
	6000 2700 6100 2700
Wire Wire Line
	6000 2800 6100 2800
Wire Wire Line
	6000 2900 6100 2900
Wire Wire Line
	6000 3000 6100 3000
Wire Wire Line
	6000 3100 6100 3100
Wire Wire Line
	6000 3200 6100 3200
Wire Wire Line
	6000 3300 6100 3300
Wire Wire Line
	6000 3400 6100 3400
Wire Wire Line
	6000 3500 6100 3500
Wire Wire Line
	6000 3600 6100 3600
Wire Wire Line
	6000 3700 6100 3700
Wire Wire Line
	6000 3800 6100 3800
Wire Wire Line
	6000 3900 6100 3900
Text GLabel 6100 3900 2    60   Output ~ 0
A15_5v
Text GLabel 6100 3800 2    60   Output ~ 0
A14_5v
Text GLabel 6100 3700 2    60   Output ~ 0
A13_5v
Text GLabel 6100 3600 2    60   Output ~ 0
A12_5v
Text GLabel 6100 3500 2    60   Output ~ 0
A11_5v
Text GLabel 6100 3400 2    60   Output ~ 0
A10_5v
Text GLabel 6100 3300 2    60   Output ~ 0
A9_5v
Text GLabel 6100 3200 2    60   Output ~ 0
A8_5v
Text GLabel 6100 3100 2    60   Output ~ 0
A7_5v
Text GLabel 6100 3000 2    60   Output ~ 0
A6_5v
Text GLabel 6100 2900 2    60   Output ~ 0
A5_5v
Text GLabel 6100 2800 2    60   Output ~ 0
A4_5v
Text GLabel 6100 2700 2    60   Output ~ 0
A3_5v
Text GLabel 6100 2600 2    60   Output ~ 0
A2_5v
Text GLabel 6100 2500 2    60   Output ~ 0
A1_5v
Text GLabel 6100 2400 2    60   Output ~ 0
A0_5v
Text GLabel 6100 4800 2    60   BiDi ~ 0
D7_5v
Text GLabel 6100 4700 2    60   BiDi ~ 0
D6_5v
Text GLabel 6100 4600 2    60   BiDi ~ 0
D5_5v
Text GLabel 6100 4500 2    60   BiDi ~ 0
D4_5v
Text GLabel 6100 4400 2    60   BiDi ~ 0
D3_5v
Text GLabel 6100 4300 2    60   BiDi ~ 0
D2_5v
Text GLabel 6100 4200 2    60   BiDi ~ 0
D1_5v
Text GLabel 6100 4100 2    60   BiDi ~ 0
D0_5v
$Comp
L +5V #PWR?
U 1 1 568DE771
P 4400 2950
F 0 "#PWR?" H 4400 2800 50  0001 C CNN
F 1 "+5V" H 4400 3090 50  0000 C CNN
F 2 "" H 4400 2950 50  0000 C CNN
F 3 "" H 4400 2950 50  0000 C CNN
	1    4400 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 2950 4400 3000
Wire Wire Line
	4400 3000 4400 3100
Wire Wire Line
	4400 3100 4400 3600
Wire Wire Line
	4400 3100 4600 3100
Wire Wire Line
	4600 3000 4400 3000
Connection ~ 4400 3000
Text GLabel 4500 4100 0    60   Output ~ 0
/RD_5v
Text GLabel 4500 4200 0    60   Output ~ 0
/WR_5v
Text GLabel 4500 4300 0    60   Output ~ 0
/MREQ_5v
Text GLabel 4500 4400 0    60   Output ~ 0
/IORQ_5v
Wire Wire Line
	4500 4100 4600 4100
Wire Wire Line
	4500 4200 4600 4200
Wire Wire Line
	4500 4300 4600 4300
Wire Wire Line
	4500 4400 4600 4400
NoConn ~ 4600 3700
NoConn ~ 4600 3500
Wire Wire Line
	4400 3600 4600 3600
NoConn ~ 4600 3400
Text GLabel 4500 4700 0    60   Input ~ 0
/BUSRQ_5v
Text GLabel 4500 4800 0    60   Output ~ 0
/BUSACK_5v
Wire Wire Line
	4500 4700 4600 4700
Wire Wire Line
	4500 4800 4600 4800
Connection ~ 4400 3100
$EndSCHEMATC
