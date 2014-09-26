/*
 * S�bor    :   proj4.c
 * D�tum    :   11.12.2010
 * Autor    :   Michal Luk��, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Projekt �.4. �esk� radenie z�znamov.
 * Popis    :   Tento projekt je projektom do predmetu z�klady programovania.
 * P.riadkov:   741
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMAXCHAR 1000     /* Kon�tanta pre max. po�et znakov v riadku.  */
#define CCVAL 200         /* Kon�tanta pre max. po�et znakov v stlpci.  */
const int CPVAL = 4;      /* Kon�tanta pre po�et povinn�ch parametrov.  */
const char CCH = '%';     /* Kon�tanta pre ch.                          */
const char CCH_2 = '&';   /* Kon�tanta pre CH.                          */

// tabulka �esk�ch znakov pod�a hodnoty
const unsigned char CZTABLE[256] =
{
  [(unsigned char)' '] = 0,  [(unsigned char)'Q'] = 19,
  [(unsigned char)'A'] = 1,  [(unsigned char)'q'] = 19,
  [(unsigned char)'�'] = 1,  [(unsigned char)'R'] = 20,
  [(unsigned char)'a'] = 1,  [(unsigned char)'r'] = 20,
  [(unsigned char)'�'] = 1,  [(unsigned char)'�'] = 21,
  [(unsigned char)'B'] = 2,  [(unsigned char)'�'] = 21,
  [(unsigned char)'b'] = 2,  [(unsigned char)'S'] = 22,
  [(unsigned char)'C'] = 3,  [(unsigned char)'s'] = 22,
  [(unsigned char)'c'] = 3,  [(unsigned char)'�'] = 23,
  [(unsigned char)'�'] = 4,  [(unsigned char)'�'] = 23,
  [(unsigned char)'�'] = 4,  [(unsigned char)'T'] = 24,
  [(unsigned char)'D'] = 5,  [(unsigned char)'�'] = 24,
  [(unsigned char)'�'] = 5,  [(unsigned char)'t'] = 24,
  [(unsigned char)'d'] = 5,  [(unsigned char)'�'] = 24,
  [(unsigned char)'�'] = 5,  [(unsigned char)'U'] = 25,
  [(unsigned char)'E'] = 6,  [(unsigned char)'�'] = 25,
  [(unsigned char)'�'] = 6,  [(unsigned char)'�'] = 25,
  [(unsigned char)'�'] = 6,  [(unsigned char)'u'] = 25,
  [(unsigned char)'e'] = 6,  [(unsigned char)'�'] = 25,
  [(unsigned char)'�'] = 6,  [(unsigned char)'�'] = 25,
  [(unsigned char)'�'] = 6,  [(unsigned char)'V'] = 26,
  [(unsigned char)'F'] = 7,  [(unsigned char)'v'] = 26,
  [(unsigned char)'f'] = 7,  [(unsigned char)'W'] = 27,
  [(unsigned char)'G'] = 8,  [(unsigned char)'w'] = 27,
  [(unsigned char)'g'] = 8,  [(unsigned char)'X'] = 28,
  [(unsigned char)'H'] = 9,  [(unsigned char)'x'] = 28,
  [(unsigned char)'h'] = 9,  [(unsigned char)'Y'] = 29,
  [(unsigned char)'&'] = 10, [(unsigned char)'y'] = 29,
  [(unsigned char)'%'] = 10, [(unsigned char)'�'] = 29,
  [(unsigned char)'I'] = 11, [(unsigned char)'�'] = 29,
  [(unsigned char)'i'] = 11, [(unsigned char)'Z'] = 30,
  [(unsigned char)'�'] = 11, [(unsigned char)'z'] = 30,
  [(unsigned char)'�'] = 11, [(unsigned char)'�'] = 31,
  [(unsigned char)'J'] = 12, [(unsigned char)'�'] = 31,
  [(unsigned char)'j'] = 12, [(unsigned char)'0'] = 32,
  [(unsigned char)'K'] = 13, [(unsigned char)'1'] = 33,
  [(unsigned char)'k'] = 13, [(unsigned char)'2'] = 34,
  [(unsigned char)'L'] = 14, [(unsigned char)'3'] = 35,
  [(unsigned char)'l'] = 14, [(unsigned char)'4'] = 36,
  [(unsigned char)'M'] = 15, [(unsigned char)'5'] = 37,
  [(unsigned char)'m'] = 15, [(unsigned char)'6'] = 38,
  [(unsigned char)'N'] = 16, [(unsigned char)'7'] = 39,
  [(unsigned char)'�'] = 16, [(unsigned char)'8'] = 40,
  [(unsigned char)'n'] = 16, [(unsigned char)'9'] = 41,
  [(unsigned char)'�'] = 16, [(unsigned char)'.'] = 42,
  [(unsigned char)'O'] = 17, [(unsigned char)','] = 43,
  [(unsigned char)'�'] = 17, [(unsigned char)';'] = 44,
  [(unsigned char)'o'] = 18, [(unsigned char)'?'] = 45,
  [(unsigned char)'�'] = 18, [(unsigned char)'!'] = 46,
  [(unsigned char)'P'] = 18, [(unsigned char)':'] = 47,
  [(unsigned char)'p'] = 18, [(unsigned char)'"'] = 48,
                             [(unsigned char)'-'] = 49,
};

// druh� tabulka vyjadruj�ca v�hy znakov odli�ne pre sekund�rne porovnanie
const unsigned char CZTABLE2[256] =
{
  [(unsigned char)' '] = 0,  [(unsigned char)'Q'] = 26,
  [(unsigned char)'A'] = 1,  [(unsigned char)'q'] = 26,
  [(unsigned char)'�'] = 2,  [(unsigned char)'R'] = 27,
  [(unsigned char)'a'] = 1,  [(unsigned char)'r'] = 27,
  [(unsigned char)'�'] = 2,  [(unsigned char)'�'] = 28,
  [(unsigned char)'B'] = 3,  [(unsigned char)'�'] = 28,
  [(unsigned char)'b'] = 3,  [(unsigned char)'S'] = 29,
  [(unsigned char)'C'] = 4,  [(unsigned char)'s'] = 29,
  [(unsigned char)'c'] = 4,  [(unsigned char)'�'] = 30,
  [(unsigned char)'�'] = 5,  [(unsigned char)'�'] = 30,
  [(unsigned char)'�'] = 5,  [(unsigned char)'T'] = 31,
  [(unsigned char)'D'] = 6,  [(unsigned char)'�'] = 32,
  [(unsigned char)'�'] = 7,  [(unsigned char)'t'] = 31,
  [(unsigned char)'d'] = 6,  [(unsigned char)'�'] = 32,
  [(unsigned char)'�'] = 7,  [(unsigned char)'U'] = 33,
  [(unsigned char)'E'] = 8,  [(unsigned char)'�'] = 34,
  [(unsigned char)'�'] = 9,  [(unsigned char)'�'] = 35,
  [(unsigned char)'�'] = 10, [(unsigned char)'u'] = 33,
  [(unsigned char)'e'] = 8,  [(unsigned char)'�'] = 34,
  [(unsigned char)'�'] = 9,  [(unsigned char)'�'] = 35,
  [(unsigned char)'�'] = 10, [(unsigned char)'V'] = 36,
  [(unsigned char)'F'] = 11, [(unsigned char)'v'] = 36,
  [(unsigned char)'f'] = 11, [(unsigned char)'W'] = 37,
  [(unsigned char)'G'] = 12, [(unsigned char)'w'] = 37,
  [(unsigned char)'g'] = 12, [(unsigned char)'X'] = 38,
  [(unsigned char)'H'] = 13, [(unsigned char)'x'] = 38,
  [(unsigned char)'h'] = 13, [(unsigned char)'Y'] = 39,
  [(unsigned char)'&'] = 14, [(unsigned char)'y'] = 39,
  [(unsigned char)'%'] = 14, [(unsigned char)'�'] = 40,
  [(unsigned char)'I'] = 15, [(unsigned char)'�'] = 40,
  [(unsigned char)'i'] = 15, [(unsigned char)'Z'] = 41,
  [(unsigned char)'�'] = 16, [(unsigned char)'z'] = 41,
  [(unsigned char)'�'] = 16, [(unsigned char)'�'] = 42,
  [(unsigned char)'J'] = 17, [(unsigned char)'�'] = 42,
  [(unsigned char)'j'] = 17, [(unsigned char)'0'] = 43,
  [(unsigned char)'K'] = 18, [(unsigned char)'1'] = 44,
  [(unsigned char)'k'] = 18, [(unsigned char)'2'] = 45,
  [(unsigned char)'L'] = 19, [(unsigned char)'3'] = 46,
  [(unsigned char)'l'] = 19, [(unsigned char)'4'] = 47,
  [(unsigned char)'M'] = 20, [(unsigned char)'5'] = 48,
  [(unsigned char)'m'] = 20, [(unsigned char)'6'] = 49,
  [(unsigned char)'N'] = 21, [(unsigned char)'7'] = 50,
  [(unsigned char)'�'] = 22, [(unsigned char)'8'] = 51,
  [(unsigned char)'n'] = 21, [(unsigned char)'9'] = 52,
  [(unsigned char)'�'] = 22, [(unsigned char)'.'] = 53,
  [(unsigned char)'O'] = 23, [(unsigned char)','] = 54,
  [(unsigned char)'�'] = 24, [(unsigned char)';'] = 55,
  [(unsigned char)'o'] = 23, [(unsigned char)'?'] = 56,
  [(unsigned char)'�'] = 24, [(unsigned char)'!'] = 57,
  [(unsigned char)'P'] = 25, [(unsigned char)':'] = 58,
  [(unsigned char)'p'] = 25, [(unsigned char)'"'] = 59,
                             [(unsigned char)'-'] = 60,
};

// K�dy ch�b programu
enum tecodes
{
  EOK = 0,                      /* Bez chyby.                        */
  ECLWRONG = 1,                 /* Chybn� pr�kazov� riadok.          */
  EFILEO = 2,                   /* Chyba pri otv�ran� s�boru.        */
  EMALLOC = 3,                  /* Chyba pri alokovan� pam�te.       */
  EFILEL = 4,                   /* Chyba pri na��tan� zo s�boru.     */
};

// Chybov� hl�senia odpovedaj�ce chybov�m k�dom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nChyba pri otvarani-zatvarani suboru!\n",             /* EFILEO   */
  "\nChyba pri alokovani pamate!\n",                      /* EMALLOC  */
  "\nChyba pri citani/zapisu suboru!\n",                  /* EFILEL   */
};

// �trukt�ra obsahuj�ca hodnoty parametrov z pr�kazovej riadky
typedef struct params
{
  int ecode;                    /* Chybov� k�d programu, odpoved� v��tu tecodes.    */
  int select;                   /* Stav pre parameter before = 1; after = 2.        */
  char selectcolumn[CCVAL];     /* Stav pre vybranie riadkov.                       */
  char printcolumn[CCVAL];      /* Ktor� st�pec sa m� tla�i�.                       */
  int sort;                     /* Stav pre porovn�vanie vzostupne.                 */
  char filesrc[CCVAL];          /* N�zov k zdrojov�mu s�boru.                       */
  char filedest[CCVAL];         /* N�zov k cie�ov�mu s�boru.                        */
  char selecttext[CCVAL];       /* Stav pod�a ak�ho re�azca sa bude vybera�.        */
} TParams;

// �trukt�ra pre polo�ku
typedef struct item TItem;
struct item
{
  char printitem[CCVAL];      /* Re�azec pre st�pec print.   */
  TItem *next;                /* Nasleduj�ci prvok.          */
};

// �trukt�ra pre zoznam
typedef struct
{
 TItem * first;               /* Pointer pre prv� prvok.     */
} TList;

// Pomocn� �trukt�ra pre orient�ciu na riadku
typedef struct columns
{
  int printval;               /* Poloha st�pca pre print.     */
  int selectval;              /* Poloha st�pca pre select.    */
  int count;                  /* Po�et st�pcov.               */
} TColumns;

// Spr�va n�povedy
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

// prototypy funkci�
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
 * Funkcia printhelp vytla�� n�povedu a ukon�i program.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytla�� hl�senie odpovedaj�ce chybov�mu k�du pomocou
 * v��tov�ho typu a pola re�azcov ECODEMSG.
 * @param code k�d chyby programu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Funkcia getParams spracuje parametre prikazoveho riadku.
 * @param argc po�et parametrov pr�kazov�ho riadka.
 * @param argv pole parametrov pr�kazov�ho riadka.
 * @return �trukt�ra obsahuj�ca na��tan� parametre z argumentov pr�kazov�ho.
 *         riadka
 */
TParams getParams(int argc, char *argv[])
{
  //inicializ�cia �trukt�ry
  TParams result =
  {
    .ecode = EOK,
    .select = 0,
    .sort = 0,
  };
  int required = 0, opt = 1;                              // premenn� ur�uj�ce �i je v�etko zadan�

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
      else if(x == argc - 2)                              // meno zdrojov�ho s�bora
      {
        strcpy(result.filesrc,argv[x]);
        required++;
        opt++;
      }
      else if(x == argc - 1)                              // meno cie�ov�ho s�bora
      {
        strcpy(result.filedest,argv[x]);
        required++;
        opt++;
      }
      else
        result.ecode = ECLWRONG;
    }
  }
  if(required != CPVAL)                                   // fin�lna podmienka
    result.ecode = ECLWRONG;
  if(opt != argc)
    result.ecode = ECLWRONG;
  if(argc == 2)                                           // ak je n�poveda
  {
      if(strcmp(argv[1],"-h") == 0)
      {
        strcpy(result.selectcolumn,argv[1]);
        result.ecode = EOK;
      }
  }
 return result;                                           // vr�tenie spracovan�ch parametrov
}

/**
 * Funkcia addItem prid� polo�ku do zoznamu.
 * @param sitem je polo�ka na pridanie do zoznamu.
 * @param slist je zoznam pre pridanie polo�ky.
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
 * Funkcia freeList uvoln� jednotliv� polo�ky v zozname.
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
 * Funkcia printList vytla�� zoznam.
 * @param slist je line�rny zoznam
 * @param filename n�zov s�boru
 * @return �spe�nos� funkcie
 */
int printList(TList *slist, char *filename)
{
  FILE *fw;
  TItem *zoznam;

  if((fw = fopen(filename,"w")) != NULL)            // v�p� zoznam do s�boru
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
 * Funkcia readRow pre��ta riadok zo s�bora.
 * @param row re�azec pre riadok
 * @param fr s�bor z ktor� ��tame riadok
 */
int readRow(char *row, FILE *fr)
{
  int c;                                      // premenn� pre ��tanie znakov
  int a = 0;                                  // premenn� na index�ciu

  while((c = fgetc(fr)) != EOF && c != '\n')  // cyklus pre na��tanie znakov
  {
    row[a] = c;
    a++;
    if(a == CMAXCHAR) break;
  }
  row[a] = '\0';                              // ukon�enie re�azca

  if(c == EOF) return EOF;
  return 1;
}

/**
 * Funkcia identifyColumns identifikuje poriadie st�pcov.
 * @param fr �trukt�ra s�bora
 * @param param �trukt�ra s �dajmi z pr�kazov�ho riadku
 * @param colval �trukt�ra ur�uj�ca po�et a poradie st�pcov
 * @return �spe�nos� funkcie
 */
int identifyColumns(FILE *fr, TParams *param, TColumns *colval)
{
  char row[CMAXCHAR];                         // pole pre prv� riadok
  int x = 1;                                  // poloha st�pca
  char * column;                              // ukazatel na st�pec

  readRow(row,fr);                            // pre��tanie riadku
  column = strtok(row," \t\r");               // rozdelenie riadku pomocou strtok
  while(column != NULL)                       // rozdelovanie a identifik�cia
  {
    if(param->select)
      if(strcmp(column,param->selectcolumn) == 0)
        colval->selectval = x;
    if(strcmp(column,param->printcolumn) == 0)
      colval->printval = x;
    x++;
    column = strtok(NULL," \t\r");            // �a��ie rozdelenie
  }

  colval->count = x-1;
  if(!param->select)                          // pokia� nie je zadan� after before...
    colval->selectval = colval->printval;
  if(param->select && colval->selectval == 0) // pokia� param select true ale ni� sa nena�lo
    return EFILEL;
  if(0 == colval->printval)                   // pokia� sa ni� nena�lo pre print
    return EFILEL;
  return EOK;
}

/**
 * Funkcia getColumns n�jde st�pce pre print a after-before
 * @param cl1 re�azec pre st�pec print
 * @param cl2 re�azec pre st�pec after-before
 * @param row re�azec reprezentuj�ci riadok
 * @param colval �trukt�ra ur�uj�ca poradie st�pcov
 * @return �spe�nos� funkcie
 */
int getColumns(char *cl1, char *cl2,char *row,TColumns *colval)
{
  char *column1;                  // ukazatel na re�azec
  column1 = strtok(row," \t\r");  // z�skanie st�pca
  int a = 1;                      // pomocn� premenn� ur�uj�ca poradie st�pca

  while(column1 != NULL)          // algoritmus pre z�skanie st�pcov pomocou strtok
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
 * Funkcia to czechTable skonvertuje re�azec pokia� v �om je ch.
 * @param string retazec pre prekonvertovanie
 */
void toConvert(char *stringa)
{
    char hstring[CCVAL];          // pomocn� re�azec
    int i = strlen(stringa);      // prekop�rovanie
    int b = 0;                    // pomocn� premenn� pre ur�enie polohy v re�azci

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
    strcpy(stringa,hstring);      // sp�tn� prekop�rovanie
}

/**
 * Funkcia strlenght porovn� d�ku dvoch re�azcov
 * @param column1 prv� re�azec
 * @param scolumn1 druh� re�azec
 * @return d�ka men�ieho re�azca
 */
int strlenght(char *column1,char *scolumn1)
{
  int i = 0;
  if(strlen(scolumn1)>strlen(column1))  // porovnanie velkosti polo�iek
    i = strlen(scolumn1);
  else
    i = strlen(column1);
  return i;
}

/**
 * Funkcia strczcmp porovn� dva re�azce pod�a �esk�ho radenia.
 * @param column re�azec pre porovn�vanie
 * @param selectcolumn re�azec pre porovn�vanie
 * @return hodnota ur�uj�ca ktor� re�azec je abecedne vacsi
 */
int strczcmp(char *column,char *selectcolumn)
{
  int comparevalue = 0;                 // premenn� ur�uj�ca rovnos� pod�a 1.tabulky
  char column1[CCVAL];                  // pomocn� re�azec column
  char scolumn1[CCVAL];                 // pomocn� re�azec pre select..

  strcpy(column1,column);               // prekop�rovanie re�azcov
  strcpy(scolumn1,selectcolumn);
  toConvert(column1);                   // konvertovanie re�azca
  toConvert(scolumn1);
  int i = strlenght(column1,scolumn1);  // porovnanie dvoch re�azcov

  for(int a = 0; a < i; a++)            // porovnanie pod�a prvej tabulky
  {
    if(CZTABLE[(unsigned char)column1[a]] < CZTABLE[(unsigned char)scolumn1[a]])
      return 1;
    else if (CZTABLE[(unsigned char)column1[a]] > CZTABLE[(unsigned char)scolumn1[a]])
      return -1;
    else if ((CZTABLE[(unsigned char)column1[a]] == CZTABLE[(unsigned char)scolumn1[a]]))
      comparevalue = 1;
  }

  if(comparevalue)                      // porovnanie pod�a druhej tabulky
  {
    for(int a = 0; a < i; a++)
    {
      if(CZTABLE2[(unsigned char)column1[a]] < CZTABLE2[(unsigned char)scolumn1[a]])
        return 1;
      else if (CZTABLE2[(unsigned char)column1[a]] > CZTABLE2[(unsigned char)scolumn1[a]])
        return -1;
    }
  }

  if(strlen(scolumn1) > strlen(column1))// fin�lne porovnanie
    return -1;
  else if(strlen(scolumn1) < strlen(column1))
    return 1;
  return 0;
}

/**
 * Funkcia select vybere z textov�ho s�boru po�adovan� zlo�ky
 * @param param je parameter s hodnotami z pr�kazov�ho riadku
 * @param rows je �trukt�ra pre jednotliv� polo�ky
 * @return �spe�nos� funkcie
 */
int select(TParams * param, TList *slist)
{
  char row[CMAXCHAR];           // re�azec pre riadok
  char cl1[CCVAL];              // re�azec pre st�pec
  char cl2[CCVAL];              // re�azec pre st�pec
  TItem *item;                  // ukazatel na polo�ku
  FILE *fw = NULL;              // typ pre s�bor
  FILE *fr = NULL;              // typ pre s�bor
  int rvalue;                   // n�vratov� hodnota funkcie

  // �trukt�ra pre spracovanie prv�ho riadku
  TColumns colval = {.selectval = 0,.printval = 0,.count = 0,};

  if((fr = fopen(param->filesrc,"r")) == NULL) return EFILEO;
  if((rvalue = identifyColumns(fr,param,&colval)))  // identifikova� poradie st�pcov
    return rvalue;
  if(!param->sort)
  {
    if((fw = fopen(param->filedest,"w")) == NULL)
      { fclose(fr); return EFILEO;}
  }

  while((rvalue = readRow(row,fr)))                 // pod�a identifikacie citat
  {
    if(getColumns(cl1,cl2,row,&colval) != EOK)      // z�skanie st�pcov
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

  if(param->sort)                                   // uzavretie s�bora
    if(fclose(fr) == EOF) return EFILEO;
  else
  {
    if(fclose(fr) == EOF) {fclose(fw);return EFILEO;}
    if(fclose(fw) == EOF) return EFILEO;
  }

  return EOK;
}

/**
 * Funkcia sort zorad� zoznam pomocou selectsortu.
 * @param slist je zoznam pre zoradenie
 * @param filename n�zov s�boru
 * @return �spe�nos� funkcie
 */
int selsort(TList *slist, char *filename)
{
  TItem *zoznam;  // ukazatel na polo�ku
  TItem *zoznam2; // ukazatel na polo�ku
  TItem x;        // pomocn� premenn�
  TItem y;        // pomocn� premenn�

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
 * Funkcia process vol� funkcie pod�a parametra z pr. riadku.
 * @param param hodnoty pr�kazov�ho riadku
 * @return �spe�nos� funkcie
 */
int process(TParams * param)
{
  int rvalue = EOK;     // n�vratov� hodnota funkcie-�spe�nos�
  TList list;           // �trukt�ra pre z�znam
  list.first = NULL;    // inicializ�cia z�znamu

  rvalue = select(param,&list);
  if(param->sort && rvalue == EOK)
  {
    rvalue = selsort(&list,param->filedest);
  }
  freeList(&list);
  return rvalue;
}

/**
 * Funkcia main vol� funkciu pre spracovanie parametrov
 * a pod�a toho prisl�chaj�ce funkcie.
 * @param argc po�et parametrov z pr�kazov�ho riadku
 * @param argv jednotliv� polo�ky z pr�kazov�ho riadku
 * @return �spe�nos� programu
 */
int main(int argc, char *argv[])
{
  TParams result = getParams(argc, argv); // z�skanie parametrov z pr�kazov�ho riadka
  int error = 0;                          // chyba nastaven� na v�etko v poriadku

  if(result.ecode > EOK)                  // ak je chyba
    error = result.ecode;
  else                                    // inak zavolaj po�adovan� funkciu
  {
    if(strcmp(result.selectcolumn,"-h") == 0)
      printHelp();
    else
      error = process(&result);
  }
  if(error)                               // zavolaj chybov� v�stup
  {
      printECode(error);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
// koniec s�boru proj4.c od xlukac05
