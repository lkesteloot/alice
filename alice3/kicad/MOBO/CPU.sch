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
LIBS:km681000blp-7
LIBS:28f512
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
L Z80CPU U6
U 1 1 568DDED4
P 5300 3600
F 0 "U6" H 4750 5000 50  0000 L CNN
F 1 "Z80CPU" H 5550 5000 50  0000 L CNN
F 2 "" H 5300 4000 50  0000 C CNN
F 3 "" H 5300 4000 50  0000 C CNN
	1    5300 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR52
U 1 1 568DE0B3
P 5300 5200
F 0 "#PWR52" H 5300 4950 50  0001 C CNN
F 1 "GND" H 5300 5050 50  0000 C CNN
F 2 "" H 5300 5200 50  0000 C CNN
F 3 "" H 5300 5200 50  0000 C CNN
	1    5300 5200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR51
U 1 1 568DE0CB
P 5300 2000
F 0 "#PWR51" H 5300 1850 50  0001 C CNN
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
L +5V #PWR50
U 1 1 568DE771
P 4400 2950
F 0 "#PWR50" H 4400 2800 50  0001 C CNN
F 1 "+5V" H 4400 3090 50  0000 C CNN
F 2 "" H 4400 2950 50  0000 C CNN
F 3 "" H 4400 2950 50  0000 C CNN
	1    4400 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 2950 4400 3600
Wire Wire Line
	4400 3000 4600 3000
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
Text GLabel 4300 3100 0    60   Input ~ 0
/INT_5v
Wire Wire Line
	4600 3100 4300 3100
Text GLabel 3000 2400 0    60   Input ~ 0
/RESET_5v
$Comp
L CP C3
U 1 1 568DF3E6
P 3600 2650
F 0 "C3" H 3625 2750 50  0000 L CNN
F 1 "2.2 µF" H 3625 2550 50  0000 L CNN
F 2 "" H 3638 2500 50  0000 C CNN
F 3 "" H 3600 2650 50  0000 C CNN
	1    3600 2650
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 568DF407
P 4200 2150
F 0 "R7" V 4280 2150 50  0000 C CNN
F 1 "1k" V 4200 2150 50  0000 C CNN
F 2 "" V 4130 2150 50  0000 C CNN
F 3 "" H 4200 2150 50  0000 C CNN
	1    4200 2150
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW2
U 1 1 568DF428
P 3200 2800
F 0 "SW2" H 3350 2910 50  0000 C CNN
F 1 "RESET" H 3200 2720 50  0000 C CNN
F 2 "" H 3200 2800 50  0000 C CNN
F 3 "" H 3200 2800 50  0000 C CNN
	1    3200 2800
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR49
U 1 1 568DF44C
P 4200 1950
F 0 "#PWR49" H 4200 1800 50  0001 C CNN
F 1 "+5V" H 4200 2090 50  0000 C CNN
F 2 "" H 4200 1950 50  0000 C CNN
F 3 "" H 4200 1950 50  0000 C CNN
	1    4200 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR48
U 1 1 568DF463
P 3400 3200
F 0 "#PWR48" H 3400 2950 50  0001 C CNN
F 1 "GND" H 3400 3050 50  0000 C CNN
F 2 "" H 3400 3200 50  0000 C CNN
F 3 "" H 3400 3200 50  0000 C CNN
	1    3400 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2500 3200 2400
Connection ~ 3200 2400
Wire Wire Line
	3600 2500 3600 2400
Connection ~ 3600 2400
Wire Wire Line
	4200 2000 4200 1950
Wire Wire Line
	4600 2400 3000 2400
Wire Wire Line
	4200 2300 4200 2400
Connection ~ 4200 2400
Wire Wire Line
	3600 2800 3600 3150
Wire Wire Line
	3600 3150 3200 3150
Wire Wire Line
	3200 3150 3200 3100
Wire Wire Line
	3400 3200 3400 3150
Connection ~ 3400 3150
$Comp
L KM681000BLP-7 U?
U 1 1 568E02F3
P 8200 3350
F 0 "U?" H 8200 4550 50  0000 C CNN
F 1 "KM681000BLP-7" H 8270 2150 50  0000 C CNN
F 2 "" H 8200 3350 50  0000 C CNN
F 3 "" H 8200 3350 50  0000 C CNN
	1    8200 3350
	1    0    0    -1  
$EndComp
$Comp
L 28F512 U?
U 1 1 568E0849
P 2250 5000
F 0 "U?" H 2250 6200 50  0000 C CNN
F 1 "28F512" H 2250 3800 50  0000 C CNN
F 2 "" H 2250 5000 50  0000 C CNN
F 3 "" H 2250 5000 50  0000 C CNN
	1    2250 5000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
