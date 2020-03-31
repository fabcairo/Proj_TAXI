
#include <stdlib.h>
#include <stdio.h>
#include "grafoFS.h"
#include "heapdinE.h"
#include "dijkstra.h"
#include "zaino.h"

void InterpretaLineaComando(int argc, char *argv[], char *file1, char *file2, char *file3);

void CaricaGrafo(char *filedati, boolean Orientato);

void CaricaTaxi(char *filedati);

void InizializzaTaxi(void);

void CaricaChiamate(char *filedati);

void CreaHeapIndSTR (vint Index, heapdine *pH, int dim);

void AggiornaHeapIndSTR (vint Index, heapdine *pH, int dim, int i);

void StampaClientiOrdinati(void);

void CreaHeapIndINT (vint Index, heapdine *pH, int dim);

void AggiornaHeapIndINT (vint Index, heapdine *pH, int dim, int i);

boolean ConfrontaViaggi(evento *c1, evento *c2);

void StampaViaggiOrdinati(void);

void StampaCapovoltoVINT(vint v, int n);

void CollocaTaxi(void);

void SommaC(vint B, int col, int n);

int TrovaIndiceMax(vint B, int n);

int TrovaIndiceMaxIndiretto (vint V, vint D, int dim);

void SelectionSortIndiretto (vint V, vint D, int dim);

void StampaPosizTaxi();

int AssegnaTaxi(evento *e, int *hFine);

void StampaEvento(evento *e);

void DistruggeMatrice(int **M, int n);


/* Variabili globali del progetto. */
grafo R; /* Rete stradale. */
heapdine H; /* Heap degli eventi. */
int nv; /* Numero complessivo di taxi. */
int T; /* Orizzonte temporale del servizio. */
int aut; /* Autonomia in sec delle batterie. */
int percaut; /* Percentuale minima di batteria per non rientrare in sede. */
int dr; /* Durata della ricarica. */
vint inc; /* Vettore degli incassi parziali dei taxi. */
nodo *pos; /* Vettore delle posizioni dei taxi. */
vint batt; /* Vettore delle batterie residue dei taxi. */
int tfr1 = 0; /* Tempo di fine ricarica del taxi in testa alla coda. */
int ntc = 0; /* Numero di taxi in coda alla sede centrale. */
vboolean disp; /* Vettore delle disponibilita' dei taxi. */
peso **C; /* Matrice simmetrica dei cammini minimi tra nodi, memorizzata in una matrice triangolare inferiore. */
nodo **P; /* Matrice quadrata dei predecessori nei suddetti cammini. */
int crif = 0; /* Contatore dei rifiuti di chiamate. */
int cric = 0; /* Contatore delle avvenute ricariche. */
int tmov = 0; /* Contatore del tempo totale di movimento dei veicoli. */
int guad = 0; /* Contatore del guadagno totale del servizio. */

int main (int argc, char *argv[]) 
{
  /* Stringhe dei nomi dei files. */
  char file_rete[ROW_LENGTH];
  char file_veicoli[ROW_LENGTH];
  char file_chiamate[ROW_LENGTH];
  /* Strutture per la stima con il pb. dello zaino */
  KPdata I;
  KPsolution S;

  InterpretaLineaComando(argc,argv,file_rete,file_veicoli,file_chiamate);

  /* Caricamento della rete stradale. */
  CaricaGrafo(file_rete,FALSE);
	
	
  /* Caricamento dei taxi e delle informazioni sul servizio. */
  CaricaTaxi(file_veicoli);

  InizializzaTaxi();
  
  /* Caricamento delle chiamate. */
  CaricaChiamate(file_chiamate);
  
  /* Ordinamento alfabetico e stampa dei clienti. */
  StampaClientiOrdinati();
  
  
  /* Calcolo dei cammini minimi e stampa delle chiamate in ordine di durata. */
  C = AllocaMTI(R.n);
  P = AllocaMQ(R.n);
  InizializzaMTI(C,R.n,INFINITY);
  InizializzaMQ(P,R.n,0);
  
  AggiornaChiamateHeapDinE(&R,&H,C,P,R.n);
  
  StampaViaggiOrdinati();
  
  /* Collocazione iniziale dei taxi e stampa. */
  CollocaTaxi();
  
  StampaPosizTaxi();
  
  
  /* Stima knapsack */
  CaricaDatiKP(&I, &H, T, nv);
  CreaSoluzione(I.n,&S);
  GreedyKP(&I,&S);
  DistruggeDatiKP(&I); 
  
    
  /* INIZIO SIMULAZIONE */
  
  CreaHeapDinE(&H);
 
  puts("\nEventi:");
  while( !VuotoHeapDinE(&H) )
  {
    evento e = TestaHeapDinE(&H);
    evento enew;
    

    if( e.tempo > T )
      break;
    
    StampaEvento(&e);
    
	/* ++++++++++++++++++++++++++ */
    if( e.tipo == CHIAMATA )
    {
      int t;
	  int hFine;
      
      if( (t = AssegnaTaxi(&e,&hFine)) == 0 )
      {
        CancellaHeapDinE(&H);
      }
      else 
      {
        /* Innesca evento FINE_SERVIZIO. */     
        ScriveEvento(&enew, hFine, FINE_SERVIZIO, e.c, t); 
        SostituisceHeapDinE(&enew, &H);
      }  
    }
	
	/* ++++++++++++++++++++++++++ */
    else if ( e.tipo == FINE_SERVIZIO )
    {
      if( batt[e.taxi] < percaut )
      {
        int dRientro = LeggeCompletataC(&R, C, P, e.c->nodoDest, 1);
        int hRientro = e.tempo + dRientro;
        batt[e.taxi] -= dRientro;
        pos[e.taxi] = 1;
        /* disp[e.taxi] rimane FALSE */
        tmov += dRientro;
    
    
        ScriveEvento(&enew, hRientro, RIENTRO_SEDE, NO_CALL, e.taxi);
        DistruggeChiamata(&H.V[1]); /* Prima di sostituire il nuovo evento, dealloco la chiamata presente il quello vecchio. */
        SostituisceHeapDinE(&enew, &H);
      }
      else
      {
        disp[e.taxi] = TRUE;
        CancellaHeapDinE(&H);
      }
    }
	
	/* ++++++++++++++++++++++++++ */
    else if ( e.tipo == RIENTRO_SEDE )
    {
      int hFineRic;
	  if( ntc == 0 )
	  {
	    hFineRic = e.tempo + dr;
		tfr1 = hFineRic;
	  }
	  else
	  {
		hFineRic = ntc*dr + tfr1;
	  }
	  
	  ntc++;
	  
	  ScriveEvento(&enew, hFineRic, FINE_RICARICA, NO_CALL, e.taxi);
	  SostituisceHeapDinE(&enew, &H);
    }
	
	/* ++++++++++++++++++++++++++ */
    else if ( e.tipo == FINE_RICARICA )
    {
      batt[e.taxi] = aut;
	  disp[e.taxi] = TRUE;
	  cric++;
	  
	  /* Aggiorna i contatori della coda. */
	  ntc--;
	  tfr1 = e.tempo + dr; /* Se ntc == 0, il valore di tfr1 risulta fittizio perche' non ci sono
	  piu' taxi in coda, tuttavia e' un valore inutile dato che il caso ntc == 0 e' opportunamente gestito per
	  eventi di tipo RIENTRO_SEDE (vedi blocco else if precedente). */
	  
	  CancellaHeapDinE(&H);
    }
  
  }
  
  
  /* Finito il tempo, svuota lo heap senza innescare altri eventi. */
  if( !VuotoHeapDinE(&H) )
  {
    puts("\nEventi in completamento oltre l'ora finale del servizio complessivo:");
	while( !VuotoHeapDinE(&H) )
	{
      evento e = TestaHeapDinE(&H);
	  StampaEvento(&e);
	  
	  if( e.tipo == CHIAMATA )
	    crif++;
	  else if( e.tipo == FINE_RICARICA )
	    cric++;
	
	  CancellaHeapDinE(&H);
	}
  }
  
  printf("\nRifiuti: %d\nRicariche: %d\nTempo totale: %d\nGuadagno: %d\nUB: %ld\n", crif, cric, tmov, guad, S.f);
  
  /* Distruzione delle strutture e vettori del progetto. */
  DistruggeSoluzioneKP(&S);
  DistruggeMatrice(C, R.n);
  DistruggeMatrice(P, R.n);
  distrugge_grafo(&R);
  DistruggeHeapDinE(&H);  
  free(inc);
  free(pos);
  free(batt);
  free(disp);
  
    
  return EXIT_SUCCESS;
}




void InterpretaLineaComando(int argc, char *argv[], char *file1, char *file2, char *file3) 
{
  if (argc != 4)
  {
    fprintf(stderr,"Errore nella linea di comando!\n");
    fprintf(stderr,"Formato richiesto: %s [file_rete] [file_veicoli] [file_chiamate]\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  strcpy(file1,argv[1]);
  strcpy(file2,argv[2]);
  strcpy(file3,argv[3]);
}



void CaricaGrafo (char *filedati, boolean Orientato)
{
  FILE *fp;
  int i, j, k, n, m;

  /* Apre il file */
  fp = fopen(filedati,"r");
  if (fp == NULL)
  {
    fprintf(stderr,"File %s could not be opened!\n",filedati);
    exit(EXIT_FAILURE);
  }

  /* Conta i nodi del grafo */
  if( fscanf(fp,"%d %d",&n,&m) != 2 )
  {
     fprintf(stderr,"Errore di formato!\n");
     exit(EXIT_FAILURE);
  }

  /* Crea il grafo, assumendo che gli indici dei nodi vadano da 1 a n */
  crea_grafo(n,&R);

  /* Aggiunge gli archi del grafo */
  while (fscanf(fp,"%d %d %d",&i,&j,&k) == 3)
  {
    ins_arco(i,j,k,&R);
    if (!Orientato) ins_arco(j,i,k,&R);
  }

  if( 2*m != R.m ) 
  {
     fprintf(stderr,"Incoerenza nei dati.\n", m, R.m);
     exit(EXIT_FAILURE);
  }

  /* Chiude il file */
  fclose(fp);
}


void CaricaTaxi(char *filedati)
{
  FILE *fp;

  /* Apre il file */
  fp = fopen(filedati,"r");
  if (fp == NULL)
  {
    fprintf(stderr,"File %s could not be opened!\n",filedati);
    exit(EXIT_FAILURE);
  }

  /* Legge i dati */
  if( fscanf(fp,"%d %d %d %d",&nv,&T,&aut,&dr) != 4 )
  {
     fprintf(stderr,"Errore di formato!\n");
     exit(EXIT_FAILURE);
  }
  
  percaut = aut / 5 ;

  /* Chiude il file */
  fclose(fp);
}



void InizializzaTaxi(void)
{
   int i;
   inc = (int *) calloc(nv+1,sizeof(int));
   pos = (nodo *) calloc(nv+1,sizeof(nodo));
   batt = (int *) calloc(nv+1,sizeof(int));
   disp = (boolean *) calloc(nv+1,sizeof(boolean));

   if( inc == NULL || pos == NULL || batt == NULL || disp == NULL )
   {
      fprintf(stderr,"Errore di allocazione!\n");
      exit(EXIT_FAILURE);
   }

   for(i = 1; i <= nv; i++ ) 
   {
      batt[i] = aut;
      disp[i] = TRUE;
   }
}


void CaricaChiamate(char *filedati)
{
  FILE *fp;
  int size;
  int tempo, nodoOrig, nodoDest, hmin, hmax, premio;
  char cliente[ROW_LENGTH];
  int i = 0;

  fp = fopen(filedati,"r");
  if (fp == NULL)
  {
    fprintf(stderr,"File %s could not be opened!\n",filedati);
    exit(EXIT_FAILURE);
  }

  if( fscanf(fp,"%d",&size) != 1 )
  {
    fprintf(stderr,"Errore di formato!\n");
    exit(EXIT_FAILURE);
  }
  
  CreaVettoreHeapDinE(&H,size);
  
  while( fscanf(fp,"%d %s %d %d %d %d %d",&tempo,cliente,&nodoOrig,&nodoDest,&hmin,&hmax,&premio) == 7 )
  {
    i++;
	AggiungeChiamataHeapDinE(&H,&i,&tempo,cliente,&nodoOrig,&nodoDest,&hmin,&hmax,&premio);
  }
  
  if( i != size ) fprintf(stderr, "Numero di chiamate incoerente.\n");
  
}


void CreaHeapIndSTR (vint Index, heapdine *pH, int dim)
{
  int i;

  for (i = dim/2; i >= 1; i--)
    AggiornaHeapIndSTR(Index,pH,dim,i);
}


void AggiornaHeapIndSTR (vint Index, heapdine *pH, int dim, int i)
{
  int s, d;
  int iMin;

  s = 2*i;
  d = 2*i+1;

  iMin = i;
  if ( (s <= dim) && ( strcmp(pH->V[Index[iMin]].c->cliente, pH->V[Index[s]].c->cliente) > 0? TRUE : FALSE ) ) iMin = s;
  if ( (d <= dim) && ( strcmp(pH->V[Index[iMin]].c->cliente, pH->V[Index[d]].c->cliente) > 0? TRUE : FALSE ) ) iMin = d;

  if (iMin != i)
  {
    ScambiaInt(&Index[iMin],&Index[i]);
    AggiornaHeapIndSTR(Index,pH,dim,iMin);
  }
}


void StampaClientiOrdinati(void)
{
  int dim = H.last;
  int *Index = (int *) calloc(dim+1,sizeof(int)); 
  int i;
  
  for(i = 1; i<=dim; i++)
    Index[i]=i;

  puts("Clienti:");
  CreaHeapIndSTR(Index,&H,dim);
  for (i = dim; i > 1; i--)
  {
    printf("%s\n", H.V[Index[1]].c->cliente);
	ScambiaInt(&Index[1],&Index[i]); 
    AggiornaHeapIndSTR(Index,&H,i-1,1);
  }
  printf("%s\n", H.V[Index[1]].c->cliente );
  
  free(Index);
}


void CreaHeapIndINT (vint Index, heapdine *pH, int dim)
{
  int i;

  for (i = dim/2; i >= 1; i--)
    AggiornaHeapIndINT(Index,pH,dim,i);
}


void AggiornaHeapIndINT (vint Index, heapdine *pH, int dim, int i)
{
  int s, d;
  int iMax;

  s = 2*i;
  d = 2*i+1;

  iMax = i;
  if ( (s <= dim) && ConfrontaViaggi(&pH->V[Index[s]], &pH->V[Index[iMax]]) ) iMax = s;
  if ( (d <= dim) && ConfrontaViaggi(&pH->V[Index[d]], &pH->V[Index[iMax]]) ) iMax = d;

  if (iMax != i)
  {
    ScambiaInt(&Index[iMax],&Index[i]);
    AggiornaHeapIndINT(Index,pH,dim,iMax);
  }
}

/* Restituisce true sse la chiamata 1 ha una durata di viaggio maggiore
   alla durata di viaggio della chiamata 2, oppure se ha la stessa durata
   di viaggio ma ora di chiamata minore. */
boolean ConfrontaViaggi(evento *c1, evento *c2)
{
  if(c1->c->durata > c2->c->durata) return TRUE;
  if(c1->c->durata < c2->c->durata) return FALSE;
  
  if(c1->tempo < c2->tempo) return TRUE;
  return FALSE;
}


void StampaViaggiOrdinati(void)
{
  int dim = H.last;
  int *Index = (int *) calloc(dim+1,sizeof(int)); 
  int i;
  
  for(i = 1; i <= dim; i++)
    Index[i] = i;

  puts("\nViaggi:");
  CreaHeapIndINT(Index,&H,dim);
  for (i = dim; i >= 1; i--)
  {
    printf("%d %s %d ", H.V[Index[1]].tempo, H.V[Index[1]].c->cliente, H.V[Index[1]].c->durata);
    StampaCapovoltoVINT(H.V[Index[1]].c->nodiVis, H.V[Index[1]].c->nNodi);
    puts(""); /* va a capo */
    
	ScambiaInt(&Index[1],&Index[i]); 
    AggiornaHeapIndINT(Index,&H,i-1,1);
  }
  
  free(Index);
}


void StampaCapovoltoVINT(vint v, int n)
{
  int i;
  putchar('(');
  for(i = n; i > 1; i--)
    printf("%d, ", v[i]);
    
  printf("%d)", v[i]);
}


void CollocaTaxi(void)
{
  int i, n = R.n;
  nodo lontano;
  peso *B = (peso *) calloc(n+1,sizeof(peso)); /* Componenti azzerate. */
  
  /* Collocazione del primo taxi in sede. */
  pos[1] = 1;
  SommaC(B,1,n);
  
  /* Ciclo: colloca ogni taxi successivo al primo. */
  for(i = 2; i <= nv; i++)
  {
    lontano = TrovaIndiceMax(B,n);
    pos[i] = lontano;
    SommaC(B,lontano,n);
	B[lontano] = -INFINITY; /* Riga aggiunta al fine di non mettere due taxi nella stessa posizione. */
  }  
  
  free(B);
}


/* Somma ad un vettore di interi B la colonna col di C. */
void SommaC(vint B, int col, int n)
{
  int i;
  for(i = 1; i <= n; i++)
  {
    B[i] += LeggeCompletataC(&R,C,P,i,col);
  }
}


/* Trova l'indice dell'elemento massimo secondo il consueto ordinamento dei naturali
   in un vettore V di lunghezza n. */
int TrovaIndiceMax (vint V, int n)
{
  int i, iMax;

  /* Predilige indice minimo a parita' di massimo. */
  iMax = 1;
  for (i = 2; i <= n; i++)
    if (V[i] > V[iMax]) iMax = i;

  return iMax;
}

/* Trova l'indice dell'elemento massimo di un vettore V secondo l'ordinamento indotto dalle distanze
   minime da un nodo. */
int TrovaIndiceMaxIndiretto (vint V, vint D, int dim)
{
  int i, iMax;

  /* Predilige indice minimo a parita' di massimo. */
  iMax = 1;
  for (i = 2; i <= dim; i++)
    if ( D[V[i]] > D[V[iMax]] ) iMax = i;

  return iMax;
}


void SelectionSortIndiretto (vint V, vint D, int dim)
{
  int j, i;

  for (j = dim; j >= 1; j--)
  {
    i = TrovaIndiceMaxIndiretto(V,D,j);
    ScambiaInt(&V[i],&V[j]);
  }
}

void StampaPosizTaxi(void)
{
  int i;
  
  puts("\nPosizioni:");
  for(i = 1; i <= nv; i++)
    printf("%d ", pos[i]);
  puts("");
}



/* Questa procedura trova un taxi disponibile a servire l'evento 'e' che dev'essere di tipo chiamata. 
Se tale taxi t esiste, aggiorna anche le relative componenti t dei vettori inc, pos, batt e disp. 
Inoltre, aggiorna i contatori globali. Infine, scrive su *hFine l'orario di fine del servizio effettuato
dal taxi appena assegnato. */
int AssegnaTaxi(evento *e, int *hFine)
{
  int i, dim = 0, iPuntuale = 0;
  int oraAttuale = e->tempo;
  int taxiAssegnato = 0;
  
  vint vtaxi = (int *) calloc(nv+1, sizeof(int)); /* vettore di taxi disponibili*/
  vint tdao = (int *) calloc(nv+1, sizeof(int)); /* Tempi di arrivo all'origine del viaggio. */
  
  /* Se il cliente vuole viaggiare in un arco temporale inferiore alla minima
     durata del viaggio da lui richiesto, cio' non sara' possibile, ovviamente. */
  if( e->c->hmax - e->c->hmin < e->c->durata  )
  {
    crif++;
    free(vtaxi);
	free(tdao);
    return taxiAssegnato;
  }
  
  
  /* Creazione del vettore con i taxi disponibili e delle relative distanze
  dal punto di origine del viaggio. */
  for(i = 1; i <= nv; i++)
  {
    if( disp[i] == TRUE )
    {
      dim++;
      vtaxi[dim] = i;
    }
	tdao[i] = LeggeCompletataC(&R,C,P,pos[i],e->c->nodoOrig);
  }
  
  /* Se non ci sono taxi disponibili, restituisce 0. */
  if( dim == 0 )
  {
    crif++;
    free(vtaxi);
	free(tdao);
    return taxiAssegnato;
  }
  
  
  /* Qui inizia il lavoro di assegnazione taxi.
     Ordino i taxi per tempo crescente di arrivo al nodoOrig
     con un selection sort. */
  SelectionSortIndiretto(vtaxi, tdao, dim);
  
  /* Ripartisco il vettore in due: i taxi che riescono ad arrivare
  prima di hmin al nodoOrig avranno indice <= iPuntuale, gli altri > */
  while( (iPuntuale < dim) && (oraAttuale + tdao[vtaxi[iPuntuale+1]] <= e->c->hmin) )
    iPuntuale++;
  
  /* Assegno il 'primo' taxi candidato che puo' effettivamente
  servire il cliente secondo i criteri rimanenti.
  'Primo' significa il primo in ordine decrescente di arrivo se 
  prima di iPuntuale e in ordine crescente se dopo iPuntuale.  */
  for(i = iPuntuale; i >= 1; i--)
  {
    if ( tdao[vtaxi[i]] + e->c->durata + LeggeCompletataC(&R,C,P,e->c->nodoDest,1)  <=  batt[vtaxi[i]] )
    {
	  /* vtaxi[i] e' il taxi cercato ! */
	  taxiAssegnato = vtaxi[i];
	  
	  tmov += tdao[taxiAssegnato] + e->c->durata;
	  guad += ( inc[taxiAssegnato] = e->c->durata + e->c->premio ); /* Questo taxi e' puntuale. */
	  batt[taxiAssegnato] -= tdao[taxiAssegnato] + e->c->durata;
	  disp[taxiAssegnato] = FALSE;
	  pos[taxiAssegnato] = e->c->nodoDest;
	  *hFine = e->c->hmin + e->c->durata; /* Il taxi aspetta l'hmin per partire */
	  
	  free(vtaxi);
	  free(tdao);
	  return taxiAssegnato;
	}
  }
  
  /* Se il controllo esce dal precedente for, nessun taxi puntuale e' stato assegnato.
  Assegno allora, se possibile, un taxi non puntuale. */
  for(i = iPuntuale+1; i <= dim; i++)
  {
    /* Qui bisogna anche controllare che il viaggio avvenga entro hmax. */
	if( oraAttuale + tdao[vtaxi[i]] + e->c->durata <= e->c->hmax )
	{	
	  if ( tdao[vtaxi[i]] + e->c->durata + LeggeCompletataC(&R,C,P,e->c->nodoDest,1)  <=  batt[vtaxi[i]] )
      {
	    /* vtaxi[i] e' il taxi cercato ! */
	      taxiAssegnato = vtaxi[i];
	  
	    tmov += tdao[taxiAssegnato] + e->c->durata;
	    guad += ( inc[taxiAssegnato] = e->c->durata );
	    batt[taxiAssegnato] -= tdao[taxiAssegnato] + e->c->durata;
	    disp[taxiAssegnato] = FALSE;
	    pos[taxiAssegnato] = e->c->nodoDest;
		*hFine = oraAttuale + tdao[taxiAssegnato] + e->c->durata;
	  
	    free(vtaxi);
	    free(tdao);
	    return taxiAssegnato;
	  }
	}
  }
  
  /* Se i due cicli precedenti non hanno terminato la procedura, significa che nessun taxi puo' servire
  la chiamata in questione. */
  
  crif++;
  free(vtaxi);
  free(tdao); 
  return taxiAssegnato;
}


void StampaEvento(evento *e)
{
  printf("%d ", e->tempo);
  
  if(e->tipo == CHIAMATA)
    printf("%s %d %s\n", "CHIAMATA", 0, e->c->cliente);
  
  if(e->tipo == FINE_SERVIZIO)
    printf("%s %d %s\n", "FINE_SERVIZIO", e->taxi, e->c->cliente);
    
  if(e->tipo == RIENTRO_SEDE)
    printf("%s %d\n", "RIENTRO_SEDE", e->taxi);
    
  if(e->tipo == FINE_RICARICA)
    printf("%s %d\n", "FINE_RICARICA", e->taxi);
}

/* Distrugge una matrice n x n di interi (anche se triangolare). */
void DistruggeMatrice(int **M, int n)
{
  int i;
  for(i = 1; i <= n; i++)
  {
    free(M[i]);  
  }
  free(M);
}
