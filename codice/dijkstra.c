#include "dijkstra.h"


void CreaHeapDIC (heapdic *pH, int size)
{
  pH->V = (nodo *) calloc(size+1,sizeof(nodo));
  if (pH->V == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della pila!\n");
    exit(EXIT_FAILURE);
  }

  pH->size = size;
  pH->last = 0;
  
} 


void DistruggeHeapDIC(heapdic *pH)
{
  free(pH->V);
  pH->V = NULL;
  pH->size = 0;
  pH->last = 0;
}


boolean VuotoHeapDIC (heapdic *pH)
{
  return (pH->last == 0);
}



void CalcolaCamminiMinimi(grafo *pG, peso **C, nodo **P, nodo s)
{
  
  int i;
  heapdic hdic;
  
  /* (Re-)inizializzo la colonna s-esima di C, perche' funzioni correttamente Dijkstra. */
  for(i = 1; i <= pG->n; i++)
	  ScriveMTI(C,i,s,INFINITY);
  
  /* Inizio del lavoro. */
  CreaHeapDIC(&hdic, pG->n);
  
  for(i = 1; i <= pG->n; i++)
  {
    hdic.V[i] = i;
	hdic.last++;
  }
    
  /* DIJKSTRA */
  Dijkstra(pG, s, &hdic, C, P);
  
  /* RIEMPIE MATRICI  */
  RiempieMatrici(s, C, P, pG->n);
  
  DistruggeHeapDIC(&hdic);
}


void Dijkstra(grafo *pG, nodo s, heapdic *pH, peso **C, nodo **P)
{
  nodo v, u;
  listaarchi L;
  posarco pa;
  peso nuovo;
  vint Index = (vint) calloc(pG->n+1,sizeof(int));
  int i;
  
  for(i = 1; i <= pG->n; i++)
  {
    Index[i] = i;
  }
  
  /*i = TrovaIndiceNodo(pH, s);*/
  i = Index[s];
  SostituisceHeapDIC(pH, s, C, P, 0, 0, i, Index);
  
  while( !VuotoHeapDIC(pH) )
  {
    v = TestaHeapDIC(pH);
	CancellaHeapDIC(pH, s, C, P, Index);
	
	L = pG->FS[v];
	for( pa = primolistaarchi(L); !finelistaarchi(L,pa); pa = succlistaarchi(L,pa))
	{
	  u = leggedest(v,pa,pG);
	  
	  nuovo = LeggeMTI(C,s,v) + leggepeso(v,pa,pG);
	  if( nuovo < LeggeMTI(C,s,u) )
	  {
	    /* u sta gia' nello heap anche se C[u][s] e' infinito: il suo valore e' solo aggiornato. */
		/* i = TrovaIndiceNodo(pH, u) */
		i = Index[u];
		if ( i <= pH->last )
		  SostituisceHeapDIC(pH,s,C,P,nuovo,v,i,Index);
		else /* Teoricamente questo blocco non e' mai raggiunto, perche' gli elementi che escono dallo heap non ci rientrano. pero'... */
		{
		  pH->last++;
		  ScambiaInt(&pH->V[pH->last],&pH->V[i]);
		  ScambiaInt(&Index[pH->V[i]], &Index[pH->V[pH->last]]);
		  AggiornaSopraHeapDIC(pH,s,C,pH->last,Index);
		}
	  }
	}
  }
  
  free(Index);
}


void RiempieMatrici(nodo s, peso **C, nodo **P, int n) 
{
  /* Data la colonna s-esima della matrice P, conosco l'albero dei cammini minimi da s a tutti gli altri nodi.
     Sfrutto tale albero per riempire le matrici C e P con TUTTI i cammini minimi intermedi. */
  
  /* Determino le foglie dell'albero, cioe' i nodi che non sono predecessori di nessun altro nodo. */
  nodo *foglie = (nodo *) calloc(n, sizeof(nodo)); /* n-1 e' il numero massimo di foglie */
  int nf = TrovaFoglie(s,P,n,foglie);
  int i;
  
  for(i = 1; i <= nf; i++)
  {
    CamminiRicorsiva(foglie[i],s,C,P);
  }
  
  free(foglie);
}


void CamminiRicorsiva(nodo f, nodo s, peso **C, nodo **P)
{
  nodo p, a, fda; /* padre, antenato e figlio dell'antenato */
  
  p = P[f][s];
  if( p != 0 )
  {
	a = p;
	fda = f;
	while( a != 0 )
	{
      P[a][f] = fda;
      P[f][a] = p;	  
	  ScriveMTI(C,a,f, LeggeMTI(C,f,s) - LeggeMTI(C,a,s) );
	  
	  fda = a;
	  a = P[a][s];
	}
	
	CamminiRicorsiva(p,s,C,P);
  }
}

/* Trova le foglie dell'albero dei cammini minimi dalla sorgente s e le scrive nel vettore f. */
int TrovaFoglie(nodo s, nodo **P, int n, nodo *f)
{
  int i, j = 0;
  nodo *v = (nodo *) calloc(n+1,sizeof(nodo));
  
  /* Inizializzo il vettore di cancellazione. */
  for(i = 1; i <= n; i++)
  {
    v[i] = i;
  }
  /* Cancello i nodi che compaiono gia' in P[][s] */
  for(i = 1; i <= n; i++)
  {
    if( i != s )
	{
	  v[P[i][s]] = 0; /* valore convenzionale, uguale al valore di P[s][s] */
	}
  }
  /* Aggiungo i nodi rimanenti al vettore delle foglie. */
  for(i = 1; i <= n; i++)
  {
    if( v[i] != 0 )
	{
	  j++;
	  f[j] = v[i]; /* nonche' f[j] = i */
	}
  }
  
  free(v);
  return j;
}

void SostituisceHeapDIC(heapdic *pH, nodo s, peso **C, nodo **P, peso newc, nodo newp, int i, vint Index)
{
  
  if( newc < LeggeMTI(C,s,pH->V[i]) || newc == 0 )
  {
    ScriveMTI(C,pH->V[i],s,newc);
	P[ pH->V[i] ][s] = newp;
	AggiornaSopraHeapDIC(pH,s,C,i,Index);
  }
  else /* Questo blocco NON DOVREBBE ESSERE MAI RAGGIUNTO quando chiamato in Dijkstra */
  {
    ScriveMTI(C,pH->V[i],s,newc);
	P[ pH->V[i] ][s] = newp;
	AggiornaSottoHeapDIC(pH,s,C,i,Index);
  }
  
  
}

nodo TestaHeapDIC(heapdic *pH)
{
  return pH->V[1];
}

void CancellaHeapDIC(heapdic *pH, nodo s, peso **C, nodo **P, vint Index)
{
  ScambiaInt(&pH->V[1], &pH->V[pH->last]);
  ScambiaInt(&Index[pH->V[1]], &Index[pH->V[pH->last]]);
  pH->last--;
  AggiornaSottoHeapDIC(pH,s,C,1,Index);
}

void AggiornaSopraHeapDIC(heapdic *pH, nodo s, peso **C, int i, vint Index)
{
  int genit;
  
  genit = i/2;
  
  if( genit > 0 && LeggeMTI(C,s,pH->V[i]) < LeggeMTI(C,s,pH->V[genit]) )
  {
    ScambiaInt(&pH->V[i], &pH->V[genit]);
	ScambiaInt(&Index[pH->V[i]], &Index[pH->V[genit]]);
	AggiornaSopraHeapDIC(pH,s,C,genit,Index);
  }
}

void AggiornaSottoHeapDIC (heapdic *pH, nodo s, peso **C, int i, vint Index)
{
  int figlioS, figlioD;
  int iMin;

  figlioS = 2*i;  
  figlioD = 2*i+1; 

  iMin = i;
  if ( (figlioS <= pH->last) && (  LeggeMTI(C,s,pH->V[figlioS]) <  LeggeMTI(C,s,pH->V[iMin]) ) ) iMin = figlioS;
  if ( (figlioD <= pH->last) && (  LeggeMTI(C,s,pH->V[figlioD]) <  LeggeMTI(C,s,pH->V[iMin]) ) ) iMin = figlioD;

  if (iMin != i)
  {
    ScambiaInt(&pH->V[iMin],&pH->V[i]);
	ScambiaInt(&Index[pH->V[i]], &Index[pH->V[iMin]]);
    AggiornaSottoHeapDIC(pH,s,C,iMin,Index);
  }
}


/* Legge l'elemento (i,j) o (j,i) di una matrice simmetrica di interi conservata in una triangolare inferiore. */
int LeggeMTI(int **M, int i, int j)
{
  if(i >= j)
	  return M[i][j];
  return M[j][i];
  
}

/* (Sovra)scrive sull'elemento (i,j) o (j,i) di una matrice simmetrica di interi conservata in una triangolare inferiore un intero dato. */
void ScriveMTI(int **M, int i, int j, int new)
{
  if(i >= j) M[i][j] = new;
  else M[j][i] = new;
}

void ScambiaInt (int *pa, int *pb)
{
  int temp;

  temp = *pa;
  *pa = *pb;
  *pb = temp;
}


int TrovaIndiceNodo(heapdic *pH, nodo n)
{
  int i;
  
  /* Versione Theta(n) con n il numero totale di nodi
     in realta' la ricerca avviene sempre entro pH->last,
     il che e' meno. */
  for(i = 1; i <= pH->size; i++)
  {
    if( pH->V[i] == n) return i;
  }	 
   
  return 0;
}

/* Alloca una matrice quadrata di nodi */
nodo** AllocaMQ(int n)
{
  int i;
  nodo **M = (nodo **) calloc(n+1, sizeof(nodo*));
  for(i = 1; i <= n; i++) 
    M[i] = (nodo *) calloc(n+1, sizeof(nodo)); 

  return M;
}

/* Alloca una matrice triangolare inferiore di pesi. */
peso** AllocaMTI(int n)
{
  int i;
  peso **M = (peso **) calloc(n+1, sizeof(peso*));
  for(i = 1; i <= n; i++) 
    M[i] = (peso *) calloc(i + 1, sizeof(peso)); /* Allocazione decrescente: matrice triangolare inferiore. */

  return M;
}

/* Inizializzazione matrice triangolare inferiore n x n con valore init */
void InizializzaMTI(int **C, int n, int init)
{
  int i, j;	
   
  for(i = 1; i <= n; i++) 
  {	
	for(j = 1; j <= i; j++)
      C[i][j] = init;
  }
}


/* Inizializzazione matrice quadrata n x n con valore init */
void InizializzaMQ(int **P, int n, int init)
{
  int i, j;
  for(i = 1; i <= n; i++) 
  {
	for(j = 1; j <= n; j++)
      P[i][j] = init;
  }
}


/* Questa funzione tenta di leggere il costo del cammino minimo dalla sorgente j
   al nodo i, nella matrice C. Se tale cammino minimo non e' ancora stato determinato, lo calcola
   con CalcolaCamminiMinimi e restituisce l'intero desiderato. */
peso LeggeCompletataC(grafo *pG, peso **C, nodo **P, int i, int j)
{
  if( LeggeMTI(C,i,j) == INFINITY )
    CalcolaCamminiMinimi(pG,C,P,j);

  return LeggeMTI(C,i,j);
}