/*
 * Súbor    :   proj1.c
 * Dátum    :   26.10.2010 21:00
 * Autor    :   Michal Lukáè, xlukac05@fit.vutbr.cz
 * Projekt  :   Jednoduchá kompresia textu, projekt è. 1 pre predmet IZP v
 *              zimnom semestri 2010/11 na VUT FIT.
 * Popis    :   Tento program je prvý projekt pre predmet Základy programovania
 *              na VUT FIT v Brne. Pri tomto programe som vyu¾il kostru programu
 *              zo stránky projektu pre IZP, prièom som túto kostru trochu obmenil
 *              a upravil. Úlohou programu je komprimácia a dekomprimácia textu podµa
 *              zadaného parametra z príkazovej riadky/terimnálu. Poèet blokov pri
 *              komprimácii a dekomprimácii nesmie presiahnu» èíslo 9. Pri komprimácii
 *              som vyu¾il pohyb v poli pomocou kruhového buffera {(i + 1) % velkost
 *              pola, kde i je aktuálny index}. Výhodou takéhoto pohybu v poli je ¾e sa pri
 *              prejdení posledného prvku vrátime na zaèiatok pola. Av¹ak mô¾e nasta» prípad
 *              kedy sa nebude rovna» celý blok, takúto situáciu som zabezpeèil opaèným
 *              "kruhovým bufferom" kedy sa vrátime o x prvkov dozadu. Pri dekomprimáciu
 *              som vyu¾il jednoduchý algoritmus v ktorom som zis»oval èi daný znak
 *              je èíslo alebo nie a podµa toho poèítal a posielal na výstup znaky.
 *              Veµkos» parametra N je daná typom unsigned int.Preteèenie tohto èísla som
 *              kontroloval pomocou pomocnej premennej dátového typu long long int.
 * P.riadkov:   607
 * TODO     :   -
 */

/************************** Hlavièkové súbory ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

/************** Globálne premenné, kon¹tanty, makra, ¹truktúry... ********/
// Kon¹tanty
const int C_ZERO = 48;                     /* Hodnota ascii pre znak '0' */
const int C_EOF = 26;                      /* Mo¾ná hodnota "EOF"        */
const int C_MAX_BLCKS = 9;                 /* Maximálny poèet blokov     */
const int C_ASCIMAX = 127;                 /* Konec prvej polovice ASCII */
const int C_EOF2 = -1;                     /* Hodnota pre znak EOF       */
const char C_NINE = '9';                   /* Znak 9                     */

// Kódy chýb programu
enum tecodes
{
  EOK = 0,        /* Bez chyby.                                  */
  ECLWRONG = 1,   /* Chybný príkazový riadok.                    */
  ECLVOID = 2,    /* Chybný príkazový riadok - ¾iadny parameter. */
  ECLNUM = 3,     /* Zle zadaná hodnota N!                       */
  EOVRLFW = 4,    /* Chyba preteèenia hodnoty N!                 */
  EGETCHAR = 5,   /* Chyba pri zadávaní znakov.                  */
  EMALLOC = 6,    /* Chyba pri alokácii pamäte.                  */
  EFEND = 7,      /* Cyklus bol predèasne ukonèený.              */
  ECLMANY = 8,    /* Príli¹ mnoho parametrov.                    */
};

// Stavové kódy programu spracované podµa parametra
enum tstates
{
  CHELP,          /* Nápoveda.      */
  CCOMP,          /* Komprimova».   */
  CDECOMP,        /* Dekomprimova». */
};

// Chybové hlásenia odpovedajúce chybovým kódom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nNebol zadany ziadny parameter prikazoveho riadku!\n",/* ECLVOID  */
  "\nZle zadana hodnota N!\n",                            /* ECLNUM   */
  "\nChyba pretecenia hodnoty N!\n",                      /* EOVRLFW  */
  "\nChyba pri zadavani vstupnych znakov!\n",             /* EGETCHAR */
  "\nChyba pri alokacii pamate!\n",                       /* EMALLOC  */
  "\nChyba, cyklus bol predcasne ukonceny!",              /* EFEND    */
  "\nChyba, prilis mnoho parametrov!",                    /* ECLMANY  */
};

// Správa nápovedy
const char *HELPMSG =
    "Program: Jednoducha kompresia textu.\n"
    "Autor:   Michal Lukac, xlukac05@fit.vutbr.cz\n"
    "Info:    Program sluzi na komprimaciu a dekomprimaciu\n"
    "         podla zadaneho parametru. Tento program je\n"
    "         robeny ako projekt na VUT FIT do predmetu IZP.\n"
    "         Program spustite z prikazovej riadku-terminalu s nizsie\n"
    "         uvedenymi parametrami. Pri komprimacnom algoritme bol vyuzity\n"
    "         algoritmus pohybu v poli pomocou kruhoveho buffera.\n"
    "         V tomto programe bola vyuzita povolena kostra programu\n"
    "         zo stranok projektov IZP.\n"
    "Parametre: proj1 -h\n"
    "           proj1 -c N\n"
    "           proj1 -d N\n"
    "Popis parametrov:\n"
    "   -h parameter pre vypis napovedy.\n"
    "   -c N parameter pre komprimaciu\n"
    "   -d N parameter pre dekomprimaciu\n"
    "   kde N udava pocet blokov N pri komprimacii a dekomprimacii";

// ©truktúra obsahujúca hodnoty parametrov z príkazovej riadky
typedef struct params
{
  unsigned int N;   /* Hodnota N z príkazovej riadky.                 */
  int ecode;        /* Chybový kód programu, odpovedá výètu tecodes.  */
  int state;        /* Stavový kód programu, odpovedá výètu tstates.  */
} TParams;

/************************ Prototypy funkcií **********************************/
/////////////////////////////////////////////////////////////////////////////
void printHelp(void);                                                      //
void getParams(TParams *result,int argc, char *argv[]);                    //
int printECode(int ecode);                                                 //
int checkNumber(char *cnumber,long long int *number);                      //
int comp(TParams *result);                                                 //
int decomp(TParams *result);                                               //
/////////////////////////////////////////////////////////////////////////////

/************************** FUNKCIE PROGRAMU *********************************/

/**
 * Hlavná funkcia main()
 * Funkcia main neobsahuje ¾iadny výpoèet. Podµa zadaných parametrov
 * volá prislúchajúce funkcie. Pokiaµ niesú parametre v poriadku zavolá sa funkcia
 * printECode ktorá vytlaèí chybový stav. Ak je parameter -h zavolá sa funkcia
 * printHelp ktorá vytlaèí informácie-pomocníka programu. Pokiaµ je parameter
 * -c N alebo -d N kde N je celé èíslo zavolá sa buï funkcia comp pre komprimáciu
 * textu alebo funkcia decomp pre dekomprimáciu textu.
 * @param argc - poèet parametrov.
 * @param argv - konkrétne parametry.
 * @return - 'úspe¹nos»' programu
 */
int main(int argc, char *argv[])
{
  // inicializácia ¹truktúry ktorá sa predá odkazom
  TParams params =
  {
    .N = 0,
    .ecode = EOK,
    .state = CCOMP,
  };
  int error = 0; // premenná pre chybu

  // zistenie argumentov
  getParams(&params,argc,argv);

  // zachytenie chyb v parametroch,kontrola stavov == volanie príslu¹ných funkcií
  if (params.ecode > EOK)
  {
    error = printECode(params.ecode);   // volanie chybového hlásenia
  }
  else
  {
    switch(params.state)
    {
      case CHELP:                       // volanie pomocníka
            printHelp();
            break;
      case CCOMP:
            error = comp(&params);      // volanie komprimácie
            break;
      case CDECOMP:
            error = decomp(&params);    // volanie dekomprimácie
            break;
      default:
            break;
    }
  }

  // ak funkcia comp, decomp, printecode vráti nejakú chybu
  if(error == 1)
  {
      return EXIT_FAILURE;
  }

  // úspe¹né ukonèenie programu
  return EXIT_SUCCESS;
}

/**
 * Funkcia printhelp vytlaèí nápovedu a ukonèi program.
 * Táto funkcia je volaná hlavnou funkciou main.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytlaèí hlásenie odpovedajúce chybovému kódu pomocou
 * výètového typu a pole re»azcov ECODEMSG
 * @param code kód chyby programu
 * @return vráti chybový stav
 */
int printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
  return 1;
}

/**
 * Funkcia getParams spracuje argumenty príkazového riadku a vrati ich odkazom v ¹truktúry result.
 * Pokiaµ je formát argumentov chybný, ukonèí program s chybovým kódom.
 * Pri práci s hodnotou èísla N som vyu¾il pri pomocnej premennej
 * datový typ long long int, pre porovnanie èi daná hodnota je väè¹ia ako
 * datový typ unsigned int. Taktie¾ je skontrolované èi hodnota èísla N je
 * väè¹ia ako 0, prípadne èi je zadaný správny poèet parametrov...
 * @param result ¹truktúra reprezentovaná ako odkaz uchovávajúca hodnoty parametrov
 * @param argc Poèet argumentov.
 * @param argv Pole textových øe»azcov s argumentami.
 */
void getParams(TParams * result,int argc, char *argv[])
{

  if(argc == 1) // ak sa nezadá ¾iadny parameter
  {
    result->ecode = ECLVOID;
  }
  else if (argc == 2 && (strcmp("-h", argv[1]) == 0)) // ak je parameter -h nastav stav na nápovedy
  {
    result->state = CHELP;
  }
  else if (argc == 3) // dva parametry
  {
    long long int number = 0;   // pomocná premenná na kontrolu hodnoty èísla N

    if(strcmp("-c",argv[1]) == 0)  // ak je param. pre komprimáciu
    {
      if(checkNumber(argv[2],&number) && number >= 1) // ak je èíslo n v správnom tvare
      {
        if(number <= UINT_MAX)                          // ak je èíslo n  v rozsahu unsigned int
        {
            result->N = number;
        }
        else                                            // inak vypí¹ chybu preteèenia unsigned int
        {
          result->ecode = EOVRLFW;
        }
      }
      else                          // inak vypí¹ chybu zle zadaného èísla
      {
        result->ecode = ECLNUM;
      }
    }
    else if(strcmp("-d",argv[1]) == 0) //ak je dekomprimacia
    {
      if(checkNumber(argv[2], &number) && number >= 1) //ak je èíslo n v správnom tvare
      {
        if(number <= UINT_MAX)           //ak je èislo n v rozsahu unsigned int
        {
          result->N = number;
          result->state = CDECOMP;
        }
        else    // inak vypí¹ chybu preteèenia unsigned int
        {
          result->ecode = EOVRLFW;
        }
      }
      else // inak vypí¹ chybu zle zadaného èísla N
      {
        result->ecode = ECLNUM;
      }
    }
    else  //inak chyba v parametroch
    {
      result->ecode = ECLWRONG;
    }
  }
  else  // pøíli¹ mnoho parametrov
  {
    result->ecode = ECLMANY;
  }

}

/**
 * Funkcia checkNumber skontroluje èi zadané èíslo v re»azci je
 * v poriadku a zároveò prevedie toto èíslo na typ int.
 * Algoritmus prevedie cifru z char na int a vynásobí ju 10 xkrát
 * podµa toho na akej pozícii sa nachádza.
 * @param cnumber Re»azcová hodnota èísla N.
 * @param number Hodnota "odkaz" do ktorej sa prekonvertuje re»azec èísla N
 * pri práci s touto hodnotou som pou¾il datový typ long long int
 * pre neskôr¹ie porovnanie s UINT_MAX.
 * @return Vracia true(1) pokiaµ je v¹etko v poriadku inak false(0).
 */
int checkNumber(char cnumber[],long long int *number)
{
  long long int num = 0;                    // pomocná premenná
  int j = strlen(cnumber);                  // poèet cifier èísla

  // algoritmus na prevod èísla z re»azca do int
  for (unsigned int i = 0; i < strlen(cnumber); i++)
  {
    // ak je znak v rozpätí od 0 po 9
    if((cnumber[i] >= '0') && (cnumber[i] <= '9'))
    {
      // konvertovanie èísla
      num = (int)(cnumber[i]) - C_ZERO;
      for(int k = 0; k < j - 1; k++)
      {
        num = num * 10;
      }

      j = j - 1;        // zní¾enie násobenie o cifru
      *number += num;   // prièítanie èísla do výsledku
    }
    else
    {
      // Chyba v parametru N
      return 0;
    }
  }
  return 1;
}

/**
 * Funkcia comp komprimuje zadaný re»azec podµa zadaného parametra N.
 * Pomocou cyklu while sa naèítavajú znaky do premennej c a pola kým nie je plné prièom
 * sa poèíta poèet znakov v poli. Potom sa do premmenej c naèítavajú
 * ïaµ¹ie znaky a porovnávajú sa s poµom, prièom sa poèíta poèet rovnajúcich
 * sa znakov a poèet blokov. Tento znak sa nakoniec priradí do pola. Pri
 * tejto úlohe som vyu¾il kruhový buffer ktorý indexuje dopredu ale aj
 * spätne ak sa potrebujeme vráti» k predchádzajúcim znakom. Kruhový buffer
 * vyu¾iva posun indexu pomocou modula{(i + 1) % size kde i je index) Ak sa zadá
 * neplatný znak napríklad èíslo ktoré podµa zadania nie je prípustné v komprimácii
 * vypí¹e sa chyba.
 * @param result odkaz na ¹truktúru obsahujúcu èíslo N z argumenta
 * @return vráti úspe¹nos» komprimácie 0-úspech 1-neúspech
 */
int comp(TParams *result)
{
  char *field;            // pole znakov vyu¾ívajúci algoritmus pre komprimáciu
  int c;                  // znak do ktorého sa naèítava vstup
  unsigned int i = 0;     // pomocná premenná urèujúca index pola
  unsigned int k = 0;     // pomocná premenná urèujúca poèet rovanjúcich sa znakov
  int n = 1;              // pomocná premenná urèujúca poèet rovnajúcich sa blokov
  unsigned int count = 0; // pomocná premenná urèujúca poèet znakov v poli

  // dynamická alokácia
  if ((field = (char *)malloc((sizeof(char) * result->N))) == NULL)
  {
    printECode(EMALLOC);
    return 1;
  }

  // algoritmus pre komprimáciu
  while((c=getchar()))
  {
    if(isdigit(c)) // ak je neplatný znak
    {
      free(field);
      printECode(EGETCHAR);
      return 1;
    }
    if(c > (char)(C_ASCIMAX))   // ak je na vstupe neplatný znak
    {
      free(field);
      printECode(EGETCHAR);
      return 1;
    }
    if(count < result->N)    // ak nie je pole zaplnené, zaplní sa pole
    {
      count++;
      field[i] = c;
      i = (i + 1) % result->N;
    }
    else                    // ak je pole zaplnené
    {
      if(field[i] == c)       // ak sa polo¾ka pola rovná s naèítaným znakom
      {
        field[i] = c;
        k++;
        if(result->N == k)
        {
          n++;
          k = 0;
        }
      }
      else if(n > 1)            // ak sa polo¾ka nerovná a n > 1
      {
        if(n <= C_MAX_BLCKS)                  //ak je poèet blokov men¹í ako 9
        {
          // vytlaèenie znaku a nastavenie b pre posunutie sa dozadu
          putchar(n + C_ZERO);
          int b = i;

          // posunutie sa dozadu v poli
          for(unsigned int a = 0; a < k; a++)
          {
           if(b == 0)
           {
              b = result->N - 1;
           }
           else
           {
              b = (b - 1) % result->N;
           }
          }

          // prejdenie pola a vytlaèenie na výstup
          for(unsigned int a = 0; a < result->N; a++)
          {
            putchar(field[b]);
            b = (b + 1) % result->N;
          }
        }
        else                      // ak je poèet blokov väè¹í ako 9 => rozdelenie na úseky
        {
          // rozdelenie na poèet blokov a zostatok
          int pom1 = n / C_MAX_BLCKS;
          int pom2 = n % C_MAX_BLCKS;

          // nastavenie indexu
          int c = i;

          // výpis poètu blokov - 9max poèet
          for(int a = 0; a < pom1; a++)
          {
            // výpis znaku '9'
            putchar(C_NINE);
            for(unsigned int b = 0; b < result->N; b++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
          if(pom2 > 1)  // ak je zostatok blokov väè¹í ako 1
          {
            // výpis
            putchar(pom2 + C_ZERO);
            for(unsigned int a = 0; a < result->N; a++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
          else          // inak vypí¹ iba znak
          {
            for (unsigned int a = 0; a < result->N; a++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
        }
        // nastavenie pre nové prvky
        count = k + 1;
        field[i] = c;
        k = 0;
        n = 1;
      }
      else          // inak vypí¹ prvky pola a priraim nový prvok
      {
          int b = i;
          // posunutie sa dozadu v poli
          for(unsigned int a = 0; a < k; a++)
          {
           if(b == 0)
           {
              b = result->N - 1;
           }
           else
           {
              b = (b - 1) % result->N;
           }
          }

          // vypísanie prvkov
          for(unsigned int a = 0; a < k + 1; a++)
          {
            putchar(field[b]);
            b = (b + 1) % result->N;
          }

          // nastavenie
          k = 0;
          field[i] = c;

      }

      // posunutie indexu pre ïaµ¹í znak
      i = (i + 1) % result->N;
    }
    if(c == EOF)      // ak je znak eof tak sa ukonèi cyklus
    {
      if(count > 1)                       // ak sa cyklus skonèil predèasne, poèet znakov je väè¹í ako 1
      {
        // posunutie sa v poli dozadu
        int b = i;
        for(unsigned int a = 0; a < count; a++)
        {
          if(b == 0)
          {
            b = result->N - 1;
          }
          else
          {
            b = (b - 1) % result->N;
          }
        }

        // prejdenie cyklu
        for(unsigned int a = 0; a < count - 1; a++)
        {
          if(!(field[b] == EOF))
          {
            putchar(field[b]);
          }
          b = (b + 1) % result->N;
        }
      }
      //putchar(c);
      // preru¹enie cyklu while, teda èítania znakov
      break;
    }
  }

  // uvolnenie pamäte
  free(field);
  return 0;
}

/**
 * Funkcia decomp dekomprimuje zadaný re»azec podµa zadaného parametra N.
 * Pokiaµ zadaný re»azec nespåòa podmienky. Teda napríklad nasledujú dve èísla
 * po sebe, program vypí¹e chýbovú správu a ukonèí sa.
 * Algoritmus tejto funkcie je v celku jednoduchý. Pokiaµ je na vstupe nejaké
 * èíslo naèíta sa podµa parametra ïal¹í znak/znaky. Ak je na vstupe iba znak bez
 * predchádzajúceho èísla vytlaèí sa tento znak.
 * @param result odkaz na ¹truktúru obsahujúcu èíslo N z argumenta
 * @return vráti úspe¹nos» komprimácie 0-úspech 1-neúspech
 */
int decomp(TParams *result)
{
  char *field_1;            // pole do ktorého sa pridávajú znaky
  int c;                    // premenná do ktorej sa naèíta znak
  int num = 0;              // premenná pre èíslo ktoré sa naèíta pri vstupe
  int k = 0;                // pomocná premenná urèujúca èi je na vstupe èíslo
  unsigned int i = 0;       // pomocná premenná pre pole

  // alokácia pamäti
  if ((field_1 = (char *)malloc((sizeof(char) * result->N))) == NULL)
  {
    printECode(EMALLOC);
    return 1;
  }

  // algoritmus
  while((c = getchar()) != EOF)  //pokiaµ nie je EOF
  {
    if((c == '0') || (c == '1'))                      // ak je na vstupe èíslo 1 alebo 0 = > chyba
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if(c > (char)C_ASCIMAX)   // ak je na vstupe neplatný znak
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if(((c>= '0' && c <= '9') && k == 1))        // ak je na vstupe èíslo a predtým bolo èíslo
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if((c > '1' && c <= '9') && k == 0)          // ak je na vstupe èíslo tak priraï do premennej num
    {
      num = c - C_ZERO;
      k = 1;
    }
    else if (k == 0)                                  // ak je na vstupe znak a predtým nebolo èíslo
    {
      putchar(c);
    }
    else if (k == 1)                                  // ak je na vstupe znak a predtým bolo èíslo
    {
      field_1[i] = c;
      i++;
      if(i == result->N)                           // ak je poèet znakov rovnajúci sa poètu èísla N
      {
        for(int j = 0; j < num; j++)                  // prejdi poèet blokov
        {
          for(unsigned int a = 0;a < result->N; a++)               // vytlaè znaky
          {
            putchar(field_1[a]);
          }
        }
        i = 0;  // premennu i daj na 0
        k = 0;  // premennu k daj na 0
      }
    }
  }
  if(i > 0) //ak je cyklus predèasne ukonèený
  {
    free(field_1);
    printECode(EFEND);
    return 1;
  }

  // uvolnenie pamäte
  free(field_1);
  return 0;
}

/* koniec súboru proj1.c */
