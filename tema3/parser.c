    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include "lexer.h"
    #include "ad.h"
    #include "at.h"

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
        ret.lval = false;
        return true;
    }
    if(tokens[iTk].code == TYPE_REAL){
        consume(TYPE_REAL);
        ret.type = TYPE_REAL;
        ret.lval = false;
        return true;
    }
    if(tokens[iTk].code == TYPE_STR){
        consume(TYPE_STR);
        ret.type = TYPE_STR;
        ret.lval = false;
        return true;
    }
    return false;
}

    // defVar ::= VAR ID COLON baseType SEMICOLON
    bool defVar(){
        if(tokens[iTk].code != VAR) return false;
        consume(VAR);
        if(!consume(ID)) tkerr("lipseste numele variabilei dupa 'var'");

        /* actiune semantica: adauga simbol pentru variabila */
        const char *name=consumed->text;
        Symbol *s=searchInCurrentDomain(name);
        if(s)tkerr("symbol redefinition: %s",name);
        s=addSymbol(name,KIND_VAR);
        s->local=crtFn!=NULL;

        if(!consume(COLON)) tkerr("lipseste ':' dupa numele variabilei");
        if(!baseType()) tkerr("lipseste tipul dupa ':' in declaratia de variabila");

        /* actiune semantica: seteaza tipul simbolului adaugat */
        s->type = ret.type;

        if(!consume(SEMICOLON)) tkerr("lipseste ';' la finalul declaratiei de variabila");
        return true;
    }

    // defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
    bool defFunc(){
        if(tokens[iTk].code != FUNCTION) return false;
        consume(FUNCTION);
        if(!consume(ID)) tkerr("lipsește numele funcției după 'function'");

        /* actiune semantica: creeaza simbolul functiei si un nou domeniu */
        const char *name = consumed->text;
        Symbol *s = searchInCurrentDomain(name);
        if(s) tkerr("symbol redefinition: %s", name);
        crtFn = addSymbol(name, KIND_FN);
        crtFn->args = NULL;
        addDomain(); /* domeniu nou pentru corpul functiei si parametri */

        if(!consume(LPAR)) tkerr("lipsește '(' după numele funcției");
        if(tokens[iTk].code != RPAR){
            if(!funcParams()) tkerr("parametri invalizi pentru funcție");
        }
        if(!consume(RPAR)) tkerr("lipsește ')' după lista de parametri a funcției");
        if(!consume(COLON)) tkerr("lipsește ':' după antetul funcției");
        if(!baseType()) tkerr("lipsește tipul de return după ':' în antetul funcției");

        /* actiune semantica: seteaza tipul de return al functiei */
        crtFn->type = ret.type;

        while(defVar()){}
        if(!block()) tkerr("lipsește corpul funcției (block)");
        if(!consume(END)) tkerr("lipsește 'end' după corpul funcției");

        /* paraseste domeniul functiei */
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
        /* verifica redefinire in domeniul curent */
        Symbol *s = searchInCurrentDomain(argName);
        if(s) tkerr("symbol redefinition: %s", argName);

        if(!consume(COLON)) tkerr("lipsește ':' după numele parametrului");
        if(!baseType()) tkerr("lipsește tipul după ':' în declarația parametrului");

        /* actiune semantica: adauga parametrul in domeniul curent si in lista de argumente a functiei */
        s = addSymbol(argName, KIND_ARG);
        s->local = crtFn != NULL;
        s->type = ret.type;
        /*  inregistreaza in lista de argumente a functiei si seteaza tipul */
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

                /* semantic action: check that condition type is not STR */
                if(ret.type==TYPE_STR) tkerr("the if condition must have type int or real");

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

             /* semantic action: check return statement is in a function and types match */
              if(!crtFn) tkerr("return can be used only in a function");
              if(ret.type!=crtFn->type) tkerr("the return type must be the same as the function return type");
              
            if(!consume(SEMICOLON)) tkerr("lipsește ';' după expresia return");
            return true;
        }

        if(c==WHILE){
            consume(WHILE);
            if(!consume(LPAR)) tkerr("lipsește '(' după 'while'");
            if(!expr()) tkerr("condiție invalidă pentru 'while' sau lipsește expresia");

              /* semantic action: check that condition type is not STR */
             if(ret.type==TYPE_STR) tkerr("the while condition must have type int or real");
             
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

             /* semantic action: check that operands have type int or real */
            Ret leftType = ret;
            if(leftType.type==TYPE_STR) tkerr("the left operand of '&&' or '||' cannot be of type string");

            if(!exprAssign()) 
            {
                if (tokens[iTk-1].code == AND)
                tkerr("lipsește expresia după operatorul logic '&&'");
                else if (tokens[iTk-1].code == OR)
                tkerr("lipsește expresia după operatorul logic '||'");
            }

            if(ret.type==TYPE_STR) tkerr("the right operand of '&&' or '||' cannot be of type string");
            setRet(TYPE_INT,false); /* logical expressions have int type */

        }
        return true;
    }

    // exprAssign ::= ( ID ASSIGN )? exprComp
    bool exprAssign(){
    if(tokens[iTk].code==ID && tokens[iTk+1].code==ASSIGN){
        consume(ID);
        /* semantic action: check that ID is a variable and is lval */
        const char *name = consumed->text;

        consume(ASSIGN);
        if(!exprComp()) tkerr("expected expression after '='");

        Symbol *s = searchSymbol(name);
        if(!s) tkerr("undefined symbol '%s'", name);
        if(s->kind == KIND_FN) tkerr("a function (%s) cannot be used as a destination for assignment ", name);
        if(s->type!=ret.type) tkerr("the source and destination for assignment must have same type");
        ret.lval = false;

        return true;
    }
    return exprComp();
}
    // exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
    bool exprComp(){
        if(!exprAdd()) return false;
        if(tokens[iTk].code==LESS || tokens[iTk].code==EQUAL){
            consume(tokens[iTk].code);

            /* semantic action: */
            Ret leftType = ret;

            if(!exprAdd())
            {
                if (tokens[iTk-1].code == LESS)
                tkerr("lipsește expresia după operatorul de comparație '<'");
                else if (tokens[iTk-1].code == EQUAL)
                tkerr("lipsește expresia după operatorul de comparație '=='");
            } 

            if(leftType.type!=ret.type) tkerr("different types for the operands of '<' or '=='");
             setRet(TYPE_INT, false);
        }
        return true;
    }

    // exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
    bool exprAdd(){
        if(!exprMul()) return false;
        while(tokens[iTk].code==ADD || tokens[iTk].code==SUB){
            consume(tokens[iTk].code);

             /* semantic action: */
            Ret leftType = ret;
            if(leftType.type == TYPE_STR) tkerr("the operands of '+' or '-' cannot be of type str");

            if(!exprMul()) 
            {
                if(tokens[iTk-1].code == ADD)
                tkerr("lipsește termenul după operatorul'+'");
                else if(tokens[iTk-1].code == SUB)
                tkerr("lipsește termenul după operatorul '-'");
            }

            /* semantic action: */
            if(leftType.type != ret.type) tkerr("different types for the operands of '+' or '-'");
        }
        return true;
    }

    // exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
    bool exprMul(){
        if(!exprPrefix()) return false;
        while(tokens[iTk].code==MUL || tokens[iTk].code==DIV){
            consume(tokens[iTk].code);

            /* semantic action: */
            Ret leftType=ret;
            if(leftType.type==TYPE_STR)tkerr("the operands of * or / cannot be of type str");

            if(!exprPrefix())
            {
                if(tokens[iTk-1].code == MUL)
                tkerr("lipsește termenul după operatorul'*'");
                else if(tokens[iTk-1].code == DIV)
                tkerr("lipsește termenul după operatorul '/'");
            }

             if(leftType.type!=ret.type)tkerr("different types for the operands of * or /");
                ret.lval=false;

        }
        return true;
    }

    // exprPrefix ::= ( SUB | NOT )? factor
   bool exprPrefix() {
        int code = tokens[iTk].code;
        int retFromFactor;

    switch (code){
        case SUB:
            consume(tokens[iTk].code);
            retFromFactor = factor();
            /* semantic analysis: */
            if(ret.type==TYPE_STR)tkerr("the expression of unary - must be of type int or real");
            ret.lval=false;
            break;
        case NOT:
            consume(tokens[iTk].code);
            retFromFactor = factor();
            /* semantic analysis: */
            if(ret.type==TYPE_STR)tkerr("the expression of ! must be of type int or real");
            setRet(TYPE_INT,false);
            break;
        default:
            retFromFactor = factor();
    }
     return retFromFactor;
}

    // factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
    bool factor(){
         int c = tokens[iTk].code;
     if(c==INT){
        consume(INT); 
        setRet(TYPE_INT, false);
        return true; 
    }
    if(c==REAL){
        consume(REAL);
        setRet(TYPE_REAL, false);
        return true;
    }
    if(c==STR){
        consume(STR);
        setRet(TYPE_STR, false);
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

            /* semantic action: check that the symbol is defined */
            Symbol *s = searchSymbol(consumed->text);
            if(!s) tkerr("Undefined symbol: %s", consumed->text); 

            if(tokens[iTk].code==LPAR){
                consume(LPAR);

                /* semantic action: check that the symbol is a function */
                  if(s->kind!= KIND_FN) tkerr("%s cannot be called, because it is not a function", s->name);
                  Symbol *argDef = s->args;

                if(tokens[iTk].code!=RPAR){
                    if(!expr()) tkerr("lipsește expresia în apelul funcției");

                        /* semantic action: check argument types */
                     if(!argDef) tkerr("the function %s is called with too many arguments", s->name);
                     if(argDef->type != ret.type) tkerr("the argument type at function %s call is different from the one given at its definition", s->name);
                     argDef=argDef->next;

                    while(consume(COMMA)){
                        if(!expr()) tkerr("lipsește expresia după ',' în apelul funcției");
                        if(!argDef)tkerr("the function %s is called with too many arguments",s->name);
                        if(argDef->type!=ret.type)tkerr("the argument type at function %s call is different from the one given at its definition",s->name);
                        argDef=argDef->next;
                    }
                }
                if(!consume(RPAR)) tkerr("lipsește ')' după argumentele apelului funcției");
                if(argDef)tkerr("the function %s is called with too few arguments",s->name);
                setRet(s->type,false);
                return true;
             } else {
            /* not a call: must be a variable (lvalue), not a function */
            if(s->kind==KIND_FN) tkerr("the function %s can only be called", s->name);
            setRet(s->type,true);
            return true;
        }
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
        addDomain(); // creeaza domeniul global
        addPredefinedFns();
        for(;;){
            if(defVar()){}
            else if(defFunc()){}
            else if(block()){}
            else break;
            }
         if(!consume(FINISH)) tkerr("syntax error");

    delDomain(); // sterge domeniul global
    return true;
        }

    void parse(){
        iTk=0;
        if (program()) {
        //printf("Program corect sintactic!\n");
        } else {
            printf("Eroare de sintaxă!\n");
        }
    }