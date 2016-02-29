EESchema Schematic File Version 2
LIBS:sd_card
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
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
LIBS:lm2937-3-3
LIBS:propeller_plug
LIBS:propeller_p8x32a-d40
LIBS:minim4-d40
LIBS:vga_daughter_board
LIBS:Zilog
LIBS:km681000blp-7
LIBS:stm32f415rg
LIBS:regul
LIBS:ldo
LIBS:71v424s15phgi8
LIBS:vga
LIBS:mini_din
LIBS:24lc256
LIBS:z80cpu-smd
LIBS:propeller_p8x32a-q44
LIBS:as6c2008_tsop
LIBS:MOBO_SMD-cache
EELAYER 25 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 4 5
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
L GND #PWR042
U 1 1 568DE0B3
P 3950 4600
F 0 "#PWR042" H 3950 4350 50  0001 C CNN
F 1 "GND" H 3950 4450 50  0000 C CNN
F 2 "" H 3950 4600 50  0000 C CNN
F 3 "" H 3950 4600 50  0000 C CNN
	1    3950 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 1500 4100 800 
Wire Wire Line
	4800 3500 4900 3500
Wire Wire Line
	4800 3600 4900 3600
Wire Wire Line
	4800 3700 4900 3700
Wire Wire Line
	4800 3800 4900 3800
Wire Wire Line
	4800 3900 4900 3900
Wire Wire Line
	4800 4000 4900 4000
Wire Wire Line
	4800 4100 4900 4100
Wire Wire Line
	4800 4200 4900 4200
Wire Wire Line
	4800 1800 4900 1800
Wire Wire Line
	4800 1900 4900 1900
Wire Wire Line
	4800 2000 4900 2000
Wire Wire Line
	4800 2100 4900 2100
Wire Wire Line
	4800 2200 4900 2200
Wire Wire Line
	4800 2300 4900 2300
Wire Wire Line
	4800 2400 4900 2400
Wire Wire Line
	4800 2500 4900 2500
Wire Wire Line
	4800 2600 4900 2600
Wire Wire Line
	4800 2700 4900 2700
Wire Wire Line
	4800 2800 4900 2800
Wire Wire Line
	4800 2900 4900 2900
Wire Wire Line
	4800 3000 4900 3000
Wire Wire Line
	4800 3100 4900 3100
Wire Wire Line
	4800 3200 4900 3200
Wire Wire Line
	4800 3300 4900 3300
Text GLabel 4900 3300 2    60   Output ~ 0
A15
Text GLabel 4900 3200 2    60   Output ~ 0
A14
Text GLabel 4900 3100 2    60   Output ~ 0
A13
Text GLabel 4900 3000 2    60   Output ~ 0
A12
Text GLabel 4900 2900 2    60   Output ~ 0
A11
Text GLabel 4900 2800 2    60   Output ~ 0
A10
Text GLabel 4900 2700 2    60   Output ~ 0
A9
Text GLabel 4900 2600 2    60   Output ~ 0
A8
Text GLabel 4900 2500 2    60   Output ~ 0
A7
Text GLabel 4900 2400 2    60   Output ~ 0
A6
Text GLabel 4900 2300 2    60   Output ~ 0
A5
Text GLabel 4900 2200 2    60   Output ~ 0
A4
Text GLabel 4900 2100 2    60   Output ~ 0
A3
Text GLabel 4900 2000 2    60   Output ~ 0
A2
Text GLabel 4900 1900 2    60   Output ~ 0
A1
Text GLabel 4900 1800 2    60   Output ~ 0
A0
Text GLabel 4900 4200 2    60   BiDi ~ 0
D7
Text GLabel 4900 4100 2    60   BiDi ~ 0
D6
Text GLabel 4900 4000 2    60   BiDi ~ 0
D5
Text GLabel 4900 3900 2    60   BiDi ~ 0
D4
Text GLabel 4900 3800 2    60   BiDi ~ 0
D3
Text GLabel 4900 3700 2    60   BiDi ~ 0
D2
Text GLabel 4900 3600 2    60   BiDi ~ 0
D1
Text GLabel 4900 3500 2    60   BiDi ~ 0
D0
Wire Wire Line
	3200 2350 3200 3000
Wire Wire Line
	3200 2400 3400 2400
Connection ~ 3200 2400
Text GLabel 3300 3500 0    60   Output ~ 0
/RD
Text GLabel 3300 3600 0    60   Output ~ 0
/WR
Text GLabel 3300 3700 0    60   Output ~ 0
/MREQ
Text GLabel 3300 3800 0    60   Output ~ 0
/IORQ
Wire Wire Line
	3300 3500 3400 3500
Wire Wire Line
	3300 3600 3400 3600
Wire Wire Line
	3300 3700 3400 3700
Wire Wire Line
	3300 3800 3400 3800
NoConn ~ 3400 2900
Wire Wire Line
	3200 3000 3400 3000
NoConn ~ 3400 2800
Text GLabel 3300 4100 0    60   Input ~ 0
/BUSRQ
Text GLabel 3300 4200 0    60   Output ~ 0
/BUSACK
Wire Wire Line
	3300 4200 3400 4200
Text GLabel 1800 1800 0    60   Input ~ 0
/Z80_RESET
$Comp
L GND #PWR043
U 1 1 568DF463
P 2200 2600
F 0 "#PWR043" H 2200 2350 50  0001 C CNN
F 1 "GND" H 2200 2450 50  0000 C CNN
F 2 "" H 2200 2600 50  0000 C CNN
F 3 "" H 2200 2600 50  0000 C CNN
	1    2200 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 1800 3400 1800
Wire Wire Line
	2200 2350 2200 2600
Text GLabel 3150 2100 0    60   Input ~ 0
CLK
$Comp
L R R11
U 1 1 56A01524
P 2200 2200
F 0 "R11" V 2280 2200 50  0000 C CNN
F 1 "10k" V 2200 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2130 2200 50  0001 C CNN
F 3 "" H 2200 2200 50  0000 C CNN
	1    2200 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 2050 2200 1800
Connection ~ 2200 1800
Wire Wire Line
	3150 2100 3400 2100
Wire Wire Line
	3400 2500 3200 2500
Connection ~ 3200 2500
Text GLabel 3300 3100 0    60   Output ~ 0
/HALT
Wire Wire Line
	3300 3100 3400 3100
$Comp
L GND #PWR044
U 1 1 56A06CAB
P 4450 1200
F 0 "#PWR044" H 4450 950 50  0001 C CNN
F 1 "GND" H 4450 1050 50  0000 C CNN
F 2 "" H 4450 1200 50  0000 C CNN
F 3 "" H 4450 1200 50  0000 C CNN
	1    4450 1200
	1    0    0    -1  
$EndComp
$Comp
L C C16
U 1 1 56A06CFE
P 4450 1000
F 0 "C16" H 4475 1100 50  0000 L CNN
F 1 "0.1 µF" H 4475 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 4488 850 50  0001 C CNN
F 3 "" H 4450 1000 50  0000 C CNN
	1    4450 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1200 4450 1150
Wire Wire Line
	4100 800  4450 800 
Wire Wire Line
	4450 800  4450 850 
$Comp
L C C17
U 1 1 56A06F35
P 8100 1300
F 0 "C17" H 8125 1400 50  0000 L CNN
F 1 "0.1 µF" H 8125 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 8138 1150 50  0001 C CNN
F 3 "" H 8100 1300 50  0000 C CNN
	1    8100 1300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR045
U 1 1 56A06F73
P 8100 1500
F 0 "#PWR045" H 8100 1250 50  0001 C CNN
F 1 "GND" H 8100 1350 50  0000 C CNN
F 2 "" H 8100 1500 50  0000 C CNN
F 3 "" H 8100 1500 50  0000 C CNN
	1    8100 1500
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR046
U 1 1 56A5493F
P 4100 800
F 0 "#PWR046" H 4100 650 50  0001 C CNN
F 1 "+3V3" H 4100 940 50  0000 C CNN
F 2 "" H 4100 800 60  0000 C CNN
F 3 "" H 4100 800 60  0000 C CNN
	1    4100 800 
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR047
U 1 1 56A55184
P 3200 2350
F 0 "#PWR047" H 3200 2200 50  0001 C CNN
F 1 "+3V3" H 3200 2490 50  0000 C CNN
F 2 "" H 3200 2350 60  0000 C CNN
F 3 "" H 3200 2350 60  0000 C CNN
	1    3200 2350
	1    0    0    -1  
$EndComp
Text GLabel 7050 3375 0    60   Input ~ 0
A15
Text GLabel 7050 3275 0    60   Input ~ 0
A14
Text GLabel 7050 3175 0    60   Input ~ 0
A13
Text GLabel 7050 3075 0    60   Input ~ 0
A12
Text GLabel 7050 2975 0    60   Input ~ 0
A11
Text GLabel 7050 2875 0    60   Input ~ 0
A10
Text GLabel 7050 2775 0    60   Input ~ 0
A9
Text GLabel 7050 2675 0    60   Input ~ 0
A8
Text GLabel 7050 2575 0    60   Input ~ 0
A7
Text GLabel 7050 2475 0    60   Input ~ 0
A6
Text GLabel 7050 2375 0    60   Input ~ 0
A5
Text GLabel 7050 2275 0    60   Input ~ 0
A4
Text GLabel 7050 2175 0    60   Input ~ 0
A3
Text GLabel 7050 2075 0    60   Input ~ 0
A2
Text GLabel 7050 1975 0    60   Input ~ 0
A1
Text GLabel 7050 1875 0    60   Input ~ 0
A0
Text GLabel 7050 3475 0    60   Input ~ 0
A16
Text GLabel 7050 3875 0    60   Input ~ 0
/MREQ
Text GLabel 7050 3975 0    60   Input ~ 0
/RD
Text GLabel 7050 4075 0    60   Input ~ 0
/WR
Text GLabel 8450 3275 2    60   BiDi ~ 0
D7
Text GLabel 8450 3175 2    60   BiDi ~ 0
D6
Text GLabel 8450 3075 2    60   BiDi ~ 0
D5
Text GLabel 8450 2975 2    60   BiDi ~ 0
D4
Text GLabel 8450 2875 2    60   BiDi ~ 0
D3
Text GLabel 8450 2775 2    60   BiDi ~ 0
D2
Text GLabel 8450 2675 2    60   BiDi ~ 0
D1
Text GLabel 8450 2575 2    60   BiDi ~ 0
D0
$Comp
L GND #PWR048
U 1 1 56CACCBC
P 7650 4375
F 0 "#PWR048" H 7650 4125 50  0001 C CNN
F 1 "GND" H 7650 4225 50  0000 C CNN
F 2 "" H 7650 4375 50  0000 C CNN
F 3 "" H 7650 4375 50  0000 C CNN
	1    7650 4375
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR049
U 1 1 56CACE05
P 7650 900
F 0 "#PWR049" H 7650 750 50  0001 C CNN
F 1 "+3V3" H 7650 1040 50  0000 C CNN
F 2 "" H 7650 900 60  0000 C CNN
F 3 "" H 7650 900 60  0000 C CNN
	1    7650 900 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR050
U 1 1 56CACF44
P 6400 3875
F 0 "#PWR050" H 6400 3625 50  0001 C CNN
F 1 "GND" H 6400 3725 50  0000 C CNN
F 2 "" H 6400 3875 50  0000 C CNN
F 3 "" H 6400 3875 50  0000 C CNN
	1    6400 3875
	1    0    0    -1  
$EndComp
$Comp
L Z80CPU-SMD U1
U 1 1 56D385CE
P 4100 3000
F 0 "U1" H 3550 4400 50  0000 L CNN
F 1 "Z80CPU-SMD" H 4200 4400 50  0000 L CNN
F 2 "" H 4100 3400 50  0000 C CNN
F 3 "" H 4100 3400 50  0000 C CNN
	1    4100 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 4600 3950 4500
Wire Wire Line
	7050 3775 6400 3775
Wire Wire Line
	6400 3775 6400 3875
Text GLabel 7050 3575 0    60   Input ~ 0
A17
Wire Wire Line
	3300 4100 3400 4100
$Comp
L AS6C2008_TSOP U6
U 1 1 56D3979F
P 7750 2925
F 0 "U6" H 7425 4125 50  0000 C CNN
F 1 "AS6C2008_TSOP" H 8025 1600 50  0000 C CNN
F 2 "" H 7750 2925 50  0000 C CNN
F 3 "" H 7750 2925 50  0000 C CNN
	1    7750 2925
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 1575 7650 900 
Wire Wire Line
	8100 1150 8100 950 
Wire Wire Line
	8100 950  7650 950 
Connection ~ 7650 950 
Wire Wire Line
	8100 1500 8100 1450
$EndSCHEMATC
