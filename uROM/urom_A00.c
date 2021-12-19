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
#define ALU_OP_INCA (0x05)
#define ALU_OP_DECA (0x06)
#define ALU_OP_SignEx (0x07)
#define ALU_OP_A (0x08)
#define ALU_OP_B (0x09)
#define ALU_OP_ADD (0x0A)
#define ALU_OP_SUB (0x0B)
#define ALU_OP_ADC (0x0C)
#define ALU_OP_SBB (0x0D)
#define ALU_OP_ADC0 (0x0E)
#define ALU_OP_All_1 (0x0F)

#define WR_NOP 0x00
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
#define ALU_A_W 1

#define ALU_B_NOP 0
// #define ALU_B_ACC 0 // ACCを出すに変更。検証中。W出しに戻した。ADD回路通した遅延より、BThruの方が動作速度で有利なので
#define ALU_B_W 0
#define ALU_B_BUS 1
#define ALU_B_H 2
#define ALU_B_L 3

#define ADDR_THRU 0
#define ADDR_INC 1
#define ADDR_DEC 3

#define MEM_WRITE 0x02
#define MEM_READ 0x01
#define ENDF 0x03
#define END_MARK (ENDF << 14)

/*

	set_code(make_code(0, ALU_OP_, ADDR_, WB_, WR_, ALU_A_, ALU_B_)) ;

*/


#define OP_EX_NONE 0

WORD OPECode ;
BYTE STEP ;


WORD make_code(WORD write_read, WORD alu_op, WORD op_ex, WORD wb, WORD wr, WORD alu_a, WORD alu_b){
	WORD retval=0 ;

	retval |= (write_read & 0x03) << 14 ;
	retval |= (op_ex & 0x03) << 12 ;
	retval |= (alu_op & 0x0f) << 8 ;
	retval |= (wb & 0x7) << 5 ;
	retval |= (wr & 0x3) << 3 ;
	retval |= (alu_a & 0x1) << 2 ;
	retval |= (alu_b & 0x3) << 0 ;

	return retval ;
}

WORD NOP ;	//	No operation.
WORD FETCH_BYTE ;	//	オペコードフェッチ
WORD FETCH_READDATA ;	//	1バイトフェッチ
WORD PC_INC ;	//	PC+
WORD HLPC, PCHL ;
WORD HLX, HLY, XHL, YHL ;

WORD W2L, L2W, W2H, H2W ;

WORD X_INC,Y_INC,X_DEC,Y_DEC ;


WORD OPECode ;
BYTE STEP ;
void set_opecode(BYTE opcode){
	OPECode=opcode ;
	STEP=0 ;
}

void set_code(WORD code){
	// printf("[%X:%X]%X\n", OPECode, STEP, code) ;
	int i=OPECode<<4 | (STEP & 0x0f) ;
	if(rom[i]!=0) printf("![%x:%x]", OPECode, STEP) ;
	rom[i]=code ;
	STEP++ ;
}

void make_rom(void){
	WORD code ;
	int i, n ;

	set_opecode(0) ; // NOP
	set_code(make_code(ENDF, ALU_OP_NOP, 0, WB_NONE, 0, ALU_A_ACC, ALU_B_BUS)) ;


	set_opecode(0x02) ; // ST [X],A
	set_code(make_code(MEM_WRITE, ALU_OP_A, ADDR_THRU, WB_NONE, WR_X, ALU_A_ACC, ALU_B_NOP)) ;
	set_code(make_code(ENDF, ALU_OP_NOP, 0, WB_NONE, WR_X, ALU_A_ACC, ALU_B_NOP)) ;


	set_opecode(0x08) ; // MOV A,imm8
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_ACC, WR_PC, 0, ALU_B_BUS)) ;
	set_code(PC_INC | END_MARK) ;


	set_opecode(0x0E) ; // LD A,[X+imm8]
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_W, WR_PC, 0, ALU_B_BUS)) ; // imm8->W
	set_code(make_code(0, ALU_OP_ADD, 0, WB_L, WR_X, ALU_A_W, ALU_B_L)) ; // W+XL->L
	set_code(make_code(0, ALU_OP_B, 0, WB_W, WR_X, ALU_A_ACC, ALU_B_H)) ; // XH->W
	set_code(make_code(0, ALU_OP_ADC0, 0, WB_H, WR_X, ALU_A_W, ALU_B_H)) ; // XH+0+CY->H
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_ACC, WR_HL, ALU_A_ACC, ALU_B_BUS)) ; // [HL]->A
	set_code(PC_INC | END_MARK) ;


	set_opecode(0x1E) ; // LD A,{X+imm8}
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_W, WR_PC, 0, ALU_B_BUS)) ; // imm8->W
	set_code(make_code(0, ALU_OP_ADD, 0, WB_L, WR_X, ALU_A_W, ALU_B_L)) ; // W+XL->L
	set_code(make_code(0, ALU_OP_B, 0, WB_W, WR_X, ALU_A_ACC, ALU_B_H)) ; // XH->W
	set_code(make_code(0, ALU_OP_ADC0, 0, WB_H, WR_X, ALU_A_W, ALU_B_H)) ; // W(XH)+0+CY->H

	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_W, WR_HL, ALU_A_ACC, ALU_B_BUS)) ; // [HL]->W
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_INC, WB_H, WR_HL, ALU_A_ACC, ALU_B_BUS)) ; // [HL+1]->H
	set_code(make_code(0, ALU_OP_A, 0, WB_L, WR_NOP, ALU_A_W, ALU_B_BUS)) ; // W->L
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_ACC, WR_HL, ALU_A_ACC, ALU_B_BUS)) ;// [HL]->A
	set_code(PC_INC | END_MARK) ;


	set_opecode(0x28) ; // MOV X,imm16
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_L, WR_PC, 0, ALU_B_BUS)) ;
	set_code(PC_INC) ;
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_H, WR_PC, 0, ALU_B_BUS)) ;
	set_code(PC_INC) ;
	set_code(make_code(ENDF, ALU_OP_NOP, 0, WB_X, WR_HL, 0, 0)) ;	// HL->X


	set_opecode(0x50) ; // MOV A,XL
	set_code(make_code(ENDF, ALU_OP_B, 0, WB_ACC, WR_X, 0, ALU_B_L)) ;


	set_opecode(0x61) ; // MOV Y,imm16
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_L, WR_PC, 0, ALU_B_BUS)) ;
	set_code(PC_INC) ;
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_H, WR_PC, 0, ALU_B_BUS)) ;
	set_code(PC_INC) ;
	set_code(make_code(ENDF, ALU_OP_NOP, 0, WB_Y, WR_HL, 0, 0)) ;	// HL->Y


	set_opecode(0x68) ; // MOV X,0
	set_code(make_code(0, ALU_OP_NOP, 0, WB_L, 0, 0, 0)) ; // インド人を右へ
	set_code(make_code(0, ALU_OP_NOP, 0, WB_H, 0, 0, 0)) ; // 0をHへ
	set_code(make_code(ENDF, ALU_OP_NOP, ADDR_THRU, WB_X, WR_HL, 0, 0)) ; // HL->X

	set_opecode(0x69) ; // MOV Y,0
	set_code(make_code(0, ALU_OP_NOP, 0, WB_L, 0, 0, 0)) ; // 0をLへ
	set_code(make_code(0, ALU_OP_NOP, 0, WB_H, 0, 0, 0)) ; // 0をHへ
	set_code(make_code(ENDF, ALU_OP_NOP, ADDR_THRU, WB_Y, WR_HL, 0, 0)) ; // HL->Y


	set_opecode(0x73) ; // ADD X,byte [imm16](WORD PTRいけるんじゃね？)

	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_L, WR_PC, ALU_A_ACC, ALU_B_BUS)) ; // imm8->L
	set_code(PC_INC) ;
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_H, WR_PC, ALU_A_ACC, ALU_B_BUS)) ; // imm8->H
	set_code(PC_INC) ;

	// [HL]->W [HL+]->H W->L XL->W W+L->L XH->W W+H+CY->H HL->X

	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_W, WR_HL, 0, ALU_B_BUS)) ; // [HL]->W
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_INC, WB_H, WR_HL, 0, ALU_B_BUS)) ; // [HL+]->H

	set_code(W2L) ;

//	set_code(make_code(0, ALU_OP_B, ADDR_THRU, WB_L, WR_NOP, 0, ALU_B_W)) ; // W->L　ここでHLが壊れる,Lを保存するならWを壊していい
//	set_code(make_code(0, ALU_OP_ADD, ADDR_THRU, WB_L, WR_X, 0, 0)) ; // L->W (L+XL->L step1) 無駄を省いた
	set_code(make_code(0, ALU_OP_ADD, 0, WB_L, WR_X, ALU_A_W, ALU_B_L)) ; // XL+W->L (L+XL->L step2)
	set_code(make_code(0, ALU_OP_B, 0, WB_W, 0, 0, ALU_B_H)) ; //H->W// (H+CY+XH->H step1)
	set_code(make_code(0, ALU_OP_ADC, 0, WB_H, WR_X, ALU_A_W, ALU_B_H)) ; // W+CY+XH->H (H+CY+XH->H step2)
	set_code(make_code(ENDF, ALU_OP_NOP, ADDR_THRU, WB_X, WR_HL, 0, 0)) ; // HL->X

	set_opecode(0x80) ; // ADD A,A
	set_code(make_code(0, ALU_OP_A, 0, WB_W, WR_NOP, ALU_A_ACC, ALU_B_W)) ;	// A->W
	set_code(make_code(ENDF, ALU_OP_ADD, 0, WB_ACC, WR_NOP, ALU_A_ACC, ALU_B_W)) ;	// A+=W

	set_opecode(0xD0) ; // INC A
	set_code(make_code(ENDF, ALU_OP_INCA, 0, WB_ACC, WR_NOP, ALU_A_ACC, ALU_B_NOP)) ;	// A++


	set_opecode(0xEE) ; // JMPS
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_W, WR_PC, 0, ALU_B_BUS)) ; // imm8->W
	set_code(PC_INC) ;
	set_code(make_code(0, ALU_OP_ADD, 0, WB_L, WR_PC, ALU_A_W, ALU_B_L)) ; // PCL+W->L
	set_code(make_code(0, ALU_OP_SignEx, 0, WB_W, 0, ALU_A_W, 0)) ;	// W.SignEx->W
	set_code(make_code(0, ALU_OP_ADC, 0, WB_H, WR_PC, ALU_A_W, ALU_B_H)) ;	// CY+W+PCH->H
	set_code(make_code(ENDF, ALU_OP_NOP, 0, WB_PC, WR_HL, 0, 0)) ;	// HL->PC


	set_opecode(0xEF) ; // JMPN @（検証まだ）
	set_code(PC_INC | END_MARK) ;


	set_opecode(0xF0) ; // break
	set_code(END_MARK) ;


	set_opecode(0xF8) ; // CALL imm16
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_L, WR_PC, 0, ALU_B_BUS)) ; // imm8->L
	set_code(PC_INC) ;
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_H, WR_PC, 0, ALU_B_BUS)) ; // imm8->H
	set_code(PC_INC) ;

	set_code(make_code(0, ALU_OP_NOP, ADDR_DEC, WB_Y, WR_Y, 0, 0)) ; // Y-
	set_code(make_code(0, ALU_OP_B, ADDR_THRU, WB_W, WR_PC, 0, ALU_B_H)) ; // PCH->W
	set_code(make_code(MEM_WRITE, ALU_OP_A, ADDR_THRU, 0, WR_Y, ALU_A_W, 0)) ; // [Y]<-W
	set_code(make_code(0, ALU_OP_NOP, ADDR_DEC, WB_Y, WR_Y, 0, 0)) ; // Y-
	set_code(make_code(0, ALU_OP_B, ADDR_THRU, WB_W, WR_PC, 0, ALU_B_L)) ; // PCL->W
	set_code(make_code(MEM_WRITE, ALU_OP_A, ADDR_THRU, 0, WR_Y, ALU_A_W, 0)) ; // [Y]<-W
	set_code(HLPC | END_MARK) ; // HL->PC


	set_opecode(0xFB) ; // RET
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_L, WR_Y, 0, ALU_B_BUS)) ; // [Y]->L
	set_code(make_code(0, ALU_OP_NOP, ADDR_INC, WB_Y, WR_Y, 0, 0)) ; // Y+
	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_H, WR_Y, 0, ALU_B_BUS)) ; // [Y]->H
	set_code(make_code(0, ALU_OP_NOP, ADDR_INC, WB_Y, WR_Y, 0, 0)) ; // Y+
	set_code(HLPC | END_MARK) ;


	set_opecode(0xFE) ; // JMP imm16
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_L, WR_PC, 0, ALU_B_BUS)) ; // imm8->L
	set_code(PC_INC) ;
	set_code(make_code(MEM_READ, ALU_OP_B, 0, WB_H, WR_PC, 0, ALU_B_BUS)) ; // imm8->H
	set_code(PC_INC) ;
	set_code(HLPC | END_MARK) ;


	set_opecode(CODE_RESET) ; // Reset アドレス0:1にあるアドレスにJMP
	// リセット直後は前回の命令の状態が残ってるので、実際の動作は 0xE01 から始まるようにする。
	set_code(make_code(0, ALU_OP_NOP, 0, 0, 0, 0, 0)) ; // 落ち着くまでNOP
	set_code(make_code(0, ALU_OP_NOP, 0, WB_L, 0, 0, 0)) ; // 0をLへ
	set_code(make_code(0, ALU_OP_NOP, 0, WB_H, 0, 0, 0)) ; // 0をHへ
//	set_code(HLX) ; // HL->X
	// X--
//	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_THRU, WB_L, WR_X, 0, ALU_B_BUS)) ; // [X]->L
//	set_code(make_code(MEM_READ, ALU_OP_B, ADDR_INC, WB_H, WR_X, 0, ALU_B_BUS)) ; // [X+1]->H
	set_code(HLPC | END_MARK) ; // HL->PC


	set_opecode(CODE_FETCH) ;
	// バスへ出すアドレスをセレクタで切り替えるなら、PC->アドレスバス、と同時にPC+をPCにライトバックというテもある
	// ADDRのDEC+CYを使うのがいいかな。切り替え＋INC出力で。その場合2to1セレクタx16で、最低でもIC4つ増えるので。
	set_code(make_code(MEM_READ, ALU_OP_NOP, ADDR_THRU, WB_NONE, WR_PC, 0, 0)) ; // PC->OUT ここでオペコードふぇっちしとかないと、次で消えちゃう
	set_code(PC_INC | END_MARK) ; // PC+ -> PC
}

void setup(void){
	PC_INC = make_code(0, ALU_OP_NOP, ADDR_INC, WB_PC, WR_PC, 0, ALU_B_BUS) ;
	FETCH_BYTE = make_code(MEM_READ, ALU_OP_B, 0, 0, WR_PC, 0, ALU_B_BUS) ;
	HLPC = make_code(0, ALU_OP_NOP, ADDR_THRU, WB_PC, WR_HL, 0, 0) ; // HLをPCへ
	HLX = make_code(0, ALU_OP_NOP, ADDR_THRU, WB_X, WR_HL, 0, 0) ; // HL->X
	HLY = make_code(0, ALU_OP_NOP, ADDR_THRU, WB_Y, WR_HL, 0, 0) ; // HL->Y

	W2L = make_code(0, ALU_OP_A, ADDR_THRU, WB_L, WR_HL, ALU_A_W, 0) ; // W->L
	L2W = make_code(0, ALU_OP_B, ADDR_THRU, WB_W, WR_HL, 0, ALU_B_L) ; // L->W
	W2H = make_code(0, ALU_OP_A, ADDR_THRU, WB_H, WR_HL, ALU_A_W, 0) ; // W->H
	H2W = make_code(0, ALU_OP_B, ADDR_THRU, WB_W, WR_HL, 0, ALU_B_H) ; // H->W
}



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


	fp=fopen("298uROM_L.HEX", "wb") ;

	WORD chksum=0, addr=0 ;

	for(i=0 ; i<ROM_SIZE ; i++){
		if((i%16)==0){
			fprintf(fp, ":10") ;
			fprintf(fp, "%04X", addr) ;
			fprintf(fp, "00") ;
			chksum = 0x10+(addr & 0xff)+((addr>>8)&0xff) ;
			addr+=16 ;
		}

		fprintf(fp, "%02X", (rom[i] & 0xff)) ;
		chksum += (rom[i] & 0xff) ;
		if((i%16)==15){
			fprintf(fp, "%02X\x0D\x0A", ((-chksum) & 0xff)) ;
			chksum=0 ;
		}
	}
	fprintf(fp, ":00000001FF\x0D\x0A") ;
	fclose(fp) ;


	fp=fopen("298uROM_H.HEX", "wb") ;

	chksum=0, addr=0 ;

	for(i=0 ; i<ROM_SIZE ; i++){
		if((i%16)==0){
			fprintf(fp, ":10") ;
			fprintf(fp, "%04X", addr) ;
			fprintf(fp, "00") ;
			chksum = 0x10+(addr & 0xff)+((addr>>8)&0xff) ;
			addr+=16 ;
		}

		fprintf(fp, "%02X", ((rom[i]>>8) & 0xff)) ;
		chksum += ((rom[i]>>8) & 0xff) ;
		if((i%16)==15){
			fprintf(fp, "%02X\x0D\x0A", ((-chksum) & 0xff)) ;
			chksum=0 ;
		}
	}
	fprintf(fp, ":00000001FF\x0D\x0A") ;
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
