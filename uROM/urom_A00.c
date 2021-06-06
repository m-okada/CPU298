/*

CPU298-A00 Decoder ROM build tool.
show Makefile

バスアクセスタイミングの変更：１サイクルでR/W
　アドレスバッファ、データバッファをトランスペアレントラッチにする

cl urom_A00.c

*/
#include <stdio.h>

typedef unsigned short WORD ;
typedef unsigned char BYTE ;


//	addr 12bits
#define ROM_SIZE 4096
WORD rom[ROM_SIZE]={0} ;
WORD rom_idx[ROM_SIZE]={0} ;

#define CODE_RESET 0xe0
#define CODE_FETCH 0xe1
#define CODE_EXEC 0xe2	//	実際にはOPECODEなので、これは使わない

//	ALU
#define ALU_OP_NOP (0x00)
#define ALU_OP_AND (0x01)
#define ALU_OP_OR (0x02)
#define ALU_OP_XOR (0x03)
#define ALU_OP_NOT (0x04)
#define ALU_OP_Undef5 (0x05)
#define ALU_OP_Undef6 (0x06)
#define ALU_OP_SignEx (0x07)
#define ALU_OP_A_Thru (0x08)
#define ALU_OP_B_Thru (0x09)
#define ALU_OP_ADD (0x0A)
#define ALU_OP_SUB (0x0B)
#define ALU_OP_ADDC (0x0C)
#define ALU_OP_SUBB (0x0D)
#define ALU_OP_ADC0 (0x0E)
#define ALU_OP_All_1 (0x0F)

#define WR_HL 0x00
#define WR_X 0x01
#define WR_Y 0x02
#define WR_PC 0x03

//	write back control.
#define WB_NONE 0x00
#define WB_ACC 0x01
#define WB_W 0x02
#define WB_H 0x03
#define WB_L 0x04
#define WB_X 0x05
#define WB_Y 0x06
#define WB_PC 0x07

#define ALU_A_ACC 0
#define ALU_A__W 1

#define ALU_B_BUS 0
#define ALU_B_RET 1
#define ALU_B_H 2
#define ALU_B_L 3

#define BR_THRU 0
#define BR_INC 1
#define BR_DEC 0x11

#define MEM_WRITE 0x02
#define MEM_READ 0x02
#define ENDF 0x03

WORD OPECode ;
BYTE STEP ;


WORD make_code(WORD write_read, WORD alu_op, WORD c_inv, WORD wb, WORD wr, WORD alu_a, WORD alu_b){
	WORD retval=0 ;

	retval |= (write_read & 0x03) << 14 ;
	retval |= (alu_op & 0x0f) << 9 ;
	retval |= (c_inv & 0x01) << 8 ;
	retval |= (wb & 0x7) << 5 ;
	retval |= (wr & 0x3) << 3 ;
	retval |= (alu_a & 0x1) << 2 ;
	retval |= (alu_b & 0x3) << 0 ;

	return retval ;
}

WORD NOP ;	//	No operation.
WORD FETCH_BYTE ;	//	オペコードフェッチ
WORD FETCH_READDATA ;	//	1バイトフェッチ
WORD CP_INC ;	//	CP+

WORD X_INC ;


WORD OPECode ;
BYTE STEP ;
void set_opecode(BYTE opcode){
	OPECode=opcode ;
	STEP=0 ;
}

void set_code(WORD code){
	// printf("[%X:%X]%X\n", OPECode, STEP, code) ;
	int i=OPECode<<4 | STEP ;
	if(rom[i]!=0) printf("![%x:%x]", OPECode, STEP) ;
	rom[i]=code ;
	STEP++ ;
}

void make_rom(void){
	WORD code ;
	int i, n ;

	set_opecode(0) ; //NOP
	code = make_code(ENDF, ALU_OP_NOP, 0, WB_NONE, 0, ALU_A_ACC, ALU_B_BUS) ;
	set_code(code) ;

	set_opecode(0xEE) ; // JMPS
//	code=make_code_imm8(0, 8, FB_BUFF1) ;
	code = make_code(ENDF, 0, 0, 0, 0, 0, 0) ;
	set_code(code) ;

}

void setup(void){
	CP_INC = make_code(0, ALU_OP_NOP, 0, WB_PC, WR_PC, 0, BR_INC) ;
}


WORD mem[1024] ;

void write_romfile(void){
	FILE* fp ;

	int i ;
	int tbl_idx=0 ;
	int opstart=0 ;
	int zlen=0 ;
	int mem_idx=0 ;

	int out_count=0 ;
	int romsize=0 ;

	fp=fopen("298uROM.TXT", "w") ;
	fprintf(fp, "v2.0 raw\x0a") ;

	for(i=0 ; i<ROM_SIZE ; i++){
		if(rom[i]==0){
			zlen++ ;
		}
		else{
			if(zlen!=0){	//	Output "nnn*0 "
				fprintf(fp, "%d*0 ", zlen) ;
				out_count++ ;
			}
			zlen=0 ;

			romsize++ ;

			fprintf(fp, "%x ", rom[i]) ;
			out_count++ ;

			if(out_count>=8){
				fprintf(fp, "\x0a") ;
				out_count=0 ;
			}
		}
	}
	fclose(fp) ;

	fp=fopen("298uROM.h", "w") ;
	fprintf(fp, "static unsigned int urom[]={\n") ;

	for(i=0 ; i<ROM_SIZE ; i++){
		if((i & 0xff)==0){
			fprintf(fp, "/* %x */", i>>8) ;
		}
		fprintf(fp, "0x%x,", rom[i]) ;
		if((i%8)==7){
			fprintf(fp, "\n") ;
		}
	}
	fprintf(fp, "};\n") ;
}

int main(void){
	setup() ;
	make_rom() ;
	write_romfile() ;
}
