#ifndef __grafofs_h
#define __grafofs_h

#include <stdio.h>
#include <stdlib.h>

#include "listaarchiFS.h"


#define TRUE  1
#define FALSE 0
#define boolean int


typedef struct _grafo grafo;

struct _grafo
{
  int n;
  int m;
  listaarchi *FS;
};


/* Crea un grafo *pG di n nodi, senza archi */
void crea_grafo (int n, grafo *pG);

/* Distrugge il grafo *pG */
void distrugge_grafo (grafo *pG);

/* Aggiunge l'arco (dest,cost) al grafo *pG in corrispondenza di orig.
   Non verifica che l'arco non esista gia'! */
void ins_arco (nodo orig, nodo dest, peso cost, grafo *pG);

/* Cancella l'arco (dest,cost) (per qualsiasi cost) con origine in orig dal grafo *pG. */
void canc_arco (nodo orig, nodo dest, grafo *pG);

/* Determina se l'arco (dest,cost) (per qualsiasi cost) con origine in orig appartiene al grafo *pG */
boolean esiste_arco (nodo orig, nodo dest, grafo *pG);

/* Restituisce il peso dell'arco puntato da pa. */
peso leggepeso (nodo orig, posarco pa, grafo *pG);

posarco primoarcoFS (nodo orig, grafo *pG);

boolean finearchiFS (nodo orig, posarco pa, grafo *pG);

posarco succarcoFS (nodo orig, posarco pa, grafo *pG);

nodo leggedest (nodo orig, posarco pa, grafo *pG);

#endif
