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
Sheet 5 5
Title "Power subsystem"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LD29080DT33R U8
U 1 1 56BA71CB
P 7300 1650
F 0 "U8" H 7300 1900 40  0000 C CNN
F 1 "LD29080DT33R" H 7300 1850 40  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" H 7300 1750 40  0001 C CNN
F 3 "" H 7300 1650 60  0000 C CNN
	1    7300 1650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR052
U 1 1 56BA78E3
P 7300 2200
F 0 "#PWR052" H 7300 1950 50  0001 C CNN
F 1 "GND" H 7300 2050 50  0000 C CNN
F 2 "" H 7300 2200 60  0000 C CNN
F 3 "" H 7300 2200 60  0000 C CNN
	1    7300 2200
	1    0    0    -1  
$EndComp
$Comp
L CP1_Small C19
U 1 1 56BA78F3
P 6800 1900
F 0 "C19" H 6810 1970 50  0000 L CNN
F 1 "10µF" H 6810 1820 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 6800 1900 60  0001 C CNN
F 3 "" H 6800 1900 60  0000 C CNN
	1    6800 1900
	1    0    0    -1  
$EndComp
$Comp
L CP1_Small C20
U 1 1 56BA7930
P 7800 1900
F 0 "C20" H 7810 1970 50  0000 L CNN
F 1 "10µF" H 7810 1820 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7800 1900 60  0001 C CNN
F 3 "" H 7800 1900 60  0000 C CNN
	1    7800 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 1600 6800 1600
Wire Wire Line
	6800 1300 6800 1800
Wire Wire Line
	7300 1900 7300 2200
Wire Wire Line
	6800 2000 6800 2100
Wire Wire Line
	6800 2100 7800 2100
Connection ~ 7300 2100
Wire Wire Line
	7700 1600 7800 1600
Wire Wire Line
	7800 1300 7800 1800
Wire Wire Line
	7800 2100 7800 2000
$Comp
L +3V3 #PWR053
U 1 1 56BA7A57
P 7800 1300
F 0 "#PWR053" H 7800 1150 50  0001 C CNN
F 1 "+3V3" H 7800 1440 50  0000 C CNN
F 2 "" H 7800 1300 60  0000 C CNN
F 3 "" H 7800 1300 60  0000 C CNN
	1    7800 1300
	1    0    0    -1  
$EndComp
Connection ~ 7800 1600
$Comp
L +5V #PWR054
U 1 1 56BA7A9A
P 6800 1300
F 0 "#PWR054" H 6800 1150 50  0001 C CNN
F 1 "+5V" H 6800 1440 50  0000 C CNN
F 2 "" H 6800 1300 60  0000 C CNN
F 3 "" H 6800 1300 60  0000 C CNN
	1    6800 1300
	1    0    0    -1  
$EndComp
Connection ~ 6800 1600
$EndSCHEMATC
