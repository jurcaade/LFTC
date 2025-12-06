#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ad.h"
#include "utils.h"

Ret ret; // retine tipul ultimului element analizat
Domain *symTable;  // pointer catre domeniul curent
Symbol *crtFn; // simbolul functiei curente

Domain *addDomain(){
	puts("creates a new domain");
	Domain *d=(Domain*)safeAlloc(sizeof(Domain));
	d->parent=symTable; //domeniul parinte este cel curent
	d->symbols=NULL;
	symTable=d; // actualizeaza domeniul curent
	return d;
	}

void delSymbols(Symbol *list);

void delSymbol(Symbol *s){
	printf("\tdeletes the symbol %s\n",s->name);
	 // daca simbolul este o functie, sterge si argumentele ei
	if(s->kind==KIND_FN){
		delSymbols(s->args);
		}
	free(s);
	}

void delSymbols(Symbol *list){
	for(Symbol *s1=list,*s2;s1;s1=s2){
		s2=s1->next; // salveaza urmatorul inainte sa stergem
		delSymbol(s1); // sterge simbolul curent
		}
	}

void delDomain(){
	puts("deletes the current domain");
	Domain *parent=symTable->parent;
	delSymbols(symTable->symbols);  // sterge toate simbolurile domeniului curent
	free(symTable);
	symTable=parent;  // seteaza domeniul curent la parinte
	puts("returns to the parent domain");
	}

Symbol *searchInList(Symbol *list,const char *name){
	for(Symbol *s=list;s;s=s->next){
		if(!strcmp(s->name,name))return s;
		}
	return NULL;
	}

Symbol *searchInCurrentDomain(const char *name){
	return searchInList(symTable->symbols,name);
	}

Symbol *searchSymbol(const char *name){
	for(Domain *d=symTable;d;d=d->parent){
		Symbol *s=searchInList(d->symbols,name);
		if(s)return s;
		}
	return NULL;
	}

Symbol *createSymbol(const char *name,int kind){
	Symbol *s=(Symbol*)safeAlloc(sizeof(Symbol));
	s->name=name;
	s->kind=kind; // ex: KIND_VAR, KIND_FN, KIND_ARG
	return s;
	}

Symbol *addSymbol(const char *name,int kind){
	printf("\tadds symbol %s\n",name);
	Symbol *s=createSymbol(name,kind);
	// adauga simbolul la inceputul listei domeniului curent
	s->next=symTable->symbols;
	symTable->symbols=s;
	return s;
	}

// Adauga un argument al functiei in lista de argumente a functiei
Symbol *addFnArg(Symbol *fn,const char *argName){
	printf("\tadds symbol %s as argument\n",argName);
	Symbol *s=createSymbol(argName,KIND_ARG);
	s->next=NULL;
	if(fn->args){
		  // daca lista de argumente exista, mergem la final si adaugam
		Symbol *p;
		for(p=fn->args;p->next;p=p->next){}
		p->next=s;
		}else{
			 // daca lista este goala, argumentul devine primul
		fn->args=s;
		}
	return s;
	}

