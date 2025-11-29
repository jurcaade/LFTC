#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lexer.h"
#include "ad.h"

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
    if(tokens[iTk].code == TYPE_INT){
        consume(TYPE_INT);
        ret.type = TYPE_INT;
        return true;
    }
    if(tokens[iTk].code == TYPE_REAL){
        consume(TYPE_REAL);
        ret.type = TYPE_REAL;
        return true;
    }
    if(tokens[iTk].code == TYPE_STR){
        consume(TYPE_STR);
        ret.type = TYPE_STR;
        return true;
    }
    return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar(){
    if(tokens[iTk].code != VAR) return false;
    consume(VAR);
    if(!consume(ID)) tkerr("lipseste numele variabilei dupa 'var'");

     /* semantic action: add symbol for the variable */
    const char *name=consumed->text;
    Symbol *s=searchInCurrentDomain(name);
    if(s)tkerr("symbol redefinition: %s",name);
    s=addSymbol(name,KIND_VAR);
    s->local=crtFn!=NULL;

    if(!consume(COLON)) tkerr("lipseste ':' dupa numele variabilei");
    if(!baseType()) tkerr("lipseste tipul dupa ':' in declaratia de variabila");

     /* semantic action: set the type of the newly added symbol from ret */
      s->type = ret.type;

    if(!consume(SEMICOLON)) tkerr("lipseste ';' la finalul declaratiei de variabila");
    return true;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc(){
    if(tokens[iTk].code != FUNCTION) return false;
    consume(FUNCTION);
    if(!consume(ID)) tkerr("lipsește numele funcției după 'function'");

    /* semantic action: create function symbol and new domain */
    const char *name = consumed->text;
    Symbol *s = searchInCurrentDomain(name);
    if(s) tkerr("symbol redefinition: %s", name);
    crtFn = addSymbol(name, KIND_FN);
    crtFn->args = NULL;
    addDomain(); /* new domain for function body and parameters */

    if(!consume(LPAR)) tkerr("lipsește '(' după numele funcției");
    if(tokens[iTk].code != RPAR){
        if(!funcParams()) tkerr("parametri invalizi pentru funcție");
    }
    if(!consume(RPAR)) tkerr("lipsește ')' după lista de parametri a funcției");
    if(!consume(COLON)) tkerr("lipsește ':' după antetul funcției");
    if(!baseType()) tkerr("lipsește tipul de return după ':' în antetul funcției");

    /* semantic action: set function return type */
    crtFn->type = ret.type;

    while(defVar()){}
    if(!block()) tkerr("lipsește corpul funcției (block)");
    if(!consume(END)) tkerr("lipsește 'end' după corpul funcției");

    /* leave function domain */
    delDomain();
    crtFn=NULL;

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
        if(!funcParam()) tkerr("lipsește/invalid parametru după ','");
    }
    return true;
}

// funcParam ::= ID COLON baseType
bool funcParam(){
    if(!consume(ID)) return false;

    const char *argName = consumed->text;
     /* check redefinition in current domain */
    Symbol *s = searchInCurrentDomain(argName);
    if(s) tkerr("symbol redefinition: %s", argName);

    if(!consume(COLON)) tkerr("lipsește ':' după numele parametrului");
    if(!baseType()) tkerr("lipsește tipul după ':' în declarația parametrului");

     /* semantic action: add parameter to current domain and to function's arg list */
    s = addSymbol(argName, KIND_ARG);
    s->local = crtFn != NULL;
    s->type = ret.type;
    /* also register in the function's args list and set its type */
    Symbol *sFnParam = addFnArg(crtFn, argName);
    if(sFnParam) sFnParam->type = ret.type;

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
        if(!exprAssign()) 
        {
            if (tokens[iTk-1].code == AND)
            tkerr("lipsește expresia după operatorul logic '&&'");
            else if (tokens[iTk-1].code == OR)
            tkerr("lipsește expresia după operatorul logic '||'");
        }
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
     if(tokens[iTk].code==LESS || tokens[iTk].code==EQUAL){
        consume(tokens[iTk].code);
        if(!exprAdd())
        {
            if (tokens[iTk-1].code == LESS)
            tkerr("lipsește expresia după operatorul de comparație '<'");
            else if (tokens[iTk-1].code == EQUAL)
            tkerr("lipsește expresia după operatorul de comparație '=='");
        } 
    }
    return true;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd(){
    if(!exprMul()) return false;
    while(tokens[iTk].code==ADD || tokens[iTk].code==SUB){
        consume(tokens[iTk].code);
        if(!exprMul()) 
        {
            if(tokens[iTk-1].code == ADD)
            tkerr("lipsește termenul după operatorul'+'");
            else if(tokens[iTk-1].code == SUB)
            tkerr("lipsește termenul după operatorul '-'");
        }
    }
    return true;
}

// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul(){
    if(!exprPrefix()) return false;
    while(tokens[iTk].code==MUL || tokens[iTk].code==DIV){
        consume(tokens[iTk].code);
        if(!exprPrefix())
        {
            if(tokens[iTk-1].code == MUL)
            tkerr("lipsește termenul după operatorul'*'");
            else if(tokens[iTk-1].code == DIV)
            tkerr("lipsește termenul după operatorul '/'");
        }
    }
    return true;
}

// exprPrefix ::= ( SUB | NOT )? factor
bool exprPrefix(){
    if(tokens[iTk].code==SUB || tokens[iTk].code==NOT){
        consume(tokens[iTk].code);
    }

     if(!factor()){
        if(tokens[iTk].code == SUB)
            tkerr("lipsește factorul după operatorul  '-'");
        else if(tokens[iTk].code == NOT)
            tkerr("lipsește factorul după operatorul  '!'");
        return false;
     }
    return true;
}

// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
bool factor(){
   int c = tokens[iTk].code;
    if(c==INT){
        consume(INT); 
        return true; 
    }
    if(c==REAL){
        consume(REAL);
        return true;
    }
    if(c==STR){
        consume(STR);
        return true;
    }
     if(c==LPAR){
        consume(LPAR);
        if(!expr()) tkerr("lipsește expresia după '('");
        if(!consume(RPAR)) tkerr("lipsește ')' după expresie");
        return true;
    }

    if(c==ID){
        consume(ID);

        Symbol *s = searchSymbol(consumed->text);
        if(!s) tkerr("undefined symbol: %s", consumed->text);

        if(tokens[iTk].code==LPAR){
            consume(LPAR);
            if(tokens[iTk].code!=RPAR){
                if(!expr()) tkerr("lipsește expresia în apelul funcției");
                while(consume(COMMA)){
                    if(!expr()) tkerr("lipsește expresia după ',' în apelul funcției");
                }
            }
            if(!consume(RPAR)) tkerr("lipsește ')' după argumentele apelului funcției");
            return true;
        }
        return true;
    }
    return false;
}

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt,...){
	fprintf(stderr,"[ERROR] in line %d: ",tokens[iTk].line );
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
     addDomain();// create the global domain
	if (program()) {
    //printf("Program corect sintactic!\n");
    } else {
        printf("Eroare de sintaxă!\n");
    }
     delDomain(); // delete the global domain
}