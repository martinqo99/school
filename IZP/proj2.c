/*
 * Súbor    :   proj2.c
 * Dátum    :   5.11.2010
 * Autor    :   Michal Lukáè, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Iteraèné výpoèty, projekt è. 2 pre predmet IZP v
 *              zimnom semestri 2010/11 na VUT FIT.
 * Popis    :   Tento program vznikol ako projekt pre predmet základy
 *              programovania na VUT FIT. Program poèíta pomocou iteraèných
 *              výpoètov zadané matematické a ¹tatistické funkcie - hyperbolický
 *              tangens, obecný logaritmus, vá¾ený aritmetický priemer a vá¾ený
 *              kvadratický priemer. Program sa spú¹»a so zadanými parametrami.
 *              Funkcie hyperbolický tangens a obecný logaritmus program poèíta pomocou
 *              takzvaných taylorových radov. Výpoèet sa ukonèí ak absolutna hodnota
 *              dvoch po sebe idúcich prvkov je men¹ia ako epsilon.
 *              ©tatistické funkcie vyu¾ívajú jednoduchý výpoèet podµa vzorca.
 * P.riadkov:   723
 * TODO     :   -
 */

/****************************** Hlavièkové súbory *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

/************** Globálne premenné, kon¹tanty, makra, ¹truktúry... ***************/
// Kon¹tanty
const int C_ZERO = 48;                         // hodnota ascii '0'
const double C_ACURR = 0.1;                    // hodnota pre vypocet presnosti epsilon
const double C_INTERVAL = 2.0;                 // hodnota kedy sa bude logaritmus rozdelova» na men¹ie èasti
const double C_NLOGTEN = 2.302585092994045684; // hodnota prirodzeného logaritmu 10, ln(10)
const double C_TANGHLIM = 15.0;                // hodnota resp. limita od ktorej je tangh nadobúda iba 1
const int DBL_DIG = 15;		                     // hodnota pre max poèet cifier double
const int C_TEN = 10;                          // hodnota pou¾ívaná pri konverzii re»azca na èíslo

// Kódy chýb programu
enum tecodes
{
  EOK = 0,        /* Bez chyby.                                  */
  ECLWRONG = 1,   /* Chybný príkazový riadok.                    */
  ECLVOID = 2,    /* Chybný príkazový riadok - ¾iadny parameter. */
  ECLLOGA = 3,    /* Chybný príkazový riadok - základ logaritmu. */
  ECLSIG = 4,     /* Chybný príkazový riadok - sigdig            */
};

// Stavové kódy programu spracované podµa parametra
enum tstates
{
  CHELP,          /* Nápoveda.                      */
  CTANGH,         /* Hyperbolický tangens.          */
  CLOGAX,         /* Obecný logaritmus.             */
  CWAVERAGE,      /* Vá¾ený aritmetický priemer.    */
  CWQAVERAGE,     /* Vá¾ený kvadratický priemer.    */
};

// Chybové hlásenia odpovedajúce chybovým kódom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nNebol zadany ziadny parameter prikazoveho riadku!\n",/* ECLVOID  */
  "\nChybny prikazovy riadok - zly zaklad logaritmu\n",   /* ECLLOGA  */
  "\nChybny prikazovy riadok - zly pocet plat. cislic\n", /* ECLSIG   */
};

// Správa nápovedy
const char *HELPMSG =
    "Program  :   Iteracne vypocty.\n"
    "Autor    :   Michal Lukac, xlukac05@stud.fit.vutbr.cz\n"
    "Info     :   Tento program vznikol ako projekt pre predmet Zaklady\n"
    "             programovania na VUT FIT. Ulohou programu je pocitat zadane\n"
    "             funkcie(hyperbolicky tangens, obecny logaritmus) na pozadovanu\n"
    "             presnost, ktora je zadana ako parameter prikazoveho riadku.\n"
    "             Taktiez su implementovane funkcie pre pocitanie vazeneho aritmetickeho\n"
    "             priemeru a vazeneho kvadratickeho priemeru. Uzivatel zadava vstupy\n"
    "             a program priebezne vypisuje vysledky na standartny vystup.\n"
    "Parametre: proj1 -h\n"
    "           proj1 --tanh sigdig\n"
    "           proj1 --logax sigdig a\n"
    "           proj1 --wam\n"
    "           proj1 --wqm\n"
    "Popis parametrov:\n"
    "           -h  parameter pre vypis napovedy.\n"
    "           --tanh sigdig  parameter pre vypocet hyperbolickeho tangensu.\n"
    "           --logax sigdig a  parameter pre vypocet obecneho logaritmu.\n"
    "           --wam  parameter pre vypocet vazeneho aritmetickeho priemeru.\n"
    "           --wqm  parameter pre vypocet vazeneho kvadratickeho priemeru.\n"
    "           kde sigdig je presnos» zadaná ako poèet platných cifier.\n"
    "           kde a je zaklad obecneho logaritmu.\n";

// ©truktúra obsahujúca hodnoty parametrov z príkazovej riadky
typedef struct params
{
  double epsilon;   /* Vypoèítaná presnos» pre epsilon.                 */
  double loga;      /* Základ logaritmu.                                */
  int ecode;        /* Chybový kód programu, odpovedá výètu tecodes.    */
  int state;        /* Stavový kód programu, odpovedá výètu tstates.    */
} TParams;

// ©truktúra obsahujúca hodnoty pre výpoèet aritmetického a kvadratického priemer
typedef struct averages
{
    double numerator;       /* Hodnota èitateµa   */
    double denumerator;     /* Hodnota menovateµa */
} TAverage;

/****************************** prototypy funkcií **********************************/
//////////////////////////////////////////////////////////////////////////
void printHelp(void);                                                   //
void printECode(int ecode);                                             //
TParams getParams(int argc, char *argv[]);                              //
int toInt(char number[], int *acc);                                     //
void mathm(TParams param,int state);                                    //
double tangens(TParams * param, double number);                         //
double logresult(TParams param, double number);                         //
double logarithm(TParams *param, double number);                        //
double qaverage(TAverage *aver, double weight,double value);            //
double waverage(TAverage *aver, double weight,double value);            //
void avrg(int state);                                                   //
double toEpsilon(int sigdig);                                           //
int countLnTen(double *number);                                         //
double coshx(TParams *param, double number);                            //
double sinhx(TParams *param, double number);                            //
//////////////////////////////////////////////////////////////////////////

/********************************** Funkcie *****************************************/
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
 * výètového typu a pole re»azcov ECODEMSG.
 * @param code kód chyby programu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Funkcia toEpsilon prevedie èíslo sigdig zadaného v príkazovom
 * riadku na èíslo epsilon, teda presnos» výpoètu. Vypoèet presnosti,
 * vyhádza zo vz»ahu (0.1)^sigdig. Funkcia je volaná funkciou getParams.
 * @param sigdig urèuje poèet platných cifier.
 * @return vypoèítaná hodnota epsilon
 */
double toEpsilon(int sigdig)
{
  double epsilon = C_ACURR;
  if (sigdig > DBL_DIG)
  {
    sigdig = DBL_DIG;
  }


  // cyklus násobí epsilon èíslom 0.1
  for (int a = 0; a < sigdig ; a++)
  {
    epsilon = epsilon * C_ACURR;
  }

  return epsilon;
}

/**
 * Funkcia getParams je volaná hlavnou funkciou main. Táto
 * funkcia spracuje argumenty príkazového riadku. Prevedie èísla
 * v re»azcovej podobe do podoby èíselnej. Funkcia volá ïaµ¹ie funkcie
 * na kontrolu a prevod parametrov. Na zaèiatku funkcie sa inicializuje ¹truktúra
 * do ktorej sú podµa parametra predávané jednotlivé polo¾ky.
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
    .epsilon = 0,
    .loga = 0,
    .ecode = EOK,
    .state = CTANGH,
  };

  int sigdig = 0; // pomocná premenná pre presnos»

  if(argc == 1)   // ak sa nezadá ¾iadny parameter
  {
    result.ecode = ECLVOID;
  }
  else if (argc == 2) // ak je jeden parameter
  {
    if(strcmp("-h", argv[1]) == 0)          // ak je parameter -h nastav nápovedu
    {
      result.state = CHELP;
    }
    else if(strcmp("--wam", argv[1]) == 0)  // ak je parameter --wam nastav aritmetický priemer
    {
      result.state = CWAVERAGE;
    }
    else if(strcmp("--wqm", argv[1]) == 0)  // ak je parameter --wqm nastav kvadratický priemer
    {
      result.state = CWQAVERAGE;
    }
    else                                    // inak nastav chybu
    {
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 3) // ak sú dva parametre
  {
    if(strcmp("--tanh",argv[1]) == 0)       // ak je --tanh
    {
      if(toInt(argv[2],&sigdig) && sigdig > 0)              // a ak je èíslo z parametru v poriadku, nastav epsilon
      {
        result.epsilon = toEpsilon(sigdig);
      }
      else                                    // inak chyba
      {
        result.ecode = ECLSIG;
      }
    }
    else //inak chyba v parametroch
    {
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 4) // ak sú parametre tri
  {
    if(strcmp("--logax",argv[1]) == 0) // ak je parameter --logax
    {
      if(toInt(argv[2],&sigdig) && sigdig > 0) // a ak je èíslo z parametru v poriadku
      {
        result.epsilon = toEpsilon(sigdig);
        if(((result.loga = strtod(argv[3], NULL)) != 0.0)) // ak je zle zadaný základ logaritmu
        {
          if (((result.loga > 0.0) && (result.loga < 1.0)) || (result.loga > 1.0)) // ak je základ v platnom intervale
          {
            result.state = CLOGAX;
          }
          else // chyba
          {
            result.ecode = ECLLOGA;
          }
        }
        else // chyba
        {
          result.ecode = ECLLOGA;
        }
      }
      else // chyba
      {
        result.ecode = ECLSIG;
      }
    }
    else // chyba
    {
      result.ecode = ECLWRONG;
    }
  }
  else  // pøíli¹ mnoho parametrov
  {
    result.ecode = ECLWRONG;
  }

  return result; // vráti výsledok
}

/**
 * Funkcia toInt prekonvertuje re»azec-pole znakov na datový typ
 * integer. Algoritmus kontroloju èi dané èíslo je zadané v správnom
 * tvare.
 * @param number re»azec predstavujúci èíslo.
 * @param accuracy odkaz na presnos», do tejto premennej sa prekonvertuje èíslo.
 * @return hodnota urèujúca èi bola konverzia úspe¹ná.
 */
int toInt(char number[],int *accuracy)
{
  int j = strlen(number);                  // poèet cifier èísla
  int l = j;                               // poèet cifier èísla
  int num = 0;                             // pomocná premenná

  // algoritmus na prevod èísla z re»azca do int
  for (int i = 0; i < l; i++)
  {
    // ak je znak v rozpätí od 0 po 9
    if((number[i] >= '0') && (number[i] <= '9'))
    {
      // konvertovanie èísla
      num = (int)(number[i]) - C_ZERO;
      for(int k = 0; k < j - 1; k++)
      {
        num = num * C_TEN;
      }

      j = j - 1;          // zní¾enie násobenie o cifru
      *accuracy += num;   // prièítanie èísla do výsledku
    }
    else
    {
      //Chyba v parametru N
      return 0;
    }
  }

  return 1;
}

/**
 * Funkcia sinh vypoèíta sínus pomocou taylorovho radu
 * ==================================================
 *                 x^3     x^5     x^7     x^9
 *  sinh(x) = x + ----- + ----- + ----- + ----- + ...
 *                  3!      5!      7!      9!
 * ==================================================
 * @param param je parameter ¹pecifikujúci výpoèet
 * @param number je èíslo pre ktoré mám poèíta»
 * @return vráti výsledok sínusu
 */
double sinhx(TParams *param, double number)
{
  // nastavenie nových hodnôt pre sinh(x)
  double numerator = number;                                // hodnota pre èitateµ
  double denumerator = 1;                                   // hodnota pre menovateµ
  double actual = number;                                   // aktuálna hodnota
  double previous = 0;                                      // predchádzajúca hodnota
  double iteration = 1;                                     // poèiatoèná iterácia
  double result = 0;                                        // výsledok
  double num_2 = number * number;                           // mocnina

  //algoritmus pre sinh(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                                      // nastavenie predchádzajúcej hodnoty
    result += actual;                                       // prièítanie do výsledku

    iteration += 2;                                         // nastavenie pre vz»ah zo vzorca
    numerator *= num_2;                                     // výpoèet èitateµa
    denumerator = denumerator * (iteration) *(iteration -1);// výpoèet menovateµa
    actual = numerator/denumerator;                         // pridanie do aktuálnej hodnoty
  }

  return (result += numerator/denumerator);                 // prièítanie posledného radu a vrátenie výsledku
}

/**
 * Funkcia cosh vypoèíta cosínus pomocou taylorovho radu
 * ==================================================
 *                 x^2     x^4     x^6     x^8
 *  cosh(x) = 1 + ----- + ----- + ----- + ----- + ...
 *                  2!      4!      6!      8!
 * ==================================================
 * @param parameter pre výpoèet
 * @param number hodnota ktorú poèítame
 * @return výsledok kosínusu
 */
double coshx(TParams *param, double number)
{
  double num_2 = number * number;                           // hodnota number*number pre zrýchlenie výpoètu algoritum
  double numerator = 1;                                     // nastavenie èitateµa
  double denumerator = 1;                                   // nastavenie menovateµa
  double result = 0;                                        // hodnota pre výsledok
  double actual = 1;                                        // hodnota pre aktuálny výpoèet, nastavený pre cosh
  double previous = 0;                                      // hodnota pre predchádzajúci výpoèet
  int iteration = 0;                                        // iterácia pre výpoèet podµa vzorca

  // algoritmus pre cosh(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                                      // nastavenie predchádzajúcej hodnoty
    result += actual;                                       // prièítanie do výsledku

    iteration += 2;                                         // nastavenie pre vz»aho zo vzorca
    numerator = numerator * num_2;                          // výpoèet èitateµa
    denumerator = denumerator * iteration * (iteration - 1);// výpoèet menovateµa
    actual = numerator/denumerator;                         // pridanie do aktuálnej hodnoty
  }

  return (result += numerator/denumerator);                 // prièítanie posledného radu
}

/**
 * Funkcia tangens vypoèíta hyperbolický tangens z parametra number.
 * Funkcia vyu¾íva v¾»ahu tanh(x) =sinh(x)/cosh(x). Výpoèet je zaobstarány
 * cez taylorov rad sinh(x) a cosh(x).
 * Vz»ah pre hyperbolický tangens a taylorov rad h.sinusu a h.cosinusu:
 * =========================
 *                sinh(x)
 *  tanh(x) =    ---------
 *                cosh(x)
 * =========================
 * @param param odkaz na ¹truktúru v ktorej je ulo¾ený epsilon teda presnos»
 * @param number hodnota pre ktorú sa hyperbolický tanges poèíta
 * @return hodnota vypoèítaného hyperbolického tangensu
 */
double tangens(TParams *param,double number)
{
  // podmienky
  if (number > C_TANGHLIM)
  {
    return 1;
  }
  if (number < -C_TANGHLIM)
  {
    return -1;
  }
  // vrátenie výsledku
  return sinhx(param, number)/coshx(param, number);
}

/**
 * Funkcia countLnTen slú¾i na zmenu intervalu ak je number > 2
 * mám napríklad èíslo ln(15.3) toto èíslo mô¾em rozlo¾i» na ln(0.153) + ln(100).
 * preto upravím number na 0.153 a zvý¹im hodnotu pre výskyt ln(10).
 * 2 * ln (10) je to isté èo ln(10)^2.
 * @param number cislo pre ktory sa prirodzeny logaritmus pocita
 * @return pocet vyskytov ln(10)
 */
int countLnTen(double *number)
{
  int lncount = 0;
  if (*number >= C_INTERVAL)
  {
    while(*number >= C_INTERVAL)                      // vynásobí number s 0.1 a zvý¹i výskyt ln(10)
    {
      *number = *number * C_ACURR;
      lncount++;
    }
  }
  return lncount;
}

/**
 * Funkcia logarithm vypoèíta logaritmus.
 * Prièom prirodzený logaritmus ln sa vypoèíta pomocou iterácie
 * tayloroveho radu. Vzorec na taylorov rad je ukázaný ni¾¹ie.
 * Hodnotu ktorú chceme poèíta» musíme upravi» na zadaný interval.
 * Prirodzený logaritmus mo¾eme definova» ako:
 * ===============================================================
 *            (x - 1)   (x - 1)^2   (x - 1)^3
 *   ln(x) =  ------- - --------- + --------- + .... ;  0 < x <= 2
 *               1          2           3
 * ===============================================================
 * @param param odkaz na ¹truktúru obsahujúcu presnos»
 * @param number èíslo z ktorého sa
 * @return vypoèítaná hodnota obecného logaritmu
 */
double logarithm(TParams * param,double number)
{
  // podmienky
  if(number == INFINITY)
  {
    return INFINITY;
  }
  else if(number == 0.0)
  {
    return -INFINITY;
  }
  else if(number < 0.0)
  {
    return NAN;
  }
  else if(isnan(number))
  {
    return NAN;
  }

  int lncount = countLnTen(&number);                // hodnota poèet výskytov ln(10)
  double result1;                                   // hodnota pre výsledok
  result1 = (lncount * C_NLOGTEN);                  // prièítanie hodnoty pre x*ln(10)

  // premenné
  double numminus = number - 1;                     // nastavenie hodnoty numminus pre skrátenie výpoètu
  double numerator = number - 1;                    // hodnota pre èitateµ
  double denumerator = 1;
  double previous = 0;                              // predchádzajúca hodnota výpoètu
  double actual = numerator/denumerator;            // aktuálna hodnota výpoètu
  double sign = -1.0;                               // hodnota znamienka

  // algoritmus na výpoèet ln(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                              // nastavenie predchádzajúcej hodnoty
    result1 += actual;                              // pridanie do výsledku

    denumerator++;                                  // zvý¹enie menovateµa
    numerator *= numminus * sign;                   // vynásobenie èitateµa podµa vzorca aj so znamienkom
    actual = (numerator/denumerator);               // nastavenia aktuálnej hodnoty
  }
  result1 += numerator/denumerator;                 // prièítanie posledného výsledku po skonèení
  // vrátenie výsledku
  return result1;
}

/**
 * Funkcia mathm je funkcia v ktorej sa naèítavajú hodnoty a
 * a posiela na výstup výsledok obecného logaritmu/hyperbolcký tangens
 * z volaných funkcií.
 * @param param ¹truktúra obsahujúca parametry z príkazového riadku
 * @param state stav pre výpoèet obecného logaritmu/hyperbolického tangensu
 */
void mathm(TParams param,int state)
{
  double number = 0;  // hodnota pre èíslo
  int succesfull = 1;  // premenná urèujúca úspe¹nos» naèítania èísla

  // cyklus pre naèítanie funkcie
  while((succesfull = scanf("%lf",&number)) != EOF)
  {
    if(succesfull == 1)
    {
      if(state == CLOGAX)
        printf("%.10e\n",logresult(param, number));
      else
        printf("%.10e\n",tangens(&param,number));
    }
    else // ak je neplatný vstup tak nan
    {
      scanf("%*s");
      printf("%.10e\n",NAN);
    }
  }
}

/**
 * Funkcia logresult volá funkciu logaritmus a zo vz»ahu
 * lnx/lna vypoèíta a vráti výslednú hodnotu.
 * ======================
 *              ln(x)
 *  logax(x) = -------
 *              ln(a)
 * ======================
 * @param param ¹truktúra ukladajúca parametre pre výpoèet
 * @param number èíslo ktoré chceme vypoèíta»
 * @param return výsledok logaritmu
 */
double logresult(TParams param,double number)
{
    return logarithm(&param,number)/logarithm(&param,param.loga);
}

/**
 * Funkcia waverage výpoèíta vá¾ený aritmetický priemer zo zadaných
 * hodnôt podµa vzorca uvedeného ni¾¹ie. Funkcia vyu¾íva predávanie
 * ¹truktúry odkazom a preto výsledok poèíta priebe¾ne.
 *  ================================
 *
 *      x1h1 + x2h2 +x3h3 + ... xnhn
 *  x = ----------------------------
 *      h1  +  h2 +  h3 + ...   + hn
 *
 *  ================================
 * @param average odkaz na ¹truktúru obsahujúca výpoèet
 *        priemeru = èitateµ,menovateµ,výsledok
 * @param weight je váha prvku pre výpoèet
 * @param value hodnota prvku pre výpoèet
 * @return hodnota vá¾eného aritmetického priemeru
 */
double waverage(TAverage * average,double weight,double value)
{
  if(weight < 0)  //ak je váha záporná
  {
    average->denumerator = NAN;
    return NAN;
  }
  // výpoèet
  average->numerator += weight*value;
  average->denumerator += weight;

  return average->numerator/average->denumerator;
}

/**
 * Funkcia qaverage výpoèíta vá¾ený kvadratický priemer zo zadaných
 * hodnôt podµa vzorca uvedeného ni¾¹ie. Funkcia vyu¾íva predávanie
 * ¹truktúry odkazom a preto výsledok poèíta priebe¾ne.
 * ===================================================
 *        x1*x1*h1 + x2*x2*h2 + ... + xn*xn*hn
 * x = ( -------------------------------------- )^1/2
 *        h1 + h2 + ..................... + hn
 * ===================================================
 * @param average odkaz na ¹truktúru obsahujúca výpoèet
 *        priemeru = èitateµ,menovateµ,výsledok
 * @param weight je váha prvku pre výpoèet
 * @param value hodnota prvku pre výpoèet
 * @return hodnota vá¾eného kvadratického priemeru
 */
double qaverage(TAverage * average, double weight, double value)
{
  // záporná vaha nemô¾e by» pri priemere
  if(weight < 0)
  {
    average->denumerator = NAN;
    return NAN;
  }
  // výpoèet a vrátenie výsledku
  average->numerator += value*value*weight;
  average->denumerator += weight;

  return sqrt(average->numerator/average->denumerator);
}

/**
 * Funkcia avrg slú¾i na zadávanie hodnôt pre vá¾ený
 * kvadratický/aritmetický priemer. Na zaèiatku funkcie sa vytvorí
 * ¹truktúra a potom sa pomocou cyklu while+scanf zadávajú
 * hodnoty pre funkciu qaverage.
 * @param state stav pre aritmeticky/kvadraticky priemer
 */
void avrg(int state)
{
  // inicializácia ¹truktúry
  TAverage aver =
  {
      .numerator = 0,
      .denumerator = 0,
  };

  double weight = 0;    // premenná pre váhu
  double value = 0;     // premenná pre hodnotu
  int succesfull = 0;   // premenná urèujúca úspe¹nos» naèítania znaku
  int count = 0;        // premenná urèujúca èi sa má pridáva» do weight alebo value

  // cyklus pre naèítanie hodnôt
  while((succesfull = scanf("%lf",&weight)) != EOF)
  {
    if (count == 0) // ak je prvá hodnota teda value
    {
      if (succesfull == 1)  // ak je hodnota v poriadku
      {
        value = weight;
        count++;
      }
      else                  // inak NAN
      {
        scanf("%*s");
        value = NAN;
        count++;
      }
    }
    else            // ak je druhá hodnota teda weight
    {
      if (succesfull == 1)  // ak je hodnota v poriadku
      {
        count = 0;
        if(state == CWQAVERAGE)
          printf("%.10e\n", qaverage(&aver,weight,value));
        else
          printf("%.10e\n", waverage(&aver,weight,value));
      }
      else                  // inak NAN
      {
        scanf("%*s");
        count = 0;
        aver.denumerator = NAN;
        printf("%.10e\n",NAN);
      }
    }
  }
  // ak je predèasne ukonèený cyklus
  if (count == 1)
  {
      printf("%.10e\n",NAN);
  }
}

/**
 * Funkcia main najprv volá funkciu getParams, ktorá vráti
 * ¹truktúru obsahujúcu parametry z príkazového riadku. Podµa
 * toho èi je v¹etko v poriadku a podµa parametra z príkazového
 * riadku volá funkcie(printhelp,tangh,logax,avrg,qavrg).
 * @param argc poèet parametrov z príkazového riadku
 * @param argv jednotlivé polo¾ky z príkazového riadku
 * @return úspe¹nos» programu
 */
int main(int argc, char *argv[])
{
  // získanie parametrov z príkazového riadka
  TParams result = getParams(argc, argv);

  if(result.ecode > EOK)      // ak je chyba
  {
    printECode(result.ecode);
    return EXIT_FAILURE;
  }
  else                        // inak zavolaj po¾adovanú funkciu
  {
    switch(result.state)
    {
      case CHELP:
              printHelp();            // pomocník
              break;
      case CTANGH:
              mathm(result,CTANGH);   // hyperbolický tangens
              break;
      case CLOGAX:
              mathm(result,CLOGAX);   // obecný logaritmus
              break;
      case CWAVERAGE:
              avrg(CWAVERAGE);        // vá¾ený aritmetický priemer
              break;
      case CWQAVERAGE:
              avrg(CWQAVERAGE);       // vá¾ený kvadratický priemer
              break;
      default:break;
    }
  }

  // ukonèenie programu
  return EXIT_SUCCESS;
}
// koniec súbora proj2.c
