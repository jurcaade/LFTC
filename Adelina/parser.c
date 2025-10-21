#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt,...){
	fprintf(stderr,"error in line %d: ",tokens[iTk].line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}

bool consume(int code){
	if(tokens[iTk].code==code){
		consumed=&tokens[iTk++];
		return true;
		}
	return false;
	}
bool funcParams(){}
//defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc(){
    int start = iTk;
    if (consume(FUNCTION))
    if (consume(ID))
    if (consume(LPAR))
       if (funcParams()){}
        if (consume(RPAR))
        if (consume(COLON))
        if (baseType())
             while(defVar()) {
            }
            if (block()) {
                if (consume(END)) {
                    return true;
                }
            }
     iTk = start; 
    return false;
}

//block ::= instr+
bool block(){
    
}
// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType()
{
    int start = iTk;
    if (consume(TYPE_INT))
    return true;
    if(consume(TYPE_REAL))
    return true;
    if(consume(TYPE_STR))
    return true;

    return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar()
{
    int start = iTk;
    if(consume(VAR))
    if (consume(ID))
    if (consume(COLON))
    if (baseType())
    if (consume(SEMICOLON))
   { return true; }
    else
    { tkerr("Lipsește ';' după declararea variabilei"); }

    iTk = start;  
    return false;
}
// program ::= ( defVar | defFunc | block )* FINISH
bool program(){
	for(;;){
		if(defVar()){}
		else if(defFunc()){}
		else if(block()){}
		else break;
		}
	if(consume(FINISH)){
		return true;
		}else tkerr("syntax error");
	return false;
	}

void parse(){
	iTk=0;
	if (defFunc()) {
        printf("Program corect sintactic!\n");
    } else {
        printf("Eroare de sintaxă!\n");
    }
    }
	