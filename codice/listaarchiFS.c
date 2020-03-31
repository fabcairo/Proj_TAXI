#include "listaarchiFS.h"

listaarchi crealistaarchi ()
{
  listaarchi L = (listaarchi) malloc(sizeof(arco));
  if (L == NULL)
  {
    fprintf(stderr,"Memoria insufficiente per allocare un arco!\n");
    exit(EXIT_FAILURE);
  }

  L->dest = 0;
  L->cost = 0;

  L->succ = L;
  L->pred = L;

  return L;
}


void distruggelistaarchi (listaarchi *pL)
{
  posarco p;

  while (!listaarchivuota(*pL))
  {
    p = primolistaarchi(*pL);
    *pL = canclistaarchi(*pL,&p);
  }
  free(*pL);
  *pL = NULL;
}


void leggearco (listaarchi L, posarco p, nodo *pdest, peso *pcost) /* L SUPERFLUO. */
{
  *pdest = p->dest;
  *pcost = p->cost;
}


listaarchi scrivearco (listaarchi L, posarco p, nodo dest, peso cost)
{
  p->dest = dest;
  p->cost = cost;
  return L;
}


boolean listaarchivuota (listaarchi L)
{
  return (L->succ == L);
}


posarco primolistaarchi (listaarchi L)
{
  return L->succ;
}


posarco ultimolistaarchi (listaarchi L)
{
  return L->pred;
}


posarco succlistaarchi (listaarchi L, posarco p)
{
  return p->succ;
}


posarco preclistaarchi (listaarchi L, posarco p)
{
  return p->pred;
}


boolean finelistaarchi (listaarchi L, posarco p)
{
  return (p == L);
}


/* Inserisce A SINISTRA di p un arco. */
listaarchi inslistaarchi (listaarchi L, posarco p, nodo dest, peso cost)
{
  posarco q = (posarco) malloc(sizeof(arco));
  if (q == NULL)
  {
    fprintf(stderr,"Memoria insufficiente per allocare un arco!\n");
    exit(EXIT_FAILURE);
  }
  q->dest = dest;
  q->cost = cost;
  q->pred = p->pred;
  q->succ = p;
  p->pred->succ = q;
  p->pred = q;
  return L;
}


listaarchi canclistaarchi (listaarchi L, posarco *pp)
{
  posarco p = *pp;

  *pp = p->succ;
  p->pred->succ = p->succ;
  p->succ->pred = p->pred;
  free(p);

  return L;
}
