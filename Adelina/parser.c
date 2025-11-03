#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lexer.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

bool block();
_Noreturn void tkerr(const char *fmt,...);
bool defVar();
bool defFunc();
bool consume(int code);
bool baseType();
bool funcParams();
bool funcParam();
bool instr();
bool expr();
bool exprLogic();
bool exprAssign();
bool exprComp();
bool exprAdd();
bool exprMul();
bool exprPrefix();
bool factor();

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType(){
    if(tokens[iTk].code == TYPE_INT || tokens[iTk].code == TYPE_REAL || tokens[iTk].code == TYPE_STR){
        consume(tokens[iTk].code);
        return true;
    }
    return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON    ex: 
bool defVar(){
    if(tokens[iTk].code != VAR) return false;
    consume(VAR);
    if(!consume(ID)) tkerr("lipseste numele variabilei dupa 'var'");
    if(!consume(COLON)) tkerr("lipseste ':' dupa numele variabilei");
    if(!baseType()) tkerr("lipseste tipul dupa ':' in declaratia de variabila");
    if(!consume(SEMICOLON)) tkerr("lipseste ';' la finalul declaratiei de variabila");
    return true;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc(){
    if(tokens[iTk].code != FUNCTION) return false;
    consume(FUNCTION);
    if(!consume(ID)) tkerr("lipsește numele funcției după 'function'");
    if(!consume(LPAR)) tkerr("lipsește '(' după numele funcției");
    if(tokens[iTk].code != RPAR){
        if(!funcParams()) tkerr("parametri invalizi pentru funcție");
    }
    if(!consume(RPAR)) tkerr("lipsește ')' după lista de parametri a funcției");
    if(!consume(COLON)) tkerr("lipsește ':' după antetul funcției");
    if(!baseType()) tkerr("lipsește tipul de return după ':' în antetul funcției");
    while(defVar()){}
    if(!block()) tkerr("lipsește corpul funcției (block)");
    if(!consume(END)) tkerr("lipsește 'end' după corpul funcției");
    return true;
}

// block ::= instr+
bool block(){
    int cnt = 0;
    for(;;){
        int c = tokens[iTk].code;
        if(c==END || c==ELSE || c==FINISH) break;
        if(!instr()) break;
        cnt++;
    }
    return cnt>0;
}

// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams(){
    if(!funcParam()) return false;
    while(consume(COMMA)){
        if(!funcParam()) tkerr("lipsește un parametru după ','");
    }
    return true;
}

// funcParam ::= ID COLON baseType
bool funcParam(){
    if(!consume(ID)) return false;
    if(!consume(COLON)) tkerr("lipsește ':' după numele parametrului");
    if(!baseType()) tkerr("lipsește tipul după ':' în declarația parametrului");
    return true;
}

// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END | RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr(){
    int c = tokens[iTk].code;

    if(c==IF){
        consume(IF);
        if(!consume(LPAR)) tkerr("lipsește '(' după 'if'");
        if(!expr()) tkerr("condiție invalidă pentru 'if' sau lipsește expresia");
        if(!consume(RPAR)) tkerr("lipsește ')' după condiția 'if'");
        if(!block()) tkerr("lipsește blocul de instrucțiuni după 'if'");
        if(consume(ELSE)){
            if(!block()) tkerr("lipsește blocul de instrucțiuni după 'else'");
        }
        if(!consume(END)) tkerr("lipsește 'end' după 'if'");
        return true;
    }

    if(c==RETURN){
        consume(RETURN);
        if(!expr()) tkerr("lipsește expresia după 'return'");
        if(!consume(SEMICOLON)) tkerr("lipsește ';' după expresia return");
        return true;
    }

    if(c==WHILE){
        consume(WHILE);
        if(!consume(LPAR)) tkerr("lipsește '(' după 'while'");
        if(!expr()) tkerr("condiție invalidă pentru 'while' sau lipsește expresia");
        if(!consume(RPAR)) tkerr("lipsește ')' după condiția 'while'");
        if(!block()) tkerr("lipsește blocul de instrucțiuni după 'while'");
        if(!consume(END)) tkerr("lipsește 'end' după 'while'");
        return true;
    }

    /*  expr? SEMICOLON */
    if(tokens[iTk].code==SEMICOLON){
        consume(SEMICOLON);
        return true;
    }

    if(expr()){
        if(!consume(SEMICOLON)) tkerr("lipsește ';' după expresie");
        return true;
    }
    return false;
}

// expr ::= exprLogic
bool expr(){
    return exprLogic();
}

// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic(){
    if(!exprAssign()) return false;
    while(tokens[iTk].code==AND || tokens[iTk].code==OR){
        consume(tokens[iTk].code);
        if(!exprAssign()) tkerr("lipsește expresia după operatorul logic");
    }
    return true;
}

// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign(){
    if(tokens[iTk].code==ID && tokens[iTk+1].code==ASSIGN){
        consume(ID);
        consume(ASSIGN);
        if(!exprComp()) tkerr("lipsește expresia după '='");
        return true;
    }
    return exprComp();
}

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp(){
    if(!exprAdd()) return false;
     if(tokens[iTk].code==LESS || tokens[iTk].code==EQUAL || tokens[iTk].code==GREATER || tokens[iTk].code==LESSEQ || tokens[iTk].code==GREATEREQ || tokens[iTk].code==NOTEQ){
        consume(tokens[iTk].code);
        if(!exprAdd()) tkerr("lipsește expresia după operatorul de comparație");
    }
    return true;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd(){
    if(!exprMul()) return false;
    while(tokens[iTk].code==ADD || tokens[iTk].code==SUB){
        consume(tokens[iTk].code);
        if(!exprMul()) tkerr("lipsește termenul după '+' sau '-'");
    }
    return true;
}


// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul(){
    if(!exprPrefix()) return false;
    while(tokens[iTk].code==MUL || tokens[iTk].code==DIV){
        consume(tokens[iTk].code);
        if(!exprPrefix()) tkerr("lipsește termenul după '*' sau '/'");
    }
    return true;
}

// exprPrefix ::= ( SUB | NOT )? factor
bool exprPrefix(){
    if(tokens[iTk].code==SUB || tokens[iTk].code==NOT){
        consume(tokens[iTk].code);
    }
    return factor();
}

// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
bool factor(){
    int c = tokens[iTk].code;
    if(c==INT){ consume(INT); return true; }
    if(c==REAL){ consume(REAL); return true; }
    if(c==STR){ consume(STR); return true; }
     if(c==LPAR){
        consume(LPAR);
        if(!expr()) tkerr("lipsește expresia după '('");
        if(!consume(RPAR)) tkerr("lipsește ')' după expresie");
        return true;
    }

    if(c==ID){
        consume(ID);
        if(tokens[iTk].code==LPAR){
            consume(LPAR);
            if(tokens[iTk].code!=RPAR){
                if(!expr()) tkerr("lipsește expresia în apelul funcției");
                while(consume(COMMA)){
                    if(!expr()) tkerr("lipsește expresia după ',' în apelul funcției");
                }
            }
            if(!consume(RPAR)) tkerr("lipsește ')' după argumentele apelului funcției");
        }
        return true;
    }
    return false;
}


// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt,...){
	fprintf(stderr,"[ERROR] in line %d: ",tokens[iTk].line -1 );
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
	 if (program()) {
    printf("Program corect sintactic!\n");
    } else {
        printf("Eroare de sintaxă!\n");
    }
}