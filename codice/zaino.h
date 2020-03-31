
/* Direttive */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heapdinE.h"

typedef long * vlong;
typedef double * vdouble;

typedef struct _KPdata KPdata;
struct _KPdata
{
  long n;
  long V;
  vlong phi;
  vlong v;
};

typedef struct _KPsolution KPsolution;
struct _KPsolution
{
  long f;
  vboolean x;
};

void CaricaDatiKP(KPdata *pI, heapdine *pH, int T, int nv);

/* Distrugge l'istanza I */
void DistruggeDatiKP (KPdata *pI);

/* Crea una soluzione vuota */
void CreaSoluzione (int n, KPsolution *pS);

/* Distrugge la soluzione S */
void DistruggeSoluzioneKP(KPsolution *pS);

/* Risolve il knapsack problem con l'euristica greedy raffinata */
void GreedyKP (KPdata *pI, KPsolution *pS);

void CreaHeapIndiretto (vlong Index, vdouble phi_v, int dim);

void AggiornaHeapIndiretto (vlong Index, vdouble phi_v, int dim, int i);

void ScambiaLONG (long *pa, long *pb);
