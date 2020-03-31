#ifndef __dijkstra_h
#define __dijkstra_h

#include "grafoFS.h"
#define INFINITY 10e5

typedef int* vint;
typedef boolean* vboolean;

/* Heap Dinamico Indiretto di Cammini. */
typedef struct _heapdic heapdic;
struct _heapdic
{
  nodo *V; /* vettore dei nodi */
  int size; /* dimensione massima della dello heap */
  int last; /* indice dell'ultima posizione occupata dallo heap */
};


void CreaHeapDIC (heapdic *pH, int size);

void DistruggeHeapDIC(heapdic *pH);

boolean VuotoHeapDIC (heapdic *pH);

void CalcolaCamminiMinimi(grafo *pG, peso **C, nodo **P, nodo s);

void Dijkstra(grafo *pG, nodo s, heapdic *pH, peso **C, nodo **P);

void RiempieMatrici(nodo s, peso **C, nodo **P, int n);

void CamminiRicorsiva(nodo f, nodo s, peso **C, nodo **P);

int TrovaFoglie(nodo s, nodo **P, int n, nodo *f);

void SostituisceHeapDIC(heapdic *pH, nodo s, peso **C, nodo **P, peso newc, nodo newp, int i, vint Index);

nodo TestaHeapDIC(heapdic *pH);

void CancellaHeapDIC(heapdic *pH, nodo s, peso **C, nodo **P, vint Index);

void AggiornaSopraHeapDIC(heapdic *pH, nodo s, peso **C, int i, vint Index);

void AggiornaSottoHeapDIC (heapdic *pH, nodo s, peso **C, int i, vint Index);

/* Legge l'elemento (i,j) o (j,i) di una matrice triangolare inferiore di interi. */
int LeggeMTI(int **M, int i, int j);

/* (Sovra)scrive sull'elemento (i,j) o (j,i) di una matrice triangolare inferiore di interi un intero dato. */
void ScriveMTI(int **M, int i, int j, int new);

void ScambiaInt (int *pa, int *pb);

int TrovaIndiceNodo(heapdic *pH, nodo n);

nodo** AllocaMQ(int n);

peso** AllocaMTI(int n);

void InizializzaMTI(int **C, int n, int init);

void InizializzaMQ(int **P, int n, int init);

peso LeggeCompletataC(grafo *pG, peso **C, nodo **P, int i, int j);

#endif