EESchema Schematic File Version 4
LIBS:CPU298-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
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
L 74xx:74HC74 U1
U 1 1 61108220
P 6300 2200
F 0 "U1" H 6300 2681 50  0000 C CNN
F 1 "74HC74" H 6300 2590 50  0000 C CNN
F 2 "" H 6300 2200 50  0001 C CNN
F 3 "74xx/74hc_hct74.pdf" H 6300 2200 50  0001 C CNN
	1    6300 2200
	1    0    0    -1  
$EndComp
$Sheet
S 1050 1800 1750 1250
U 64746FA5
F0 "Sequencer" 50
F1 "sequencer.sch" 50
F2 "Q" I R 2800 3050 50 
$EndSheet
$Sheet
S 1050 4000 1750 1250
U 6474704B
F0 "Executer" 50
F1 "Executer.sch" 50
$EndSheet
$Comp
L Connector_Generic:Conn_02x20_Counter_Clockwise J1
U 1 1 6474722C
P 10150 2350
F 0 "J1" H 10200 3467 50  0000 C CNN
F 1 "Conn_02x20_Counter_Clockwise" H 10200 3376 50  0000 C CNN
F 2 "" H 10150 2350 50  0001 C CNN
F 3 "~" H 10150 2350 50  0001 C CNN
	1    10150 2350
	1    0    0    -1  
$EndComp
Entry Bus Bus
	10450 2250 10550 2350
Entry Bus Bus
	10450 2350 10550 2450
Entry Bus Bus
	10450 2450 10550 2550
Entry Bus Bus
	10450 2550 10550 2650
Entry Bus Bus
	10450 2650 10550 2750
Entry Bus Bus
	10450 2750 10550 2850
Entry Bus Bus
	10450 2850 10550 2950
Entry Bus Bus
	10450 2950 10550 3050
Wire Bus Line
	10550 3050 10900 3050
Text HLabel 3000 2200 0    50   Input ~ 0
C
Wire Wire Line
	3000 2200 3400 2200
Wire Wire Line
	2750 3050 2750 3350
Wire Wire Line
	2750 3350 2600 3350
Wire Bus Line
	10550 2350 10550 3050
$EndSCHEMATC
