/*
* uROM_emu.c
*
* Created: 2023/06/07 2:52:02
* Author : USER
*/

/*


parts
U1 ATMEGA
U2 64K-SRAM
U3 Buffer-1
U4 Buffer-2
U5 D-FF

pin function
-> ram-addr

-> ram-data

-> ram-we

-> dff-oe

U5 -> ram-oe

*/

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif
#include <avr/io.h>
#include <util/delay.h>
#include "../../298uROM_8S.h"


void setup(void){
	int op, idx ;
	int step ;
	int addr ;

	addr=idx=0 ;
	for(op=0 ; op<256 ; op++){

	}
}

void setup_pins(int mode){
	if(mode==0){ // setup
	}
	else{ // release all
	}
}

int main(void)
{
	setup() ;
    /* Replace with your application code */
    while (1)
    {
    }
}

