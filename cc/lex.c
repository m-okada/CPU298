
/*

字句解析

cl lex.c get_line.c -DDDEBUG_ALONE
*/

typedef unsigned short UINT ;


#ifdef DEBUG_ALONE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#endif

#define FF 0xffff

typedef unsigned char BYTE ;
typedef unsigned shirt WORD ;

int is_white_space(char cc){
	if(cc==0x0d || cc==0x0a || cc==tab || cc==spc) return 1 ;
	return 0 ;
}


int isNum(char cc){
	if(cc>='0' && cc <='9') return 1 ;
	return 0 ;
}

int isAlpha(char cc){
	if((cc>='A' && cc<='Z') ||(cc>='a' && cc<='z')) return 1 ;
	return 0 ;
}

int isHex(char cc){
	if((cc>='0' && cc <='9') || (cc>='A' && cc<='F') || (cc>='a' && cc<='f')) return 1 ;
	return 0 ;
}

int to_upper(int c){
	return (c>='a' && c<= 'z') ? c-32 : c ;
}

/*

*/
char* getToken(char *ptr){
}

#ifdef DEBUG_ALONE

int main(int argc, char** argv){
	WORD type ;
/*

*/
	printf("\n") ;
	return 1 ;
}

#endif
