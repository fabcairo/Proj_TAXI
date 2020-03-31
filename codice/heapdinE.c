#include "heapdinE.h"

void CreaVettoreHeapDinE (heapdine *pH, int size)
{
  pH->V = (evento *) calloc(size+1,sizeof(evento));
  if (pH->V == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della pila!\n");
    exit(EXIT_FAILURE);
  }

  pH->size = size;
  pH->last = 0;
} 
/* Oss.: non e' allocata la memoria per le chiamate perche' la loro allocazione e' gestita
   a parte dalla funzione CreaChiamata e chiamanti.  */


void DistruggeHeapDinE(heapdine *pH)
{
  /* Delloco prima tutte le chiamate rimaste nel vettore (saranno tutte le quelle degli eventi CHIAMATA rifiutati e quelle dei FINE_SERVIZIO non sostituiti da RIENTRO_SEDE. */
  int i;
  for(i = 1; i <= pH->size; i++) /* Ogni posizione del vettore dello heapdine e' almeno occupata da qualche elemento (quindi non punta a zone di memoria incognite) */
    DistruggeChiamata(&pH->V[i]);
  
  free(pH->V);
  pH->V = NULL;
  pH->size = 0;
  pH->last = 0;
}


chiamata* CreaChiamata(char *cliente, int nodoOrig, int nodoDest, int hmin, int hmax, int premio, int durata, nodo *nodiVis, int nNodi)
{
  chiamata *c = (chiamata *) malloc(sizeof(chiamata));
  strcpy(c->cliente , cliente);
  c->nodoOrig = nodoOrig;
  c->nodoDest = nodoDest;
  c->hmin = hmin;
  c->hmax = hmax;
  c->premio = premio;
  c->durata = durata;
  c->nodiVis = nodiVis;
  c->nNodi = nNodi;
  
  return c; 
}

void DistruggeChiamata(evento *e)
{
  if( e->c != NO_CALL )
  {
     if( e->c->nodiVis != NULL )
       free(e->c->nodiVis);
     
     free(e->c);
	 e->c = NO_CALL;
  }
}

/* Questa procedura puo' essere eseguita solo all'inizio della simulazione. */
void AggiungeChiamataHeapDinE(heapdine *pH, int *nChiam, int *tempo, char *cliente, int *nodoOrig, int *nodoDest, int *hmin, int *hmax, int *premio)
{
  int last = ++(pH->last);
  chiamata *c;
  
  c = CreaChiamata(cliente, *nodoOrig, *nodoDest, *hmin, *hmax, *premio, 0, NULL, 0);
  
  ScriveEvento(&pH->V[last], *tempo, CHIAMATA, c, - *nChiam);
   /* nChiam reso negativo perche' e' un valore fittizio del taxi, utile per stabilire la priorita' di eventi (v. ConfrontaEventi) */
}

/* Questa procedura scrive i campi durata e nodiVis di tutte le chiamate
   in uno heapine, grazie alle matrici dei cammini minimi C e P. */
void AggiornaChiamateHeapDinE(grafo *pG, heapdine *pH, peso **C, nodo **P, int n)
{
  int i;
  chiamata *c;
  nodo pred;
  
  for(i = 1; i <= pH->last; i++)
  {
    c = pH->V[i].c;
    
    if( c != NO_CALL )
    {
      /* Scrive la durata (minima) del viaggio i */

      c->durata = LeggeCompletataC(pG, C, P, c->nodoDest, c->nodoOrig );
      
      /* Scrive il vettore nodiVis dei nodi visitati nel cammino minimo da nodoOrig s nodoDest */
      /* Scrive i nodi visitati risalendo a ritroso nel cammino da
          nodoOrig a nodoDest. L'ordine originale di visita si otterra'
          scandendo il vettore da nNodi a 1 */
      
      c->nodiVis = (nodo *) calloc(n+1,sizeof(nodo)); 
      c->nodiVis[1] = c->nodoDest;
      c->nNodi = 1;
  
	  
      pred = P[c->nodoDest][c->nodoOrig];
      while( pred != c->nodoOrig  )
      {
        c->nNodi++;
        c->nodiVis[c->nNodi] = pred;
		pred = P[pred][c->nodoOrig];
      }  
	  
	  c->nodiVis[++(c->nNodi)] = c->nodoOrig;
    }
  }
}


boolean VuotoHeapDinE (heapdine *pH)
{
  return (pH->last == 0);
}


evento TestaHeapDinE (heapdine *pH)
{
  return pH->V[1];
}


/* Cancellazione e aggiornamento. */
void CancellaHeapDinE (heapdine *pH)
{
  DistruggeChiamata(&(pH->V[1])); /* Distrugge, se c'e', una chiamata */
  ScambiaE( &(pH->V[1]) , &(pH->V[pH->last]) );
  pH->last--;
  AggiornaHeapDinE(pH,1);
}

/* Sostituzione e aggiornamento. */
void SostituisceHeapDinE (evento *e, heapdine *pH)
{
  pH->V[1] = *e;
  AggiornaHeapDinE(pH,1);
}


void CreaHeapDinE (heapdine *pH)
{
  int i;

  for (i = pH->last/2; i >= 1; i--)
    AggiornaHeapDinE(pH,i);
}

void AggiornaHeapDinE (heapdine *pH, int i)
{
  int s, d;
  int iMin;

  s = 2*i;  
  d = 2*i+1; 

  iMin = i;
  if ( (s <= pH->last) && ( ConfrontaEventi(&pH->V[s],&pH->V[iMin]) ) ) iMin = s;
  if ( (d <= pH->last) && ( ConfrontaEventi(&pH->V[d],&pH->V[iMin]) ) ) iMin = d;

  if (iMin != i)
  {
    ScambiaE(&pH->V[iMin],&pH->V[i]);
    AggiornaHeapDinE(pH,iMin);
  }
}


void ScambiaE (evento *pa, evento *pb)
{
  evento temp;

  temp = *pa; /*copiati tutti i campi. Duplicati i campi puntatori. */
  *pa = *pb;
  *pb = temp;
}


/* Restituisce TRUE sse l'evento e1 ha priorita' sull'evento e2, secondo le
   gerarchie d'ordine assegnate nel progetto. */
boolean ConfrontaEventi(evento *e1, evento *e2)
{
  if(e1->tempo < e2->tempo) return TRUE;
  if(e1->tempo > e2->tempo) return FALSE;
  /* se il controllo arriva qui e1->tempo == e2->tempo */
  if(e1->tipo < e2->tipo) return TRUE;
  if(e1->tipo > e2->tipo) return FALSE;
  /* se il controllo arriva qui e1->tempo == e2->tempo e e1->tipo == e2->tipo */
  return ( abs(e1->taxi) < abs(e2->taxi) );  /* Le chiamate vengono marcate con indici negativi
  decrescenti in ordine di lettura da file. */
}


void ScriveEvento(evento *e, int tempo, short tipo, chiamata *c, int taxi)
{
  e->tempo = tempo;
  e->tipo = tipo;
  e->c = c;
  e->taxi = taxi;
}

