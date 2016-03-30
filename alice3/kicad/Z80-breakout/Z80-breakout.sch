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
LIBS:z80cpu-smd
LIBS:Z80-breakout-cache
EELAYER 25 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Z80 breakout board"
Date ""
Rev "1.0"
Comp "Earwig, Inc."
Comment1 "Lawrence Kesteloot"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X20 P1
U 1 1 56B3E9F5
P 2900 3750
F 0 "P1" H 2900 4800 50  0000 C CNN
F 1 "CONN_01X20" V 3000 3750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x20" H 2900 3750 50  0001 C CNN
F 3 "" H 2900 3750 50  0000 C CNN
	1    2900 3750
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X20 P2
U 1 1 56B3EA25
P 7550 3800
F 0 "P2" H 7550 4850 50  0000 C CNN
F 1 "CONN_01X20" V 7650 3800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x20" H 7550 3800 50  0001 C CNN
F 3 "" H 7550 3800 50  0000 C CNN
	1    7550 3800
	1    0    0    -1  
$EndComp
$Comp
L Z80CPU-SMD U1
U 1 1 56B3F0A3
P 5150 3850
F 0 "U1" H 4600 5250 50  0000 L CNN
F 1 "Z80CPU-SMD" H 5250 5250 50  0000 L CNN
F 2 "Housings_QFP:LQFP-44_10x10mm_Pitch0.8mm" H 5150 4250 50  0001 C CNN
F 3 "" H 5150 4250 50  0000 C CNN
	1    5150 3850
	1    0    0    -1  
$EndComp
Text Notes 2650 2600 0    60   ~ 0
Pin 1 of header
Text Notes 7150 2650 0    60   ~ 0
Pin 40 of header
Text Label 3100 2800 0    60   ~ 0
A11
Text Label 3100 2900 0    60   ~ 0
A12
Text Label 3100 3000 0    60   ~ 0
A13
Text Label 3100 3100 0    60   ~ 0
A14
Text Label 3100 3200 0    60   ~ 0
A15
Text Label 5850 2650 0    60   ~ 0
A0
Text Label 5850 2750 0    60   ~ 0
A1
Text Label 5850 2850 0    60   ~ 0
A2
Text Label 5850 2950 0    60   ~ 0
A3
Text Label 5850 3050 0    60   ~ 0
A4
Text Label 5850 3150 0    60   ~ 0
A5
Text Label 5850 3250 0    60   ~ 0
A6
Text Label 5850 3350 0    60   ~ 0
A7
Text Label 5850 3450 0    60   ~ 0
A8
Text Label 5850 3550 0    60   ~ 0
A9
Text Label 5850 3650 0    60   ~ 0
A10
Text Label 5850 3750 0    60   ~ 0
A11
Text Label 5850 3850 0    60   ~ 0
A12
Text Label 5850 3950 0    60   ~ 0
A13
Text Label 5850 4050 0    60   ~ 0
A14
Text Label 5850 4150 0    60   ~ 0
A15
Text Label 5850 4350 0    60   ~ 0
D0
Text Label 5850 4450 0    60   ~ 0
D1
Text Label 5850 4550 0    60   ~ 0
D2
Text Label 5850 4650 0    60   ~ 0
D3
Text Label 5850 4750 0    60   ~ 0
D4
Text Label 5850 4850 0    60   ~ 0
D5
Text Label 5850 4950 0    60   ~ 0
D6
Text Label 5850 5050 0    60   ~ 0
D7
Text Label 5150 2350 1    60   ~ 0
VCC
Text Label 5000 5350 3    60   ~ 0
GND
Text Label 4450 2650 2    60   ~ 0
~RESET~
Text Label 4450 2950 2    60   ~ 0
~CLK~
Text Label 4450 3250 2    60   ~ 0
~NMI~
Text Label 4450 3350 2    60   ~ 0
~INT~
Text Label 4450 3650 2    60   ~ 0
~M1~
Text Label 4450 3750 2    60   ~ 0
~RFSH~
Text Label 4450 3850 2    60   ~ 0
~WAIT~
Text Label 4450 3950 2    60   ~ 0
~HALT~
Text Label 4450 4350 2    60   ~ 0
~RD~
Text Label 4450 4450 2    60   ~ 0
~WR~
Text Label 4450 4550 2    60   ~ 0
~MREQ~
Text Label 4450 4650 2    60   ~ 0
~IORQ~
Text Label 4450 4950 2    60   ~ 0
~BUSRQ~
Text Label 4450 5050 2    60   ~ 0
~BUSACK~
Text Label 3100 3300 0    60   ~ 0
~CLK~
Text Label 3100 3400 0    60   ~ 0
D4
Text Label 3100 3500 0    60   ~ 0
D3
Text Label 3100 3600 0    60   ~ 0
D5
Text Label 3100 3700 0    60   ~ 0
D6
Text Label 3100 3800 0    60   ~ 0
VCC
Text Label 3100 3900 0    60   ~ 0
D2
Text Label 3100 4000 0    60   ~ 0
D7
Text Label 3100 4100 0    60   ~ 0
D0
Text Label 3100 4200 0    60   ~ 0
D1
Text Label 3100 4300 0    60   ~ 0
~INT~
Text Label 3100 4400 0    60   ~ 0
~NMI~
Text Label 3100 4500 0    60   ~ 0
~HALT~
Text Label 3100 4600 0    60   ~ 0
~MREQ~
Text Label 3100 4700 0    60   ~ 0
~IORQ~
Text Label 7350 3850 2    60   ~ 0
A0
Text Label 7350 3750 2    60   ~ 0
A1
Text Label 7350 3650 2    60   ~ 0
A2
Text Label 7350 3550 2    60   ~ 0
A3
Text Label 7350 3450 2    60   ~ 0
A4
Text Label 7350 3350 2    60   ~ 0
A5
Text Label 7350 3250 2    60   ~ 0
A6
Text Label 7350 3150 2    60   ~ 0
A7
Text Label 7350 3050 2    60   ~ 0
A8
Text Label 7350 2950 2    60   ~ 0
A9
Text Label 7350 2850 2    60   ~ 0
A10
Text Label 7350 3950 2    60   ~ 0
GND
Text Label 7350 4050 2    60   ~ 0
~RFSH~
Text Label 7350 4150 2    60   ~ 0
~M1~
Text Label 7350 4250 2    60   ~ 0
~RESET~
Text Label 7350 4350 2    60   ~ 0
~BUSRQ~
Text Label 7350 4450 2    60   ~ 0
~WAIT~
Text Label 7350 4550 2    60   ~ 0
~BUSACK~
Text Label 7350 4650 2    60   ~ 0
~WR~
Text Label 7350 4750 2    60   ~ 0
~RD~
$Comp
L PWR_FLAG #FLG01
U 1 1 56B3FB1B
P 5500 2250
F 0 "#FLG01" H 5500 2345 50  0001 C CNN
F 1 "PWR_FLAG" H 5500 2430 50  0000 C CNN
F 2 "" H 5500 2250 50  0000 C CNN
F 3 "" H 5500 2250 50  0000 C CNN
	1    5500 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2350 5150 2300
Wire Wire Line
	5150 2300 5500 2300
Wire Wire Line
	5500 2300 5500 2250
$Comp
L PWR_FLAG #FLG02
U 1 1 56B3FB50
P 4200 5500
F 0 "#FLG02" H 4200 5595 50  0001 C CNN
F 1 "PWR_FLAG" H 4200 5680 50  0000 C CNN
F 2 "" H 4200 5500 50  0000 C CNN
F 3 "" H 4200 5500 50  0000 C CNN
	1    4200 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 5350 5000 5600
Wire Wire Line
	5000 5600 4200 5600
Wire Wire Line
	4200 5600 4200 5500
$EndSCHEMATC
