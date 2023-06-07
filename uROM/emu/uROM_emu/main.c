/*
 * uROM_emu.c
 *
 * Created: 2023/06/07 2:52:02
 * Author : USER
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

