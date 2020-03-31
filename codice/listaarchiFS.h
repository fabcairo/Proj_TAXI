#ifndef __listaarchiarchi_h
#define __listaarchiarchi_h

#include <stdio.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0
#define boolean int


typedef struct _arco arco;
typedef arco* listaarchi;
typedef arco* posarco;
typedef int nodo;
typedef int peso;

#define NO_ARC   NULL
#define NO_NODE     0
#define NO_WEIGHT   0

struct _arco
{
  nodo dest;
  peso cost;
  posarco succ, pred;
};


listaarchi crealistaarchi (); 

void distruggelistaarchi (listaarchi *pL);

void leggearco (listaarchi L, posarco p, nodo *pdest, peso *pcost);

listaarchi scrivearco (listaarchi L, posarco p, nodo dest, peso cost);

posarco primolistaarchi (listaarchi L);

posarco ultimolistaarchi (listaarchi L);

boolean finelistaarchi (listaarchi L, posarco p);

boolean listaarchivuota (listaarchi L);

posarco preclistaarchi (listaarchi L, posarco p);

posarco succlistaarchi (listaarchi L, posarco p);

listaarchi inslistaarchi (listaarchi L, posarco p, nodo dest, peso cost);

listaarchi canclistaarchi (listaarchi L, posarco *pp);

#endif
