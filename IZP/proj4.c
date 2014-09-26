/*
 * Súbor    :   proj4.c
 * Dátum    :   11.12.2010
 * Autor    :   Michal Lukáè, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Projekt è.4. Èeské radenie záznamov.
 * Popis    :   Tento projekt je projektom do predmetu základy programovania.
 * P.riadkov:   741
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMAXCHAR 1000     /* Kon¹tanta pre max. poèet znakov v riadku.  */
#define CCVAL 200         /* Kon¹tanta pre max. poèet znakov v stlpci.  */
const int CPVAL = 4;      /* Kon¹tanta pre poèet povinných parametrov.  */
const char CCH = '%';     /* Kon¹tanta pre ch.                          */
const char CCH_2 = '&';   /* Kon¹tanta pre CH.                          */

// tabulka èeských znakov podµa hodnoty
const unsigned char CZTABLE[256] =
{
  [(unsigned char)' '] = 0,  [(unsigned char)'Q'] = 19,
  [(unsigned char)'A'] = 1,  [(unsigned char)'q'] = 19,
  [(unsigned char)'Á'] = 1,  [(unsigned char)'R'] = 20,
  [(unsigned char)'a'] = 1,  [(unsigned char)'r'] = 20,
  [(unsigned char)'á'] = 1,  [(unsigned char)'Ø'] = 21,
  [(unsigned char)'B'] = 2,  [(unsigned char)'ø'] = 21,
  [(unsigned char)'b'] = 2,  [(unsigned char)'S'] = 22,
  [(unsigned char)'C'] = 3,  [(unsigned char)'s'] = 22,
  [(unsigned char)'c'] = 3,  [(unsigned char)'©'] = 23,
  [(unsigned char)'È'] = 4,  [(unsigned char)'¹'] = 23,
  [(unsigned char)'è'] = 4,  [(unsigned char)'T'] = 24,
  [(unsigned char)'D'] = 5,  [(unsigned char)'«'] = 24,
  [(unsigned char)'Ï'] = 5,  [(unsigned char)'t'] = 24,
  [(unsigned char)'d'] = 5,  [(unsigned char)'»'] = 24,
  [(unsigned char)'ï'] = 5,  [(unsigned char)'U'] = 25,
  [(unsigned char)'E'] = 6,  [(unsigned char)'Ú'] = 25,
  [(unsigned char)'É'] = 6,  [(unsigned char)'Ù'] = 25,
  [(unsigned char)'Ì'] = 6,  [(unsigned char)'u'] = 25,
  [(unsigned char)'e'] = 6,  [(unsigned char)'ú'] = 25,
  [(unsigned char)'é'] = 6,  [(unsigned char)'ù'] = 25,
  [(unsigned char)'ì'] = 6,  [(unsigned char)'V'] = 26,
  [(unsigned char)'F'] = 7,  [(unsigned char)'v'] = 26,
  [(unsigned char)'f'] = 7,  [(unsigned char)'W'] = 27,
  [(unsigned char)'G'] = 8,  [(unsigned char)'w'] = 27,
  [(unsigned char)'g'] = 8,  [(unsigned char)'X'] = 28,
  [(unsigned char)'H'] = 9,  [(unsigned char)'x'] = 28,
  [(unsigned char)'h'] = 9,  [(unsigned char)'Y'] = 29,
  [(unsigned char)'&'] = 10, [(unsigned char)'y'] = 29,
  [(unsigned char)'%'] = 10, [(unsigned char)'Ý'] = 29,
  [(unsigned char)'I'] = 11, [(unsigned char)'ý'] = 29,
  [(unsigned char)'i'] = 11, [(unsigned char)'Z'] = 30,
  [(unsigned char)'Í'] = 11, [(unsigned char)'z'] = 30,
  [(unsigned char)'í'] = 11, [(unsigned char)'®'] = 31,
  [(unsigned char)'J'] = 12, [(unsigned char)'¾'] = 31,
  [(unsigned char)'j'] = 12, [(unsigned char)'0'] = 32,
  [(unsigned char)'K'] = 13, [(unsigned char)'1'] = 33,
  [(unsigned char)'k'] = 13, [(unsigned char)'2'] = 34,
  [(unsigned char)'L'] = 14, [(unsigned char)'3'] = 35,
  [(unsigned char)'l'] = 14, [(unsigned char)'4'] = 36,
  [(unsigned char)'M'] = 15, [(unsigned char)'5'] = 37,
  [(unsigned char)'m'] = 15, [(unsigned char)'6'] = 38,
  [(unsigned char)'N'] = 16, [(unsigned char)'7'] = 39,
  [(unsigned char)'Ò'] = 16, [(unsigned char)'8'] = 40,
  [(unsigned char)'n'] = 16, [(unsigned char)'9'] = 41,
  [(unsigned char)'ò'] = 16, [(unsigned char)'.'] = 42,
  [(unsigned char)'O'] = 17, [(unsigned char)','] = 43,
  [(unsigned char)'Ó'] = 17, [(unsigned char)';'] = 44,
  [(unsigned char)'o'] = 18, [(unsigned char)'?'] = 45,
  [(unsigned char)'ó'] = 18, [(unsigned char)'!'] = 46,
  [(unsigned char)'P'] = 18, [(unsigned char)':'] = 47,
  [(unsigned char)'p'] = 18, [(unsigned char)'"'] = 48,
                             [(unsigned char)'-'] = 49,
};

// druhá tabulka vyjadrujúca váhy znakov odli¹ne pre sekundárne porovnanie
const unsigned char CZTABLE2[256] =
{
  [(unsigned char)' '] = 0,  [(unsigned char)'Q'] = 26,
  [(unsigned char)'A'] = 1,  [(unsigned char)'q'] = 26,
  [(unsigned char)'Á'] = 2,  [(unsigned char)'R'] = 27,
  [(unsigned char)'a'] = 1,  [(unsigned char)'r'] = 27,
  [(unsigned char)'á'] = 2,  [(unsigned char)'Ø'] = 28,
  [(unsigned char)'B'] = 3,  [(unsigned char)'ø'] = 28,
  [(unsigned char)'b'] = 3,  [(unsigned char)'S'] = 29,
  [(unsigned char)'C'] = 4,  [(unsigned char)'s'] = 29,
  [(unsigned char)'c'] = 4,  [(unsigned char)'©'] = 30,
  [(unsigned char)'È'] = 5,  [(unsigned char)'¹'] = 30,
  [(unsigned char)'è'] = 5,  [(unsigned char)'T'] = 31,
  [(unsigned char)'D'] = 6,  [(unsigned char)'«'] = 32,
  [(unsigned char)'Ï'] = 7,  [(unsigned char)'t'] = 31,
  [(unsigned char)'d'] = 6,  [(unsigned char)'»'] = 32,
  [(unsigned char)'ï'] = 7,  [(unsigned char)'U'] = 33,
  [(unsigned char)'E'] = 8,  [(unsigned char)'Ú'] = 34,
  [(unsigned char)'É'] = 9,  [(unsigned char)'Ù'] = 35,
  [(unsigned char)'Ì'] = 10, [(unsigned char)'u'] = 33,
  [(unsigned char)'e'] = 8,  [(unsigned char)'ú'] = 34,
  [(unsigned char)'é'] = 9,  [(unsigned char)'ù'] = 35,
  [(unsigned char)'ì'] = 10, [(unsigned char)'V'] = 36,
  [(unsigned char)'F'] = 11, [(unsigned char)'v'] = 36,
  [(unsigned char)'f'] = 11, [(unsigned char)'W'] = 37,
  [(unsigned char)'G'] = 12, [(unsigned char)'w'] = 37,
  [(unsigned char)'g'] = 12, [(unsigned char)'X'] = 38,
  [(unsigned char)'H'] = 13, [(unsigned char)'x'] = 38,
  [(unsigned char)'h'] = 13, [(unsigned char)'Y'] = 39,
  [(unsigned char)'&'] = 14, [(unsigned char)'y'] = 39,
  [(unsigned char)'%'] = 14, [(unsigned char)'Ý'] = 40,
  [(unsigned char)'I'] = 15, [(unsigned char)'ý'] = 40,
  [(unsigned char)'i'] = 15, [(unsigned char)'Z'] = 41,
  [(unsigned char)'Í'] = 16, [(unsigned char)'z'] = 41,
  [(unsigned char)'í'] = 16, [(unsigned char)'®'] = 42,
  [(unsigned char)'J'] = 17, [(unsigned char)'¾'] = 42,
  [(unsigned char)'j'] = 17, [(unsigned char)'0'] = 43,
  [(unsigned char)'K'] = 18, [(unsigned char)'1'] = 44,
  [(unsigned char)'k'] = 18, [(unsigned char)'2'] = 45,
  [(unsigned char)'L'] = 19, [(unsigned char)'3'] = 46,
  [(unsigned char)'l'] = 19, [(unsigned char)'4'] = 47,
  [(unsigned char)'M'] = 20, [(unsigned char)'5'] = 48,
  [(unsigned char)'m'] = 20, [(unsigned char)'6'] = 49,
  [(unsigned char)'N'] = 21, [(unsigned char)'7'] = 50,
  [(unsigned char)'Ò'] = 22, [(unsigned char)'8'] = 51,
  [(unsigned char)'n'] = 21, [(unsigned char)'9'] = 52,
  [(unsigned char)'ò'] = 22, [(unsigned char)'.'] = 53,
  [(unsigned char)'O'] = 23, [(unsigned char)','] = 54,
  [(unsigned char)'Ó'] = 24, [(unsigned char)';'] = 55,
  [(unsigned char)'o'] = 23, [(unsigned char)'?'] = 56,
  [(unsigned char)'ó'] = 24, [(unsigned char)'!'] = 57,
  [(unsigned char)'P'] = 25, [(unsigned char)':'] = 58,
  [(unsigned char)'p'] = 25, [(unsigned char)'"'] = 59,
                             [(unsigned char)'-'] = 60,
};

// Kódy chýb programu
enum tecodes
{
  EOK = 0,                      /* Bez chyby.                        */
  ECLWRONG = 1,                 /* Chybný príkazový riadok.          */
  EFILEO = 2,                   /* Chyba pri otváraní súboru.        */
  EMALLOC = 3,                  /* Chyba pri alokovaní pamäte.       */
  EFILEL = 4,                   /* Chyba pri naèítaní zo súboru.     */
};

// Chybové hlásenia odpovedajúce chybovým kódom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nChyba pri otvarani-zatvarani suboru!\n",             /* EFILEO   */
  "\nChyba pri alokovani pamate!\n",                      /* EMALLOC  */
  "\nChyba pri citani/zapisu suboru!\n",                  /* EFILEL   */
};

// ©truktúra obsahujúca hodnoty parametrov z príkazovej riadky
typedef struct params
{
  int ecode;                    /* Chybový kód programu, odpovedá výètu tecodes.    */
  int select;                   /* Stav pre parameter before = 1; after = 2.        */
  char selectcolumn[CCVAL];     /* Stav pre vybranie riadkov.                       */
  char printcolumn[CCVAL];      /* Ktorý ståpec sa má tlaèi».                       */
  int sort;                     /* Stav pre porovnávanie vzostupne.                 */
  char filesrc[CCVAL];          /* Názov k zdrojovému súboru.                       */
  char filedest[CCVAL];         /* Názov k cieµovému súboru.                        */
  char selecttext[CCVAL];       /* Stav podµa akého re»azca sa bude vybera».        */
} TParams;

// ©truktúra pre polo¾ku
typedef struct item TItem;
struct item
{
  char printitem[CCVAL];      /* Re»azec pre ståpec print.   */
  TItem *next;                /* Nasledujúci prvok.          */
};

// ©truktúra pre zoznam
typedef struct
{
 TItem * first;               /* Pointer pre prvý prvok.     */
} TList;

// Pomocná ¹truktúra pre orientáciu na riadku
typedef struct columns
{
  int printval;               /* Poloha ståpca pre print.     */
  int selectval;              /* Poloha ståpca pre select.    */
  int count;                  /* Poèet ståpcov.               */
} TColumns;

// Správa nápovedy
const char *HELPMSG =
    "Program  :   .\n"
    "Autor    :   Michal Lukac, xlukac05@stud.fit.vutbr.cz\n"
    "Info     :   Tento program vznikol ako projekt pre predmet Zaklady\n"
    "             programovania na VUT FIT. Ulohou programu je radenie ceskych zaznamov.\n"
    "Parametre: proj4 -h\n"
    "           --print STLPEC\n"
    "           --after\n"
    "           --before\n"
    "           out.txt in.txt\n"
    "Popis parametrov:\n"
    "           -h  parameter pre vypis napovedy.\n"
    "           --print STLPEC povinny parameter pre vypis podla stlpca.\n"
    "           --after STLPEC TEXT nepovinne parametre pre vyber podla TEXT\n"
    "           --before STLPEC TEXT nepovinne parametre pre vyber podla TEXT\n"
    "           --sort nepovinny parameter pre radenie zaznamov\n";

// prototypy funkcií
//////////////////////////////////////////////////////////////////////////
TParams getParams(int argc, char *argv[]);                              //
void printHelp(void);                                                   //
void printECode(int ecode);                                             //
int select(TParams * param, TList *slist);                              //
void sort(TList *slist);                                                //
int getColumns(char *cl1, char *cl2,char *row,TColumns *colval);        //
int process(TParams * param);                                           //
int strczcmp(char *column,char *selectcolumn);                          //
TParams getParams(int argc, char *argv[]);                              //
void addItem(TItem *sitem,TList *slist);                                //
void freeList(TList *slist);                                            //
int readRow(char *row, FILE *fr);                                       //
int selsort(TList *slist, char *filename);                              //
void toConvert(char *stringa);                                          //
int identifyColumns(FILE *fr, TParams *param, TColumns *colval);        //
int printList(TList *slist, char *filename);                            //
int strlenght(char *column1,char *scolumn1);                            //
//////////////////////////////////////////////////////////////////////////

/**
 * Funkcia printhelp vytlaèí nápovedu a ukonèi program.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytlaèí hlásenie odpovedajúce chybovému kódu pomocou
 * výètového typu a pola re»azcov ECODEMSG.
 * @param code kód chyby programu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Funkcia getParams spracuje parametre prikazoveho riadku.
 * @param argc poèet parametrov príkazového riadka.
 * @param argv pole parametrov príkazového riadka.
 * @return ©truktúra obsahujúca naèítané parametre z argumentov príkazového.
 *         riadka
 */
TParams getParams(int argc, char *argv[])
{
  //inicializácia ¹truktúry
  TParams result =
  {
    .ecode = EOK,
    .select = 0,
    .sort = 0,
  };
  int required = 0, opt = 1;                              // premenné urèujúce èi je v¹etko zadané

  if(argc == 1)
    result.ecode = ECLWRONG;
  else
  {
    for(int x = 1; x < argc; x++)
    {
      if(strcmp(argv[x],"--print") == 0)                  // kontrola print
      {
        required++;
        if(x + 1 < argc)
        {
            opt+= 2;
            required++;
            x++;
            strcpy(result.printcolumn,argv[x]);
        }
        else
          result.ecode = ECLWRONG;
      }
      else if(strcmp(argv[x],"--before") == 0)            // kontrola --before
      {
        if(x+2 < argc)
        {
          result.select = 1; // before
          strcpy(result.selectcolumn,argv[x+1]);
          strcpy(result.selecttext,argv[x+2]);
          x = x + 2;
          opt += 3;
        }
        else
          result.ecode = ECLWRONG;
      }
      else if(strcmp(argv[x],"--after") == 0)             // kontrola --after
      {
        if(x+2 < argc)
        {
          result.select = -1; // after
          strcpy(result.selectcolumn,argv[x+1]);
          strcpy(result.selecttext,argv[x+2]);
          x = x + 2;
          opt += 3;
        }
        else
          result.ecode = ECLWRONG;
      }
      else if(strcmp(argv[x],"--sort") == 0)              // kontrola --sort
      {
        result.sort = 1;
        opt++;
      }
      else if(x == argc - 2)                              // meno zdrojového súbora
      {
        strcpy(result.filesrc,argv[x]);
        required++;
        opt++;
      }
      else if(x == argc - 1)                              // meno cieµového súbora
      {
        strcpy(result.filedest,argv[x]);
        required++;
        opt++;
      }
      else
        result.ecode = ECLWRONG;
    }
  }
  if(required != CPVAL)                                   // finálna podmienka
    result.ecode = ECLWRONG;
  if(opt != argc)
    result.ecode = ECLWRONG;
  if(argc == 2)                                           // ak je nápoveda
  {
      if(strcmp(argv[1],"-h") == 0)
      {
        strcpy(result.selectcolumn,argv[1]);
        result.ecode = EOK;
      }
  }
 return result;                                           // vrátenie spracovaných parametrov
}

/**
 * Funkcia addItem pridá polo¾ku do zoznamu.
 * @param sitem je polo¾ka na pridanie do zoznamu.
 * @param slist je zoznam pre pridanie polo¾ky.
 */
void addItem(TItem *sitem,TList *slist)
{
  if(slist != NULL && sitem != NULL)
  {
    sitem->next = slist->first;
    slist->first = sitem;
  }
}

/**
 * Funkcia freeList uvolní jednotlivé polo¾ky v zozname.
 * @param slist je zoznam pre uvolnenie
 */
 void freeList(TList *slist)
 {
   if(slist !=NULL)
   {
      TItem * it;
      while(slist->first)
      {
        it = slist->first;
        slist->first = it->next;
        free(it);
      }
   }
 }

 /**
 * Funkcia printList vytlaèí zoznam.
 * @param slist je lineárny zoznam
 * @param filename názov súboru
 * @return úspe¹nos» funkcie
 */
int printList(TList *slist, char *filename)
{
  FILE *fw;
  TItem *zoznam;

  if((fw = fopen(filename,"w")) != NULL)            // výpí¹ zoznam do súboru
  {
    for(zoznam= slist->first;zoznam != NULL ; zoznam = zoznam->next)
      fprintf(fw,"%s \n",zoznam->printitem);
    if(fclose(fw) == EOF) return EFILEO;
  }
  else
    return EFILEO;
  return EOK;
}

/**
 * Funkcia readRow preèíta riadok zo súbora.
 * @param row re»azec pre riadok
 * @param fr súbor z ktoré èítame riadok
 */
int readRow(char *row, FILE *fr)
{
  int c;                                      // premenná pre èítanie znakov
  int a = 0;                                  // premenná na indexáciu

  while((c = fgetc(fr)) != EOF && c != '\n')  // cyklus pre naèítanie znakov
  {
    row[a] = c;
    a++;
    if(a == CMAXCHAR) break;
  }
  row[a] = '\0';                              // ukonèenie re»azca

  if(c == EOF) return EOF;
  return 1;
}

/**
 * Funkcia identifyColumns identifikuje poriadie ståpcov.
 * @param fr ¹truktúra súbora
 * @param param ¹truktúra s údajmi z príkazového riadku
 * @param colval ¹truktúra urèujúca poèet a poradie ståpcov
 * @return úspe¹nos» funkcie
 */
int identifyColumns(FILE *fr, TParams *param, TColumns *colval)
{
  char row[CMAXCHAR];                         // pole pre prvý riadok
  int x = 1;                                  // poloha ståpca
  char * column;                              // ukazatel na ståpec

  readRow(row,fr);                            // preèítanie riadku
  column = strtok(row," \t\r");               // rozdelenie riadku pomocou strtok
  while(column != NULL)                       // rozdelovanie a identifikácia
  {
    if(param->select)
      if(strcmp(column,param->selectcolumn) == 0)
        colval->selectval = x;
    if(strcmp(column,param->printcolumn) == 0)
      colval->printval = x;
    x++;
    column = strtok(NULL," \t\r");            // ïaµ¹ie rozdelenie
  }

  colval->count = x-1;
  if(!param->select)                          // pokiaµ nie je zadaný after before...
    colval->selectval = colval->printval;
  if(param->select && colval->selectval == 0) // pokiaµ param select true ale niè sa nena¹lo
    return EFILEL;
  if(0 == colval->printval)                   // pokiaµ sa niè nena¹lo pre print
    return EFILEL;
  return EOK;
}

/**
 * Funkcia getColumns nájde ståpce pre print a after-before
 * @param cl1 re»azec pre ståpec print
 * @param cl2 re»azec pre ståpec after-before
 * @param row re»azec reprezentujúci riadok
 * @param colval ¹truktúra urèujúca poradie ståpcov
 * @return úspe¹nos» funkcie
 */
int getColumns(char *cl1, char *cl2,char *row,TColumns *colval)
{
  char *column1;                  // ukazatel na re»azec
  column1 = strtok(row," \t\r");  // získanie ståpca
  int a = 1;                      // pomocná premenná urèujúca poradie ståpca

  while(column1 != NULL)          // algoritmus pre získanie ståpcov pomocou strtok
  {
    if(a == colval->printval)
      strcpy(cl1,column1);
    if(a == colval->selectval)
      strcpy(cl2,column1);
    column1 = strtok(NULL," \t\r");
    a++;
  }
  if(a != colval->count + 1)
    return EFILEO;
  return EOK;
}

/**
 * Funkcia to czechTable skonvertuje re»azec pokiaµ v òom je ch.
 * @param string retazec pre prekonvertovanie
 */
void toConvert(char *stringa)
{
    char hstring[CCVAL];          // pomocný re»azec
    int i = strlen(stringa);      // prekopírovanie
    int b = 0;                    // pomocná premenná pre urèenie polohy v re»azci

    for(int a = 0; a < i; a++)    // algoritmus pre hladanie CH
    {
      if(stringa[a] == 'C' && a + 1 < i && stringa[a+1]=='h')
      {
        hstring[b] = CCH;
        a++;
      }
      else if(stringa[a] == 'c' && a + 1 < i && stringa[a+1]=='h')
      {
        hstring[b] = CCH_2;
        a++;
      }
      else
        hstring[b] = stringa[a];
      b++;
    }
    hstring[b]='\0';
    strcpy(stringa,hstring);      // spätné prekopírovanie
}

/**
 * Funkcia strlenght porovná då¾ku dvoch re»azcov
 * @param column1 prvý re»azec
 * @param scolumn1 druhý re»azec
 * @return då¾ka men¹ieho re»azca
 */
int strlenght(char *column1,char *scolumn1)
{
  int i = 0;
  if(strlen(scolumn1)>strlen(column1))  // porovnanie velkosti polo¾iek
    i = strlen(scolumn1);
  else
    i = strlen(column1);
  return i;
}

/**
 * Funkcia strczcmp porovná dva re»azce podµa èeského radenia.
 * @param column re»azec pre porovnávanie
 * @param selectcolumn re»azec pre porovnávanie
 * @return hodnota urèujúca ktorý re»azec je abecedne vacsi
 */
int strczcmp(char *column,char *selectcolumn)
{
  int comparevalue = 0;                 // premenná urèujúca rovnos» podµa 1.tabulky
  char column1[CCVAL];                  // pomocný re»azec column
  char scolumn1[CCVAL];                 // pomocný re»azec pre select..

  strcpy(column1,column);               // prekopírovanie re»azcov
  strcpy(scolumn1,selectcolumn);
  toConvert(column1);                   // konvertovanie re»azca
  toConvert(scolumn1);
  int i = strlenght(column1,scolumn1);  // porovnanie dvoch re»azcov

  for(int a = 0; a < i; a++)            // porovnanie podµa prvej tabulky
  {
    if(CZTABLE[(unsigned char)column1[a]] < CZTABLE[(unsigned char)scolumn1[a]])
      return 1;
    else if (CZTABLE[(unsigned char)column1[a]] > CZTABLE[(unsigned char)scolumn1[a]])
      return -1;
    else if ((CZTABLE[(unsigned char)column1[a]] == CZTABLE[(unsigned char)scolumn1[a]]))
      comparevalue = 1;
  }

  if(comparevalue)                      // porovnanie podµa druhej tabulky
  {
    for(int a = 0; a < i; a++)
    {
      if(CZTABLE2[(unsigned char)column1[a]] < CZTABLE2[(unsigned char)scolumn1[a]])
        return 1;
      else if (CZTABLE2[(unsigned char)column1[a]] > CZTABLE2[(unsigned char)scolumn1[a]])
        return -1;
    }
  }

  if(strlen(scolumn1) > strlen(column1))// finálne porovnanie
    return -1;
  else if(strlen(scolumn1) < strlen(column1))
    return 1;
  return 0;
}

/**
 * Funkcia select vybere z textového súboru po¾adované zlo¾ky
 * @param param je parameter s hodnotami z príkazového riadku
 * @param rows je ¹truktúra pre jednotlivé polo¾ky
 * @return úspe¹nos» funkcie
 */
int select(TParams * param, TList *slist)
{
  char row[CMAXCHAR];           // re»azec pre riadok
  char cl1[CCVAL];              // re»azec pre ståpec
  char cl2[CCVAL];              // re»azec pre ståpec
  TItem *item;                  // ukazatel na polo¾ku
  FILE *fw = NULL;              // typ pre súbor
  FILE *fr = NULL;              // typ pre súbor
  int rvalue;                   // návratová hodnota funkcie

  // ¹truktúra pre spracovanie prvého riadku
  TColumns colval = {.selectval = 0,.printval = 0,.count = 0,};

  if((fr = fopen(param->filesrc,"r")) == NULL) return EFILEO;
  if((rvalue = identifyColumns(fr,param,&colval)))  // identifikova» poradie ståpcov
    return rvalue;
  if(!param->sort)
  {
    if((fw = fopen(param->filedest,"w")) == NULL)
      { fclose(fr); return EFILEO;}
  }

  while((rvalue = readRow(row,fr)))                 // podµa identifikacie citat
  {
    if(getColumns(cl1,cl2,row,&colval) != EOK)      // získanie ståpcov
    {
      fclose(fr);
      if(!param->sort)  fclose(fw);
      if(rvalue == EOF) return EOK;      
        return EFILEL;
    }

    if(param->select)
    {
      if(strczcmp(cl2,param->selecttext) == param->select)
      {
        if(param->sort)                             //  ak je sort aj after-before
        {
          item =(TItem *) malloc(sizeof(TItem));
          if(item == NULL)
          {
            fclose(fr);
            return EMALLOC;
          }
          strcpy(item->printitem,cl1);
          addItem(item,slist);
        }
        else                                        // inak je iba after-before
          fprintf(fw,"%s\n",cl1);
      }
    }
    else if(param->sort)                            // ak je sort
    {
      item =(TItem *) malloc(sizeof(TItem));
      if(item == NULL)
      {
        fclose(fr);
        return EMALLOC;
      }
      strcpy(item->printitem,cl1);
      addItem(item,slist);
    }
    else if(!param->select && !param->sort)         // inak
      fprintf(fw,"%s\n",cl1);
    if(rvalue == EOF)
      break;
  }

  if(param->sort)                                   // uzavretie súbora
    if(fclose(fr) == EOF) return EFILEO;
  else
  {
    if(fclose(fr) == EOF) {fclose(fw);return EFILEO;}
    if(fclose(fw) == EOF) return EFILEO;
  }

  return EOK;
}

/**
 * Funkcia sort zoradí zoznam pomocou selectsortu.
 * @param slist je zoznam pre zoradenie
 * @param filename názov súboru
 * @return úspe¹nos» funkcie
 */
int selsort(TList *slist, char *filename)
{
  TItem *zoznam;  // ukazatel na polo¾ku
  TItem *zoznam2; // ukazatel na polo¾ku
  TItem x;        // pomocná premenná
  TItem y;        // pomocná premenná

  // algoritmus select sort
  for(zoznam = slist->first ; zoznam != NULL; zoznam = zoznam->next)
  {
    strcpy(x.printitem,zoznam->printitem);
    for(zoznam2 = zoznam; zoznam2 != NULL; zoznam2 = zoznam2->next)
    {
      if(strczcmp(zoznam2->printitem,x.printitem) == 1)
      {
        strcpy(y.printitem,x.printitem);
        strcpy(x.printitem,zoznam2->printitem);
        strcpy(zoznam2->printitem,y.printitem);
      }
    }
    strcpy(zoznam->printitem,x.printitem);
  }
  return printList(slist,filename);
}

/**
 * Funkcia process volá funkcie podµa parametra z pr. riadku.
 * @param param hodnoty príkazového riadku
 * @return úspeènos» funkcie
 */
int process(TParams * param)
{
  int rvalue = EOK;     // návratová hodnota funkcie-úspe¹nos»
  TList list;           // ¹truktúra pre záznam
  list.first = NULL;    // inicializácia záznamu

  rvalue = select(param,&list);
  if(param->sort && rvalue == EOK)
  {
    rvalue = selsort(&list,param->filedest);
  }
  freeList(&list);
  return rvalue;
}

/**
 * Funkcia main volá funkciu pre spracovanie parametrov
 * a podµa toho prislúchajúce funkcie.
 * @param argc poèet parametrov z príkazového riadku
 * @param argv jednotlivé polo¾ky z príkazového riadku
 * @return úspe¹nos» programu
 */
int main(int argc, char *argv[])
{
  TParams result = getParams(argc, argv); // získanie parametrov z príkazového riadka
  int error = 0;                          // chyba nastavená na v¹etko v poriadku

  if(result.ecode > EOK)                  // ak je chyba
    error = result.ecode;
  else                                    // inak zavolaj po¾adovanú funkciu
  {
    if(strcmp(result.selectcolumn,"-h") == 0)
      printHelp();
    else
      error = process(&result);
  }
  if(error)                               // zavolaj chybový výstup
  {
      printECode(error);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
// koniec súboru proj4.c od xlukac05
