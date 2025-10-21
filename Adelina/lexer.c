#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>   // pentru atoi si atof

#include "lexer.h"
#include "utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code){
	if(nTokens==MAX_TOKENS)err("too many tokens");
	Token *tk=&tokens[nTokens];
	tk->code=code;
	tk->line=line;
	nTokens++;
	return tk;
	}

// copy in the dst buffer the string between [begin,end)
char *copyn(char *dst,const char *begin,const char *end){
	char *p=dst;
	if(end-begin>MAX_STR)err("string too long");
	while(begin!=end)*p++=*begin++;
	*p='\0';
	return dst;
	}

void tokenize(const char *pch){
	const char *start;
	Token *tk;
	char buf[MAX_STR+1];
	for(;;){
		switch(*pch){
			case ' ':case '\t':pch++;break;
			case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
				if(pch[1]=='\n')pch++;
				// fallthrough to \n
			case '\n':
				line++;
				pch++;
				break;

			case '\0':addTk(FINISH);return; //sfarsit de fisier
			case ',':addTk(COMMA); pch++; break;
            case ';':addTk(SEMICOLON); pch++; break;
            case ':':addTk(COLON); pch++; break;
            case '(':addTk(LPAR); pch++; break;
            case ')':addTk(RPAR); pch++; break;
			case '+':addTk(ADD); pch++; break;
            case '-':addTk(SUB); pch++; break;
            case '*':addTk(MUL); pch++; break;
			case '&':
			if (pch[1] == '&') {
				addTk(AND);
				pch += 2;
			} else {
				err("Lipseste al doilea & pentru operatorul && %d",line);
			}
			break;
			case '|':
			if (pch[1] == '|') {
					addTk(OR);
					pch += 2;
			} else {
					err("Lipseste al doilea | pentru operatorul ||");
				}
				break;
			case '/':
				if(pch[1] == '/') {
					// sari peste comentariu pana la newline
					pch += 2;
					while(*pch != '\n' && *pch != '\0') pch++;
				} else {
					addTk(DIV);
					pch++;
				}
				break;
			case '<':
                if (pch[1] == '=') { addTk(LESSEQ); pch += 2; }
                else { addTk(LESS); pch++; }
                break;
            case '>':
                if (pch[1] == '=') { addTk(GREATEREQ); pch += 2; }
                else { addTk(GREATER); pch++; }
                break;
            case '!':
                if (pch[1] == '=') { addTk(NOTEQ); pch += 2; }
                else { addTk(NOT); pch++; }
                break;
			case '=':
				if(pch[1]=='='){
					addTk(EQUAL);
					pch+=2;
					}else{
					addTk(ASSIGN);
					pch++;
					}
				break;
			case '"':
				start = ++pch;
				while(*pch != '"' && *pch != '\0') pch++;
				if(*pch != '"') err("Sir neterminat"); //mesaj eroare
				tk = addTk(STR);   // folosește codul STR pentru constante string
				copyn(tk->text, start, pch);
				pch++;

				break;
			default:
			//id sau cuvant cheie
				if(isalpha(*pch)||*pch=='_'){
					for(start=pch++;isalnum(*pch)||*pch=='_';pch++){}
					char *text=copyn(buf,start,pch);
					if(strcmp(text,"int")==0)addTk(TYPE_INT);
					else if(strcmp(text,"real")==0)addTk(TYPE_REAL);
					else if(strcmp(text,"str")==0)addTk(TYPE_STR);
					else if(strcmp(text,"var")==0)addTk(VAR);
					else if(strcmp(text,"function")==0)addTk(FUNCTION);
					else if(strcmp(text,"if")==0)addTk(IF);
					else if(strcmp(text,"else")==0)addTk(ELSE);
					else if(strcmp(text,"while")==0)addTk(WHILE);
					else if(strcmp(text,"end")==0)addTk(END);
					else if(strcmp(text,"return")==0)addTk(RETURN);
					else { // dacă nu e cuvânt cheie → ID
						tk = addTk(ID);
						strcpy(tk->text, text);
					}
				}
				 // INT sau REAL
					else if(isdigit(*pch)){
						start = pch;
						int isReal = 0;
						while(isdigit(*pch)) pch++;

						if(*pch == '.'){
							if(isdigit(pch[1])){       // verificam daca urmeaza partea zecimala
								isReal = 1;
								pch++;                 // sarim peste .
								while(isdigit(*pch)) pch++;
							} else {
								err("Numar real incomplet la linia %d", line); //mesaj eroare
							}
						}

						copyn(buf, start, pch);
						tk = addTk(isReal ? REAL : INT);
						if(isReal) tk->r = atof(buf); //convertim in double si il salvam in tk->r
						else tk->i = atoi(buf); //convertim in int si il salvam in tk->i
					}
				else err("invalid char: %c (%d)",*pch,*pch);
			}
		}
	}

void showTokens() {
    static const char *tokenNames[] = {
        "ID","VAR","FUNCTION","IF","ELSE","WHILE","END","RETURN",
        "TYPE_INT","TYPE_REAL","TYPE_STR",
        "COMMA","SEMICOLON","COLON","LPAR","RPAR","FINISH",
        "ADD","SUB","MUL","DIV","AND","OR","NOT","ASSIGN","EQUAL","NOTEQ",
        "LESS","GREATER","GREATEREQ","LESSEQ",
   		 "INT","REAL","STR"
    };

    for(int i = 0; i < nTokens; i++){
        Token *tk = &tokens[i];
        int code = tk->code;
        const char *name = (code >= 0 && code < (int)(sizeof(tokenNames)/sizeof(tokenNames[0]))) //verificam daca code este in intervalul valid
                            ? tokenNames[code] : "UNKNOWN";

        printf("%d %s", tk->line, name);

        // afișează valoarea pentru tokenii cu text sau număr
        if(code == ID) printf(":%s", tk->text);
			else if(code == INT) printf(":%d", tk->i);
			else if(code == REAL) printf(":%g", tk->r);
			else if(code == STR) printf(":%s", tk->text);
        printf("\n");
    }
}