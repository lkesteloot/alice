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
L Z80CPU U15
U 1 1 568DDED4
P 3850 5600
F 0 "U15" H 3300 7000 50  0000 L CNN
F 1 "Z80CPU" H 4100 7000 50  0000 L CNN
F 2 "" H 3850 6000 50  0000 C CNN
F 3 "" H 3850 6000 50  0000 C CNN
	1    3850 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR048
U 1 1 568DE0B3
P 3850 7200
F 0 "#PWR048" H 3850 6950 50  0001 C CNN
F 1 "GND" H 3850 7050 50  0000 C CNN
F 2 "" H 3850 7200 50  0000 C CNN
F 3 "" H 3850 7200 50  0000 C CNN
	1    3850 7200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR049
U 1 1 568DE0CB
P 3850 4000
F 0 "#PWR049" H 3850 3850 50  0001 C CNN
F 1 "+5V" H 3850 4140 50  0000 C CNN
F 2 "" H 3850 4000 50  0000 C CNN
F 3 "" H 3850 4000 50  0000 C CNN
	1    3850 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 7200 3850 7100
Wire Wire Line
	3850 4100 3850 4000
Wire Wire Line
	4550 6100 4650 6100
Wire Wire Line
	4550 6200 4650 6200
Wire Wire Line
	4550 6300 4650 6300
Wire Wire Line
	4550 6400 4650 6400
Wire Wire Line
	4550 6500 4650 6500
Wire Wire Line
	4550 6600 4650 6600
Wire Wire Line
	4550 6700 4650 6700
Wire Wire Line
	4550 6800 4650 6800
Wire Wire Line
	4550 4400 4650 4400
Wire Wire Line
	4550 4500 4650 4500
Wire Wire Line
	4550 4600 4650 4600
Wire Wire Line
	4550 4700 4650 4700
Wire Wire Line
	4550 4800 4650 4800
Wire Wire Line
	4550 4900 4650 4900
Wire Wire Line
	4550 5000 4650 5000
Wire Wire Line
	4550 5100 4650 5100
Wire Wire Line
	4550 5200 4650 5200
Wire Wire Line
	4550 5300 4650 5300
Wire Wire Line
	4550 5400 4650 5400
Wire Wire Line
	4550 5500 4650 5500
Wire Wire Line
	4550 5600 4650 5600
Wire Wire Line
	4550 5700 4650 5700
Wire Wire Line
	4550 5800 4650 5800
Wire Wire Line
	4550 5900 4650 5900
Text GLabel 4650 5900 2    60   Output ~ 0
A15_5v
Text GLabel 4650 5800 2    60   Output ~ 0
A14_5v
Text GLabel 4650 5700 2    60   Output ~ 0
A13_5v
Text GLabel 4650 5600 2    60   Output ~ 0
A12_5v
Text GLabel 4650 5500 2    60   Output ~ 0
A11_5v
Text GLabel 4650 5400 2    60   Output ~ 0
A10_5v
Text GLabel 4650 5300 2    60   Output ~ 0
A9_5v
Text GLabel 4650 5200 2    60   Output ~ 0
A8_5v
Text GLabel 4650 5100 2    60   Output ~ 0
A7_5v
Text GLabel 4650 5000 2    60   Output ~ 0
A6_5v
Text GLabel 4650 4900 2    60   Output ~ 0
A5_5v
Text GLabel 4650 4800 2    60   Output ~ 0
A4_5v
Text GLabel 4650 4700 2    60   Output ~ 0
A3_5v
Text GLabel 4650 4600 2    60   Output ~ 0
A2_5v
Text GLabel 4650 4500 2    60   Output ~ 0
A1_5v
Text GLabel 4650 4400 2    60   Output ~ 0
A0_5v
Text GLabel 4650 6800 2    60   BiDi ~ 0
D7_5v
Text GLabel 4650 6700 2    60   BiDi ~ 0
D6_5v
Text GLabel 4650 6600 2    60   BiDi ~ 0
D5_5v
Text GLabel 4650 6500 2    60   BiDi ~ 0
D4_5v
Text GLabel 4650 6400 2    60   BiDi ~ 0
D3_5v
Text GLabel 4650 6300 2    60   BiDi ~ 0
D2_5v
Text GLabel 4650 6200 2    60   BiDi ~ 0
D1_5v
Text GLabel 4650 6100 2    60   BiDi ~ 0
D0_5v
$Comp
L +5V #PWR050
U 1 1 568DE771
P 2950 4950
F 0 "#PWR050" H 2950 4800 50  0001 C CNN
F 1 "+5V" H 2950 5090 50  0000 C CNN
F 2 "" H 2950 4950 50  0000 C CNN
F 3 "" H 2950 4950 50  0000 C CNN
	1    2950 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 4950 2950 5600
Wire Wire Line
	2950 5000 3150 5000
Connection ~ 2950 5000
Text GLabel 3050 6100 0    60   Output ~ 0
/RD_5v
Text GLabel 3050 6200 0    60   Output ~ 0
/WR_5v
Text GLabel 3050 6300 0    60   Output ~ 0
/MREQ_5v
Text GLabel 3050 6400 0    60   Output ~ 0
/IORQ_5v
Wire Wire Line
	3050 6100 3150 6100
Wire Wire Line
	3050 6200 3150 6200
Wire Wire Line
	3050 6300 3150 6300
Wire Wire Line
	3050 6400 3150 6400
NoConn ~ 3150 5700
NoConn ~ 3150 5500
Wire Wire Line
	2950 5600 3150 5600
NoConn ~ 3150 5400
Text GLabel 3050 6700 0    60   Input ~ 0
/BUSRQ_5v
Text GLabel 3050 6800 0    60   Output ~ 0
/BUSACK_5v
Wire Wire Line
	3050 6700 3150 6700
Wire Wire Line
	3050 6800 3150 6800
Text GLabel 2850 5100 0    60   Input ~ 0
/INT_5v
Wire Wire Line
	3150 5100 2850 5100
Text GLabel 1550 4400 0    60   Input ~ 0
/RESET_5v
$Comp
L CP C4
U 1 1 568DF3E6
P 2150 4650
F 0 "C4" H 2175 4750 50  0000 L CNN
F 1 "2.2 µF" H 2175 4550 50  0000 L CNN
F 2 "" H 2188 4500 50  0000 C CNN
F 3 "" H 2150 4650 50  0000 C CNN
	1    2150 4650
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 568DF407
P 2750 4150
F 0 "R12" V 2830 4150 50  0000 C CNN
F 1 "1k" V 2750 4150 50  0000 C CNN
F 2 "" V 2680 4150 50  0000 C CNN
F 3 "" H 2750 4150 50  0000 C CNN
	1    2750 4150
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW2
U 1 1 568DF428
P 1750 4800
F 0 "SW2" H 1900 4910 50  0000 C CNN
F 1 "RESET" H 1750 4720 50  0000 C CNN
F 2 "" H 1750 4800 50  0000 C CNN
F 3 "" H 1750 4800 50  0000 C CNN
	1    1750 4800
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR051
U 1 1 568DF44C
P 2750 3950
F 0 "#PWR051" H 2750 3800 50  0001 C CNN
F 1 "+5V" H 2750 4090 50  0000 C CNN
F 2 "" H 2750 3950 50  0000 C CNN
F 3 "" H 2750 3950 50  0000 C CNN
	1    2750 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR052
U 1 1 568DF463
P 1950 5200
F 0 "#PWR052" H 1950 4950 50  0001 C CNN
F 1 "GND" H 1950 5050 50  0000 C CNN
F 2 "" H 1950 5200 50  0000 C CNN
F 3 "" H 1950 5200 50  0000 C CNN
	1    1950 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 4500 1750 4400
Connection ~ 1750 4400
Wire Wire Line
	2150 4400 2150 4500
Connection ~ 2150 4400
Wire Wire Line
	2750 4000 2750 3950
Wire Wire Line
	1550 4400 3150 4400
Wire Wire Line
	2750 4300 2750 4400
Connection ~ 2750 4400
Wire Wire Line
	2150 5150 2150 4800
Wire Wire Line
	1750 5150 2150 5150
Wire Wire Line
	1750 5150 1750 5100
Wire Wire Line
	1950 5200 1950 5150
Connection ~ 1950 5150
$Comp
L KM681000BLP-7 U18
U 1 1 568E02F3
P 8400 2100
F 0 "U18" H 8400 3300 50  0000 C CNN
F 1 "KM681000BLP-7" H 8470 900 50  0000 C CNN
F 2 "" H 8400 2100 50  0000 C CNN
F 3 "" H 8400 2100 50  0000 C CNN
	1    8400 2100
	1    0    0    -1  
$EndComp
$Comp
L 28F512 U19
U 1 1 568E0849
P 8400 5150
F 0 "U19" H 8400 6350 50  0000 C CNN
F 1 "28F512" H 8400 3950 50  0000 C CNN
F 2 "" H 8400 5150 50  0000 C CNN
F 3 "" H 8400 5150 50  0000 C CNN
	1    8400 5150
	1    0    0    -1  
$EndComp
$Comp
L 74LS32 U14
U 1 1 568E0BF7
P 1950 1050
F 0 "U14" H 1950 1100 60  0000 C CNN
F 1 "74LS32" H 1950 1000 60  0000 C CNN
F 2 "" H 1950 1050 60  0000 C CNN
F 3 "" H 1950 1050 60  0000 C CNN
	1    1950 1050
	1    0    0    -1  
$EndComp
Text GLabel 1350 1150 0    60   Input ~ 0
A15_5v
Text GLabel 1350 950  0    60   Input ~ 0
/WR_5v
$Comp
L 74LS32 U14
U 2 1 568E1086
P 3450 1150
F 0 "U14" H 3450 1200 60  0000 C CNN
F 1 "74LS32" H 3450 1100 60  0000 C CNN
F 2 "" H 3450 1150 60  0000 C CNN
F 3 "" H 3450 1150 60  0000 C CNN
	2    3450 1150
	1    0    0    -1  
$EndComp
Text GLabel 2200 1700 0    60   Input ~ 0
/MREQ_5v
Wire Wire Line
	2550 1050 2850 1050
$Comp
L 74LS04 U13
U 2 1 568E11D8
P 4500 1150
F 0 "U13" H 4695 1265 60  0000 C CNN
F 1 "74LS04" H 4690 1025 60  0000 C CNN
F 2 "" H 4500 1150 60  0000 C CNN
F 3 "" H 4500 1150 60  0000 C CNN
	2    4500 1150
	1    0    0    -1  
$EndComp
$Comp
L 74LS74 U16
U 1 1 568E1225
P 5550 1150
F 0 "U16" H 5700 1450 60  0000 C CNN
F 1 "74LS74" H 5850 855 60  0000 C CNN
F 2 "" H 5550 1150 60  0000 C CNN
F 3 "" H 5550 1150 60  0000 C CNN
	1    5550 1150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR053
U 1 1 568E12CE
P 4950 950
F 0 "#PWR053" H 4950 700 50  0001 C CNN
F 1 "GND" H 4950 800 50  0000 C CNN
F 2 "" H 4950 950 60  0000 C CNN
F 3 "" H 4950 950 60  0000 C CNN
	1    4950 950 
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR054
U 1 1 568E1300
P 5100 1700
F 0 "#PWR054" H 5100 1550 50  0001 C CNN
F 1 "+5V" H 5100 1840 50  0000 C CNN
F 2 "" H 5100 1700 60  0000 C CNN
F 3 "" H 5100 1700 60  0000 C CNN
	1    5100 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 1700 5100 1700
Wire Wire Line
	5550 600  5000 600 
Text GLabel 5000 600  0    60   Input ~ 0
/RESET_5v
NoConn ~ 6150 950 
$Comp
L 74LS32 U14
U 3 1 568E148A
P 3450 2800
F 0 "U14" H 3450 2850 60  0000 C CNN
F 1 "74LS32" H 3450 2750 60  0000 C CNN
F 2 "" H 3450 2800 60  0000 C CNN
F 3 "" H 3450 2800 60  0000 C CNN
	3    3450 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 1150 1350 2900
Wire Wire Line
	1350 2900 2850 2900
Wire Wire Line
	6150 1350 6300 1350
Wire Wire Line
	6300 1350 6300 1900
Wire Wire Line
	6300 1900 2850 1900
Wire Wire Line
	2850 1900 2850 2700
$Comp
L 74LS04 U13
U 3 1 568E16DB
P 4500 2400
F 0 "U13" H 4695 2515 60  0000 C CNN
F 1 "74LS04" H 4690 2275 60  0000 C CNN
F 2 "" H 4500 2400 60  0000 C CNN
F 3 "" H 4500 2400 60  0000 C CNN
	3    4500 2400
	1    0    0    -1  
$EndComp
$Comp
L 74LS32 U14
U 4 1 568E1784
P 5550 2500
F 0 "U14" H 5550 2550 60  0000 C CNN
F 1 "74LS32" H 5550 2450 60  0000 C CNN
F 2 "" H 5550 2500 60  0000 C CNN
F 3 "" H 5550 2500 60  0000 C CNN
	4    5550 2500
	1    0    0    -1  
$EndComp
$Comp
L 74LS32 U17
U 1 1 568E17E4
P 5550 3200
F 0 "U17" H 5550 3250 60  0000 C CNN
F 1 "74LS32" H 5550 3150 60  0000 C CNN
F 2 "" H 5550 3200 60  0000 C CNN
F 3 "" H 5550 3200 60  0000 C CNN
	1    5550 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2400 4050 3100
Wire Wire Line
	4050 3100 4950 3100
Connection ~ 4050 2800
Wire Wire Line
	2200 1700 2450 1700
Wire Wire Line
	2450 1250 2450 3300
Wire Wire Line
	2450 1250 2850 1250
Wire Wire Line
	2450 3300 4950 3300
Connection ~ 2450 1700
Wire Wire Line
	4950 2600 4600 2600
Wire Wire Line
	4600 2600 4600 3300
Connection ~ 4600 3300
Wire Wire Line
	6150 2500 7100 2500
Wire Wire Line
	7100 2500 7100 2850
Wire Wire Line
	7100 2850 7700 2850
Wire Wire Line
	6150 3200 6450 3200
Wire Wire Line
	6450 3200 6450 6000
Wire Wire Line
	6450 6000 7700 6000
Text GLabel 7700 2550 0    60   Output ~ 0
A15_5v
Text GLabel 7700 2450 0    60   Output ~ 0
A14_5v
Text GLabel 7700 2350 0    60   Output ~ 0
A13_5v
Text GLabel 7700 2250 0    60   Output ~ 0
A12_5v
Text GLabel 7700 2150 0    60   Output ~ 0
A11_5v
Text GLabel 7700 2050 0    60   Output ~ 0
A10_5v
Text GLabel 7700 1950 0    60   Output ~ 0
A9_5v
Text GLabel 7700 1850 0    60   Output ~ 0
A8_5v
Text GLabel 7700 1750 0    60   Output ~ 0
A7_5v
Text GLabel 7700 1650 0    60   Output ~ 0
A6_5v
Text GLabel 7700 1550 0    60   Output ~ 0
A5_5v
Text GLabel 7700 1450 0    60   Output ~ 0
A4_5v
Text GLabel 7700 1350 0    60   Output ~ 0
A3_5v
Text GLabel 7700 1250 0    60   Output ~ 0
A2_5v
Text GLabel 7700 1150 0    60   Output ~ 0
A1_5v
Text GLabel 7700 1050 0    60   Output ~ 0
A0_5v
$Comp
L +5V #PWR055
U 1 1 568E215B
P 6900 2950
F 0 "#PWR055" H 6900 2800 50  0001 C CNN
F 1 "+5V" H 6900 3090 50  0000 C CNN
F 2 "" H 6900 2950 60  0000 C CNN
F 3 "" H 6900 2950 60  0000 C CNN
	1    6900 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 2950 7700 2950
Text GLabel 9100 1750 2    60   BiDi ~ 0
D7_5v
Text GLabel 9100 1650 2    60   BiDi ~ 0
D6_5v
Text GLabel 9100 1550 2    60   BiDi ~ 0
D5_5v
Text GLabel 9100 1450 2    60   BiDi ~ 0
D4_5v
Text GLabel 9100 1350 2    60   BiDi ~ 0
D3_5v
Text GLabel 9100 1250 2    60   BiDi ~ 0
D2_5v
Text GLabel 9100 1150 2    60   BiDi ~ 0
D1_5v
Text GLabel 9100 1050 2    60   BiDi ~ 0
D0_5v
$Comp
L +5V #PWR056
U 1 1 568E25EC
P 9150 2000
F 0 "#PWR056" H 9150 1850 50  0001 C CNN
F 1 "+5V" H 9150 2140 50  0000 C CNN
F 2 "" H 9150 2000 60  0000 C CNN
F 3 "" H 9150 2000 60  0000 C CNN
	1    9150 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 2000 9100 2000
$Comp
L GND #PWR057
U 1 1 568E2687
P 9150 2100
F 0 "#PWR057" H 9150 1850 50  0001 C CNN
F 1 "GND" H 9150 1950 50  0000 C CNN
F 2 "" H 9150 2100 60  0000 C CNN
F 3 "" H 9150 2100 60  0000 C CNN
	1    9150 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 2100 9150 2100
Text GLabel 7700 5600 0    60   Output ~ 0
A15_5v
Text GLabel 7700 5500 0    60   Output ~ 0
A14_5v
Text GLabel 7700 5400 0    60   Output ~ 0
A13_5v
Text GLabel 7700 5300 0    60   Output ~ 0
A12_5v
Text GLabel 7700 5200 0    60   Output ~ 0
A11_5v
Text GLabel 7700 5100 0    60   Output ~ 0
A10_5v
Text GLabel 7700 5000 0    60   Output ~ 0
A9_5v
Text GLabel 7700 4900 0    60   Output ~ 0
A8_5v
Text GLabel 7700 4800 0    60   Output ~ 0
A7_5v
Text GLabel 7700 4700 0    60   Output ~ 0
A6_5v
Text GLabel 7700 4600 0    60   Output ~ 0
A5_5v
Text GLabel 7700 4500 0    60   Output ~ 0
A4_5v
Text GLabel 7700 4400 0    60   Output ~ 0
A3_5v
Text GLabel 7700 4300 0    60   Output ~ 0
A2_5v
Text GLabel 7700 4200 0    60   Output ~ 0
A1_5v
Text GLabel 7700 4100 0    60   Output ~ 0
A0_5v
Text GLabel 9100 4800 2    60   BiDi ~ 0
D7_5v
Text GLabel 9100 4700 2    60   BiDi ~ 0
D6_5v
Text GLabel 9100 4600 2    60   BiDi ~ 0
D5_5v
Text GLabel 9100 4500 2    60   BiDi ~ 0
D4_5v
Text GLabel 9100 4400 2    60   BiDi ~ 0
D3_5v
Text GLabel 9100 4300 2    60   BiDi ~ 0
D2_5v
Text GLabel 9100 4200 2    60   BiDi ~ 0
D1_5v
Text GLabel 9100 4100 2    60   BiDi ~ 0
D0_5v
Text GLabel 7700 6100 0    60   Output ~ 0
/RD_5v
Text GLabel 7700 3050 0    60   Output ~ 0
/RD_5v
$Comp
L +5V #PWR058
U 1 1 568E2984
P 9650 5050
F 0 "#PWR058" H 9650 4900 50  0001 C CNN
F 1 "+5V" H 9650 5190 50  0000 C CNN
F 2 "" H 9650 5050 60  0000 C CNN
F 3 "" H 9650 5050 60  0000 C CNN
	1    9650 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR059
U 1 1 568E29B9
P 9650 5350
F 0 "#PWR059" H 9650 5100 50  0001 C CNN
F 1 "GND" H 9650 5200 50  0000 C CNN
F 2 "" H 9650 5350 60  0000 C CNN
F 3 "" H 9650 5350 60  0000 C CNN
	1    9650 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 5050 9650 5050
Wire Wire Line
	9100 5150 9650 5150
Wire Wire Line
	9650 5150 9650 5350
Wire Wire Line
	9100 5250 9650 5250
Connection ~ 9650 5250
Text GLabel 7700 6200 0    60   Output ~ 0
/WR_5v
Text GLabel 7700 3150 0    60   Output ~ 0
/WR_5v
$Comp
L GND #PWR060
U 1 1 568E2BE1
P 6700 2650
F 0 "#PWR060" H 6700 2400 50  0001 C CNN
F 1 "GND" H 6700 2500 50  0000 C CNN
F 2 "" H 6700 2650 60  0000 C CNN
F 3 "" H 6700 2650 60  0000 C CNN
	1    6700 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 2650 6700 2650
Text GLabel 1000 3950 0    60   Output ~ 0
CLK
$Comp
L 74LS04 U13
U 1 1 568E2D33
P 1550 3950
F 0 "U13" H 1745 4065 60  0000 C CNN
F 1 "74LS04" H 1740 3825 60  0000 C CNN
F 2 "" H 1550 3950 60  0000 C CNN
F 3 "" H 1550 3950 60  0000 C CNN
	1    1550 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 3950 1100 3950
Wire Wire Line
	2000 3950 2550 3950
Wire Wire Line
	2550 3950 2550 4700
Wire Wire Line
	2550 4700 3150 4700
$EndSCHEMATC