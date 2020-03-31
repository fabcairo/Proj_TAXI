#include "grafoFS.h"

/* Crea un grafo *pG di n nodi, senza archi */
void crea_grafo (int n, grafo *pG)
{
  int i;

  pG->n = n;
  pG->m = 0;

  pG->FS = (listaarchi *) calloc(pG->n+1,sizeof(listaarchi));
  if (pG->FS ==  NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della forward-star!\n");
    exit(EXIT_FAILURE);
  }

  for (i = 1; i <= n; i++)
    pG->FS[i] = crealistaarchi();
}


/* Distrugge il grafo *pG */
void distrugge_grafo (grafo *pG)
{
  int i;

  for (i = 1; i <= pG->n; i++)
    distruggelistaarchi(&pG->FS[i]);
  free(pG->FS);
  pG->FS = NULL;

  pG->n = 0;
  pG->m = 0;
}


/* Aggiunge l'arco (dest,cost) al grafo *pG in corrispondenza di orig.
   Inserisce l'arco in fondo alla lista corrispondente al nodo orig.
   Non verifica che l'arco non esista gia'! */
void ins_arco (nodo orig, nodo dest, peso cost, grafo *pG)
{
  listaarchi L = pG->FS[orig];
  inslistaarchi(L, succlistaarchi(L,ultimolistaarchi(L)), dest, cost);
  pG->m++;
}


/* Cancella l'arco (dest,cost) (per qualsiasi cost) con origine in orig dal grafo *pG. */
void canc_arco (nodo orig, nodo dest, grafo *pG)
{
  listaarchi L;
  posarco p;
  nodo n1, n2;
  boolean trovato;

  L = pG->FS[orig]; /* Uso l'implementazione della FS per sfoltire subito la ricerca. */
  p = primolistaarchi(L);
  trovato = FALSE;
  while (!trovato && !finelistaarchi(L,p))
  {
    leggearco(L,p,&n1,&n2); /* n2 inutilizzato. */
    trovato = ( n1 == dest );
    if (!trovato) p = succlistaarchi(L,p);
  }

  if (trovato)
  {
    L = canclistaarchi(L,&p);
    pG->m--;
  }
}


/* Determina se l'arco (dest,cost) (per qualsiasi cost) con origine in orig appartiene al grafo *pG */
boolean esiste_arco (nodo orig, nodo dest, grafo *pG)
{
  listaarchi L = pG->FS[orig];
  posarco pa;
  nodo n;
  peso w;

  for (pa = primolistaarchi(L); !finelistaarchi(L,pa); pa = succlistaarchi(L,pa))
  {
    leggearco(L,pa,&n,&w); /* w inutilizzato. */
    if ( n == dest ) return TRUE;
  }

  return FALSE;

  /* si potrebbe anche fare:
  return ( leggepeso(pG,orig,dest) > 0 ); */
}


/* Restituisce il peso dell'arco puntato da pa. */
peso leggepeso (nodo orig, posarco pa, grafo *pG) /* orig SUPERFLUO (vedi leggearco). */
{
  nodo i;
  peso j;

  leggearco(pG->FS[orig],pa,&i,&j);
  return j;
}


posarco primoarcoFS (nodo orig, grafo *pG)
{
  return primolistaarchi(pG->FS[orig]);
}


boolean finearchiFS (nodo orig, posarco pa, grafo *pG)
{
  return finelistaarchi(pG->FS[orig],pa);
}


posarco succarcoFS (nodo orig, posarco pa, grafo *pG)
{
  return succlistaarchi(pG->FS[orig],pa);
}


nodo leggedest (nodo orig, posarco pa, grafo *pG) /* orig SUPERFLUO (vedi leggearco) */
{
  nodo i;
  peso j;

  leggearco(pG->FS[orig],pa,&i,&j);
  return i;
}
