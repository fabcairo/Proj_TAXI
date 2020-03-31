#include "zaino.h"


void CaricaDatiKP(KPdata *pI, heapdine *pH, int T, int nv)
{
  int i;
  
  pI->n = pH->last;
  pI->V = T*nv;  
  
  pI->phi = (long *) calloc(pI->n+1,sizeof(long));
  pI->v = (long *) calloc(pI->n+1,sizeof(long));
  for(i = 1; i <= pI->n; i++)
  {
    pI->phi[i] = pH->V[i].c->durata + pH->V[i].c->premio;
    pI->v[i] = pH->V[i].c->durata;
  }
  
}

/* Dealloca le strutture dati dinamiche */
void DistruggeDatiKP (KPdata *pI)
{
  free(pI->phi);
  free(pI->v);
  pI->n = 0;
  pI->phi = NULL;
  pI->v = NULL;
  pI->V = 0;
}


/* Crea una soluzione vuota */
void CreaSoluzione (int n, KPsolution *pS)
{
  pS->f = 0L;
  pS->x = (vboolean) calloc(n+1,sizeof(boolean));
  if (pS->x == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della soluzione!\n");
    exit(EXIT_FAILURE);
  }
}


/* Distrugge la soluzione S */
void DistruggeSoluzioneKP(KPsolution *pS)
{
  pS->f = 0L;
  free(pS->x);
  pS->x = NULL;
}



void CreaHeapIndiretto (vlong Index, vdouble phi_v, int dim)
{
  int i;

  for (i = dim/2; i >= 1; i--)
    AggiornaHeapIndiretto(Index,phi_v,dim,i);
}


void AggiornaHeapIndiretto (vlong Index, vdouble phi_v, int dim, int i)
{
  int s, d;
  int iMax;

  s = 2*i;
  d = 2*i+1;

  iMax = i;
  if ( (s <= dim) && (phi_v[Index[iMax]] < phi_v[Index[s]]) ) iMax = s;
  if ( (d <= dim) && (phi_v[Index[iMax]] < phi_v[Index[d]]) ) iMax = d;

  if (iMax != i)
  {
    ScambiaLONG(&Index[iMax],&Index[i]);
    AggiornaHeapIndiretto(Index,phi_v,dim,iMax);
  }
}


void ScambiaLONG (long *pa, long *pb)
{
  long temp;

  temp = *pa;
  *pa = *pb;
  *pb = temp;
}


/* Risolve il knapsack problem con l'euristica greedy raffinata */
void GreedyKP (KPdata *pI, KPsolution *pS)
{
  vlong Index;
  vdouble phi_v;
  long dim;
  long i, iMax;
  long v;


  Index = (vlong) calloc(pI->n+1,sizeof(long));
  if (Index == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione di Index!\n");
    exit(EXIT_FAILURE);
  }

  for (i = 1; i <= pI->n; i++)
    Index[i] = i;

  phi_v = (vdouble) calloc(pI->n+1,sizeof(double));
  if (phi_v == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione di phi_v!\n");
    exit(EXIT_FAILURE);
  }

  for (i = 1; i <= pI->n; i++)
    phi_v[i] = ((double) pI->phi[i])/pI->v[i];

  dim = pI->n;
  CreaHeapIndiretto(Index,phi_v,dim);

  v = 0L;
  while ( (dim > 0) && (v < pI->V) )
  {
    /* Trova l'elemento di 'premio diviso volume' massimo */
    iMax = Index[1];

    /* Se tale elemento sta nello zaino, lo aggiunge allo zaino */
    if (v + pI->v[iMax] <= pI->V)
    {
      pS->x[iMax] = TRUE;
      pS->f = pS->f + pI->phi[iMax];
      v = v + pI->v[iMax];
    }

        /* Cancella l'elemento considerato dall'heap */
    Index[1] = Index[dim];
    dim--;
    AggiornaHeapIndiretto(Index,phi_v,dim,1);
  }

  free(Index);
  free(phi_v);
}
