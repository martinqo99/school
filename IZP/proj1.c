/*
 * S�bor    :   proj1.c
 * D�tum    :   26.10.2010 21:00
 * Autor    :   Michal Luk��, xlukac05@fit.vutbr.cz
 * Projekt  :   Jednoduch� kompresia textu, projekt �. 1 pre predmet IZP v
 *              zimnom semestri 2010/11 na VUT FIT.
 * Popis    :   Tento program je prv� projekt pre predmet Z�klady programovania
 *              na VUT FIT v Brne. Pri tomto programe som vyu�il kostru programu
 *              zo str�nky projektu pre IZP, pri�om som t�to kostru trochu obmenil
 *              a upravil. �lohou programu je komprim�cia a dekomprim�cia textu pod�a
 *              zadan�ho parametra z pr�kazovej riadky/terimn�lu. Po�et blokov pri
 *              komprim�cii a dekomprim�cii nesmie presiahnu� ��slo 9. Pri komprim�cii
 *              som vyu�il pohyb v poli pomocou kruhov�ho buffera {(i + 1) % velkost
 *              pola, kde i je aktu�lny index}. V�hodou tak�hoto pohybu v poli je �e sa pri
 *              prejden� posledn�ho prvku vr�time na za�iatok pola. Av�ak m��e nasta� pr�pad
 *              kedy sa nebude rovna� cel� blok, tak�to situ�ciu som zabezpe�il opa�n�m
 *              "kruhov�m bufferom" kedy sa vr�time o x prvkov dozadu. Pri dekomprim�ciu
 *              som vyu�il jednoduch� algoritmus v ktorom som zis�oval �i dan� znak
 *              je ��slo alebo nie a pod�a toho po��tal a posielal na v�stup znaky.
 *              Ve�kos� parametra N je dan� typom unsigned int.Prete�enie tohto ��sla som
 *              kontroloval pomocou pomocnej premennej d�tov�ho typu long long int.
 * P.riadkov:   607
 * TODO     :   -
 */

/************************** Hlavi�kov� s�bory ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

/************** Glob�lne premenn�, kon�tanty, makra, �trukt�ry... ********/
// Kon�tanty
const int C_ZERO = 48;                     /* Hodnota ascii pre znak '0' */
const int C_EOF = 26;                      /* Mo�n� hodnota "EOF"        */
const int C_MAX_BLCKS = 9;                 /* Maxim�lny po�et blokov     */
const int C_ASCIMAX = 127;                 /* Konec prvej polovice ASCII */
const int C_EOF2 = -1;                     /* Hodnota pre znak EOF       */
const char C_NINE = '9';                   /* Znak 9                     */

// K�dy ch�b programu
enum tecodes
{
  EOK = 0,        /* Bez chyby.                                  */
  ECLWRONG = 1,   /* Chybn� pr�kazov� riadok.                    */
  ECLVOID = 2,    /* Chybn� pr�kazov� riadok - �iadny parameter. */
  ECLNUM = 3,     /* Zle zadan� hodnota N!                       */
  EOVRLFW = 4,    /* Chyba prete�enia hodnoty N!                 */
  EGETCHAR = 5,   /* Chyba pri zad�van� znakov.                  */
  EMALLOC = 6,    /* Chyba pri alok�cii pam�te.                  */
  EFEND = 7,      /* Cyklus bol pred�asne ukon�en�.              */
  ECLMANY = 8,    /* Pr�li� mnoho parametrov.                    */
};

// Stavov� k�dy programu spracovan� pod�a parametra
enum tstates
{
  CHELP,          /* N�poveda.      */
  CCOMP,          /* Komprimova�.   */
  CDECOMP,        /* Dekomprimova�. */
};

// Chybov� hl�senia odpovedaj�ce chybov�m k�dom tecodes.
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

// Spr�va n�povedy
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

// �trukt�ra obsahuj�ca hodnoty parametrov z pr�kazovej riadky
typedef struct params
{
  unsigned int N;   /* Hodnota N z pr�kazovej riadky.                 */
  int ecode;        /* Chybov� k�d programu, odpoved� v��tu tecodes.  */
  int state;        /* Stavov� k�d programu, odpoved� v��tu tstates.  */
} TParams;

/************************ Prototypy funkci� **********************************/
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
 * Hlavn� funkcia main()
 * Funkcia main neobsahuje �iadny v�po�et. Pod�a zadan�ch parametrov
 * vol� prisl�chaj�ce funkcie. Pokia� nies� parametre v poriadku zavol� sa funkcia
 * printECode ktor� vytla�� chybov� stav. Ak je parameter -h zavol� sa funkcia
 * printHelp ktor� vytla�� inform�cie-pomocn�ka programu. Pokia� je parameter
 * -c N alebo -d N kde N je cel� ��slo zavol� sa bu� funkcia comp pre komprim�ciu
 * textu alebo funkcia decomp pre dekomprim�ciu textu.
 * @param argc - po�et parametrov.
 * @param argv - konkr�tne parametry.
 * @return - '�spe�nos�' programu
 */
int main(int argc, char *argv[])
{
  // inicializ�cia �trukt�ry ktor� sa pred� odkazom
  TParams params =
  {
    .N = 0,
    .ecode = EOK,
    .state = CCOMP,
  };
  int error = 0; // premenn� pre chybu

  // zistenie argumentov
  getParams(&params,argc,argv);

  // zachytenie chyb v parametroch,kontrola stavov == volanie pr�slu�n�ch funkci�
  if (params.ecode > EOK)
  {
    error = printECode(params.ecode);   // volanie chybov�ho hl�senia
  }
  else
  {
    switch(params.state)
    {
      case CHELP:                       // volanie pomocn�ka
            printHelp();
            break;
      case CCOMP:
            error = comp(&params);      // volanie komprim�cie
            break;
      case CDECOMP:
            error = decomp(&params);    // volanie dekomprim�cie
            break;
      default:
            break;
    }
  }

  // ak funkcia comp, decomp, printecode vr�ti nejak� chybu
  if(error == 1)
  {
      return EXIT_FAILURE;
  }

  // �spe�n� ukon�enie programu
  return EXIT_SUCCESS;
}

/**
 * Funkcia printhelp vytla�� n�povedu a ukon�i program.
 * T�to funkcia je volan� hlavnou funkciou main.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytla�� hl�senie odpovedaj�ce chybov�mu k�du pomocou
 * v��tov�ho typu a pole re�azcov ECODEMSG
 * @param code k�d chyby programu
 * @return vr�ti chybov� stav
 */
int printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
  return 1;
}

/**
 * Funkcia getParams spracuje argumenty pr�kazov�ho riadku a vrati ich odkazom v �trukt�ry result.
 * Pokia� je form�t argumentov chybn�, ukon�� program s chybov�m k�dom.
 * Pri pr�ci s hodnotou ��sla N som vyu�il pri pomocnej premennej
 * datov� typ long long int, pre porovnanie �i dan� hodnota je v��ia ako
 * datov� typ unsigned int. Taktie� je skontrolovan� �i hodnota ��sla N je
 * v��ia ako 0, pr�padne �i je zadan� spr�vny po�et parametrov...
 * @param result �trukt�ra reprezentovan� ako odkaz uchov�vaj�ca hodnoty parametrov
 * @param argc Po�et argumentov.
 * @param argv Pole textov�ch �e�azcov s argumentami.
 */
void getParams(TParams * result,int argc, char *argv[])
{

  if(argc == 1) // ak sa nezad� �iadny parameter
  {
    result->ecode = ECLVOID;
  }
  else if (argc == 2 && (strcmp("-h", argv[1]) == 0)) // ak je parameter -h nastav stav na n�povedy
  {
    result->state = CHELP;
  }
  else if (argc == 3) // dva parametry
  {
    long long int number = 0;   // pomocn� premenn� na kontrolu hodnoty ��sla N

    if(strcmp("-c",argv[1]) == 0)  // ak je param. pre komprim�ciu
    {
      if(checkNumber(argv[2],&number) && number >= 1) // ak je ��slo n v spr�vnom tvare
      {
        if(number <= UINT_MAX)                          // ak je ��slo n  v rozsahu unsigned int
        {
            result->N = number;
        }
        else                                            // inak vyp� chybu prete�enia unsigned int
        {
          result->ecode = EOVRLFW;
        }
      }
      else                          // inak vyp� chybu zle zadan�ho ��sla
      {
        result->ecode = ECLNUM;
      }
    }
    else if(strcmp("-d",argv[1]) == 0) //ak je dekomprimacia
    {
      if(checkNumber(argv[2], &number) && number >= 1) //ak je ��slo n v spr�vnom tvare
      {
        if(number <= UINT_MAX)           //ak je �islo n v rozsahu unsigned int
        {
          result->N = number;
          result->state = CDECOMP;
        }
        else    // inak vyp� chybu prete�enia unsigned int
        {
          result->ecode = EOVRLFW;
        }
      }
      else // inak vyp� chybu zle zadan�ho ��sla N
      {
        result->ecode = ECLNUM;
      }
    }
    else  //inak chyba v parametroch
    {
      result->ecode = ECLWRONG;
    }
  }
  else  // p��li� mnoho parametrov
  {
    result->ecode = ECLMANY;
  }

}

/**
 * Funkcia checkNumber skontroluje �i zadan� ��slo v re�azci je
 * v poriadku a z�rove� prevedie toto ��slo na typ int.
 * Algoritmus prevedie cifru z char na int a vyn�sob� ju 10 xkr�t
 * pod�a toho na akej poz�cii sa nach�dza.
 * @param cnumber Re�azcov� hodnota ��sla N.
 * @param number Hodnota "odkaz" do ktorej sa prekonvertuje re�azec ��sla N
 * pri pr�ci s touto hodnotou som pou�il datov� typ long long int
 * pre nesk�r�ie porovnanie s UINT_MAX.
 * @return Vracia true(1) pokia� je v�etko v poriadku inak false(0).
 */
int checkNumber(char cnumber[],long long int *number)
{
  long long int num = 0;                    // pomocn� premenn�
  int j = strlen(cnumber);                  // po�et cifier ��sla

  // algoritmus na prevod ��sla z re�azca do int
  for (unsigned int i = 0; i < strlen(cnumber); i++)
  {
    // ak je znak v rozp�t� od 0 po 9
    if((cnumber[i] >= '0') && (cnumber[i] <= '9'))
    {
      // konvertovanie ��sla
      num = (int)(cnumber[i]) - C_ZERO;
      for(int k = 0; k < j - 1; k++)
      {
        num = num * 10;
      }

      j = j - 1;        // zn�enie n�sobenie o cifru
      *number += num;   // pri��tanie ��sla do v�sledku
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
 * Funkcia comp komprimuje zadan� re�azec pod�a zadan�ho parametra N.
 * Pomocou cyklu while sa na��tavaj� znaky do premennej c a pola k�m nie je pln� pri�om
 * sa po��ta po�et znakov v poli. Potom sa do premmenej c na��tavaj�
 * �a��ie znaky a porovn�vaj� sa s po�om, pri�om sa po��ta po�et rovnaj�cich
 * sa znakov a po�et blokov. Tento znak sa nakoniec prirad� do pola. Pri
 * tejto �lohe som vyu�il kruhov� buffer ktor� indexuje dopredu ale aj
 * sp�tne ak sa potrebujeme vr�ti� k predch�dzaj�cim znakom. Kruhov� buffer
 * vyu�iva posun indexu pomocou modula{(i + 1) % size kde i je index) Ak sa zad�
 * neplatn� znak napr�klad ��slo ktor� pod�a zadania nie je pr�pustn� v komprim�cii
 * vyp�e sa chyba.
 * @param result odkaz na �trukt�ru obsahuj�cu ��slo N z argumenta
 * @return vr�ti �spe�nos� komprim�cie 0-�spech 1-ne�spech
 */
int comp(TParams *result)
{
  char *field;            // pole znakov vyu��vaj�ci algoritmus pre komprim�ciu
  int c;                  // znak do ktor�ho sa na��tava vstup
  unsigned int i = 0;     // pomocn� premenn� ur�uj�ca index pola
  unsigned int k = 0;     // pomocn� premenn� ur�uj�ca po�et rovanj�cich sa znakov
  int n = 1;              // pomocn� premenn� ur�uj�ca po�et rovnaj�cich sa blokov
  unsigned int count = 0; // pomocn� premenn� ur�uj�ca po�et znakov v poli

  // dynamick� alok�cia
  if ((field = (char *)malloc((sizeof(char) * result->N))) == NULL)
  {
    printECode(EMALLOC);
    return 1;
  }

  // algoritmus pre komprim�ciu
  while((c=getchar()))
  {
    if(isdigit(c)) // ak je neplatn� znak
    {
      free(field);
      printECode(EGETCHAR);
      return 1;
    }
    if(c > (char)(C_ASCIMAX))   // ak je na vstupe neplatn� znak
    {
      free(field);
      printECode(EGETCHAR);
      return 1;
    }
    if(count < result->N)    // ak nie je pole zaplnen�, zapln� sa pole
    {
      count++;
      field[i] = c;
      i = (i + 1) % result->N;
    }
    else                    // ak je pole zaplnen�
    {
      if(field[i] == c)       // ak sa polo�ka pola rovn� s na��tan�m znakom
      {
        field[i] = c;
        k++;
        if(result->N == k)
        {
          n++;
          k = 0;
        }
      }
      else if(n > 1)            // ak sa polo�ka nerovn� a n > 1
      {
        if(n <= C_MAX_BLCKS)                  //ak je po�et blokov men�� ako 9
        {
          // vytla�enie znaku a nastavenie b pre posunutie sa dozadu
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

          // prejdenie pola a vytla�enie na v�stup
          for(unsigned int a = 0; a < result->N; a++)
          {
            putchar(field[b]);
            b = (b + 1) % result->N;
          }
        }
        else                      // ak je po�et blokov v��� ako 9 => rozdelenie na �seky
        {
          // rozdelenie na po�et blokov a zostatok
          int pom1 = n / C_MAX_BLCKS;
          int pom2 = n % C_MAX_BLCKS;

          // nastavenie indexu
          int c = i;

          // v�pis po�tu blokov - 9max po�et
          for(int a = 0; a < pom1; a++)
          {
            // v�pis znaku '9'
            putchar(C_NINE);
            for(unsigned int b = 0; b < result->N; b++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
          if(pom2 > 1)  // ak je zostatok blokov v��� ako 1
          {
            // v�pis
            putchar(pom2 + C_ZERO);
            for(unsigned int a = 0; a < result->N; a++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
          else          // inak vyp� iba znak
          {
            for (unsigned int a = 0; a < result->N; a++)
            {
              putchar(field[c]);
              c = (c + 1) % result->N;
            }
          }
        }
        // nastavenie pre nov� prvky
        count = k + 1;
        field[i] = c;
        k = 0;
        n = 1;
      }
      else          // inak vyp� prvky pola a priraim nov� prvok
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

          // vyp�sanie prvkov
          for(unsigned int a = 0; a < k + 1; a++)
          {
            putchar(field[b]);
            b = (b + 1) % result->N;
          }

          // nastavenie
          k = 0;
          field[i] = c;

      }

      // posunutie indexu pre �a��� znak
      i = (i + 1) % result->N;
    }
    if(c == EOF)      // ak je znak eof tak sa ukon�i cyklus
    {
      if(count > 1)                       // ak sa cyklus skon�il pred�asne, po�et znakov je v��� ako 1
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
      // preru�enie cyklu while, teda ��tania znakov
      break;
    }
  }

  // uvolnenie pam�te
  free(field);
  return 0;
}

/**
 * Funkcia decomp dekomprimuje zadan� re�azec pod�a zadan�ho parametra N.
 * Pokia� zadan� re�azec nesp��a podmienky. Teda napr�klad nasleduj� dve ��sla
 * po sebe, program vyp�e ch�bov� spr�vu a ukon�� sa.
 * Algoritmus tejto funkcie je v celku jednoduch�. Pokia� je na vstupe nejak�
 * ��slo na��ta sa pod�a parametra �al�� znak/znaky. Ak je na vstupe iba znak bez
 * predch�dzaj�ceho ��sla vytla�� sa tento znak.
 * @param result odkaz na �trukt�ru obsahuj�cu ��slo N z argumenta
 * @return vr�ti �spe�nos� komprim�cie 0-�spech 1-ne�spech
 */
int decomp(TParams *result)
{
  char *field_1;            // pole do ktor�ho sa prid�vaj� znaky
  int c;                    // premenn� do ktorej sa na��ta znak
  int num = 0;              // premenn� pre ��slo ktor� sa na��ta pri vstupe
  int k = 0;                // pomocn� premenn� ur�uj�ca �i je na vstupe ��slo
  unsigned int i = 0;       // pomocn� premenn� pre pole

  // alok�cia pam�ti
  if ((field_1 = (char *)malloc((sizeof(char) * result->N))) == NULL)
  {
    printECode(EMALLOC);
    return 1;
  }

  // algoritmus
  while((c = getchar()) != EOF)  //pokia� nie je EOF
  {
    if((c == '0') || (c == '1'))                      // ak je na vstupe ��slo 1 alebo 0 = > chyba
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if(c > (char)C_ASCIMAX)   // ak je na vstupe neplatn� znak
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if(((c>= '0' && c <= '9') && k == 1))        // ak je na vstupe ��slo a predt�m bolo ��slo
    {
      free(field_1);
      printECode(EGETCHAR);
      return 1;
    }
    else if((c > '1' && c <= '9') && k == 0)          // ak je na vstupe ��slo tak prira� do premennej num
    {
      num = c - C_ZERO;
      k = 1;
    }
    else if (k == 0)                                  // ak je na vstupe znak a predt�m nebolo ��slo
    {
      putchar(c);
    }
    else if (k == 1)                                  // ak je na vstupe znak a predt�m bolo ��slo
    {
      field_1[i] = c;
      i++;
      if(i == result->N)                           // ak je po�et znakov rovnaj�ci sa po�tu ��sla N
      {
        for(int j = 0; j < num; j++)                  // prejdi po�et blokov
        {
          for(unsigned int a = 0;a < result->N; a++)               // vytla� znaky
          {
            putchar(field_1[a]);
          }
        }
        i = 0;  // premennu i daj na 0
        k = 0;  // premennu k daj na 0
      }
    }
  }
  if(i > 0) //ak je cyklus pred�asne ukon�en�
  {
    free(field_1);
    printECode(EFEND);
    return 1;
  }

  // uvolnenie pam�te
  free(field_1);
  return 0;
}

/* koniec s�boru proj1.c */
