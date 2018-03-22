EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
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
LIBS:deltarobot-comps
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
L TM4C123G U?
U 1 1 5AB2C0DF
P 4025 3550
F 0 "U?" H 4025 2200 60  0000 C CNN
F 1 "TM4C123G" H 4025 3525 60  0000 C CNN
F 2 "" H 3375 3250 60  0001 C CNN
F 3 "" H 3375 3250 60  0001 C CNN
	1    4025 3550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5AB30AC5
P 4275 5000
F 0 "#PWR?" H 4275 4750 50  0001 C CNN
F 1 "GND" H 4275 4850 50  0000 C CNN
F 2 "" H 4275 5000 50  0001 C CNN
F 3 "" H 4275 5000 50  0001 C CNN
	1    4275 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4275 5000 4275 4800
$Comp
L GND #PWR?
U 1 1 5AB30ADE
P 5600 3750
F 0 "#PWR?" H 5600 3500 50  0001 C CNN
F 1 "GND" H 5600 3600 50  0000 C CNN
F 2 "" H 5600 3750 50  0001 C CNN
F 3 "" H 5600 3750 50  0001 C CNN
	1    5600 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3750 5600 3700
Wire Wire Line
	5600 3700 5275 3700
$Comp
L GND #PWR?
U 1 1 5AB30AFD
P 4175 2000
F 0 "#PWR?" H 4175 1750 50  0001 C CNN
F 1 "GND" H 4175 1850 50  0000 C CNN
F 2 "" H 4175 2000 50  0001 C CNN
F 3 "" H 4175 2000 50  0001 C CNN
	1    4175 2000
	-1   0    0    1   
$EndComp
Wire Wire Line
	4175 2000 4175 2300
$Comp
L GND #PWR?
U 1 1 5AB30B37
P 2425 3925
F 0 "#PWR?" H 2425 3675 50  0001 C CNN
F 1 "GND" H 2425 3775 50  0000 C CNN
F 2 "" H 2425 3925 50  0001 C CNN
F 3 "" H 2425 3925 50  0001 C CNN
	1    2425 3925
	1    0    0    -1  
$EndComp
Wire Wire Line
	2425 3925 2425 3900
Wire Wire Line
	2425 3900 2775 3900
$EndSCHEMATC
