#ifndef __heapdinE_h
#define __heapdinE_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dijkstra.h"
#include "grafoFS.h"


#define TRUE  1
#define FALSE 0
#define boolean int

#define NO_CALL NULL
#define NO_SERV NULL
/* #define NO_TAXI -1  commentato, perche' i campi taxi degli eventi chiamate 
   vengono marcati con indici negativi decrescenti in ordine di lettura da file,
   per ottimizzare la gestione dei confronti di eventi. */
#define FINE_RICARICA 1
#define RIENTRO_SEDE 2
#define FINE_SERVIZIO 3
#define CHIAMATA 4

#define ROW_LENGTH 256

/* Una chiamata e' un record con i campi che sono tutte le informazioni utili di una chiamata. */
typedef struct _chiamata chiamata;
struct _chiamata {
  char cliente[ROW_LENGTH];
  nodo nodoOrig;
  nodo nodoDest;
  int hmin;
  int hmax;
  int premio;
  /* I seguenti campi sono univocamente individuati grazie alle matrici dei cammini minimi C e P (vedi dijkstra.c); */
  int durata;
  nodo *nodiVis;
  int nNodi; /* Il numero di nodi visitati, compresi origine e destinazione. */
};

/* Un evento e' un record che ammette 4 tipi diversi. */
typedef struct _evento evento;
struct _evento {
  int tempo; /* L'orario assoluto in cui si verifica l'evento. */
  short tipo; /* Il tipo dell'evento ( da 1 a 4 ). */
  chiamata* c; /* Un puntatore a chiamata, se l'evento la contiene. */
  int taxi; /* L'indice del taxi associato all'evento, se esistente. */
};


/* Heap Dinamico di Eventi. */
typedef struct _heapdine heapdine;
struct _heapdine
{
  evento *V; /* vettore degli eventi */
  int size; /* dimensione massima della dello heap */
  int last; /* indice dell'ultima posizione occupata dallo heap */
};


void CreaVettoreHeapDinE (heapdine *pH, int size);

void DistruggeHeapDinE (heapdine *pH);

chiamata* CreaChiamata(char *cliente, int nodoOrig, int nodoDest, int hmin, int hmax, int premio, int durata, nodo *nodiVis, int nNodi);

void DistruggeChiamata(evento *e);

/* Questa procedura puo' essere eseguita solo all'inizio della simulazione. */
void AggiungeChiamataHeapDinE(heapdine *pH, int *nChiam, int *tempo, char *cliente, int *nodoOrig, int *nodoDest, int *hmin, int *hmax, int *premio);

/* Questa procedura scrive i campi durata e nodiVis di tutte le chiamate
   in uno heapine, grazie alle matrici dei cammini minimi C e P che si suppone gia' calcolate. */
void AggiornaChiamateHeapDinE(grafo *pG, heapdine *pH, peso **C, nodo **P, int n);

boolean VuotoHeapDinE (heapdine *pH);

evento TestaHeapDinE (heapdine *pH);

void CancellaHeapDinE (heapdine *pH);

void SostituisceHeapDinE (evento *e, heapdine *pH);

void CreaHeapDinE (heapdine *pH);
 
void AggiornaHeapDinE (heapdine *pH, int i);

void ScambiaE (evento *pa, evento *pb);

boolean ConfrontaEventi(evento *e1, evento *e2);

void ScriveEvento(evento *e, int tempo, short tipo, chiamata *c, int taxi);

#endif
