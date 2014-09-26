/*
 * S�bor    :   proj2.c
 * D�tum    :   5.11.2010
 * Autor    :   Michal Luk��, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Itera�n� v�po�ty, projekt �. 2 pre predmet IZP v
 *              zimnom semestri 2010/11 na VUT FIT.
 * Popis    :   Tento program vznikol ako projekt pre predmet z�klady
 *              programovania na VUT FIT. Program po��ta pomocou itera�n�ch
 *              v�po�tov zadan� matematick� a �tatistick� funkcie - hyperbolick�
 *              tangens, obecn� logaritmus, v�en� aritmetick� priemer a v�en�
 *              kvadratick� priemer. Program sa sp���a so zadan�mi parametrami.
 *              Funkcie hyperbolick� tangens a obecn� logaritmus program po��ta pomocou
 *              takzvan�ch taylorov�ch radov. V�po�et sa ukon�� ak absolutna hodnota
 *              dvoch po sebe id�cich prvkov je men�ia ako epsilon.
 *              �tatistick� funkcie vyu��vaj� jednoduch� v�po�et pod�a vzorca.
 * P.riadkov:   723
 * TODO     :   -
 */

/****************************** Hlavi�kov� s�bory *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

/************** Glob�lne premenn�, kon�tanty, makra, �trukt�ry... ***************/
// Kon�tanty
const int C_ZERO = 48;                         // hodnota ascii '0'
const double C_ACURR = 0.1;                    // hodnota pre vypocet presnosti epsilon
const double C_INTERVAL = 2.0;                 // hodnota kedy sa bude logaritmus rozdelova� na men�ie �asti
const double C_NLOGTEN = 2.302585092994045684; // hodnota prirodzen�ho logaritmu 10, ln(10)
const double C_TANGHLIM = 15.0;                // hodnota resp. limita od ktorej je tangh nadob�da iba 1
const int DBL_DIG = 15;		                     // hodnota pre max po�et cifier double
const int C_TEN = 10;                          // hodnota pou��van� pri konverzii re�azca na ��slo

// K�dy ch�b programu
enum tecodes
{
  EOK = 0,        /* Bez chyby.                                  */
  ECLWRONG = 1,   /* Chybn� pr�kazov� riadok.                    */
  ECLVOID = 2,    /* Chybn� pr�kazov� riadok - �iadny parameter. */
  ECLLOGA = 3,    /* Chybn� pr�kazov� riadok - z�klad logaritmu. */
  ECLSIG = 4,     /* Chybn� pr�kazov� riadok - sigdig            */
};

// Stavov� k�dy programu spracovan� pod�a parametra
enum tstates
{
  CHELP,          /* N�poveda.                      */
  CTANGH,         /* Hyperbolick� tangens.          */
  CLOGAX,         /* Obecn� logaritmus.             */
  CWAVERAGE,      /* V�en� aritmetick� priemer.    */
  CWQAVERAGE,     /* V�en� kvadratick� priemer.    */
};

// Chybov� hl�senia odpovedaj�ce chybov�m k�dom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nNebol zadany ziadny parameter prikazoveho riadku!\n",/* ECLVOID  */
  "\nChybny prikazovy riadok - zly zaklad logaritmu\n",   /* ECLLOGA  */
  "\nChybny prikazovy riadok - zly pocet plat. cislic\n", /* ECLSIG   */
};

// Spr�va n�povedy
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
    "           kde sigdig je presnos� zadan� ako po�et platn�ch cifier.\n"
    "           kde a je zaklad obecneho logaritmu.\n";

// �trukt�ra obsahuj�ca hodnoty parametrov z pr�kazovej riadky
typedef struct params
{
  double epsilon;   /* Vypo��tan� presnos� pre epsilon.                 */
  double loga;      /* Z�klad logaritmu.                                */
  int ecode;        /* Chybov� k�d programu, odpoved� v��tu tecodes.    */
  int state;        /* Stavov� k�d programu, odpoved� v��tu tstates.    */
} TParams;

// �trukt�ra obsahuj�ca hodnoty pre v�po�et aritmetick�ho a kvadratick�ho priemer
typedef struct averages
{
    double numerator;       /* Hodnota �itate�a   */
    double denumerator;     /* Hodnota menovate�a */
} TAverage;

/****************************** prototypy funkci� **********************************/
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
 * Funkcia printhelp vytla�� n�povedu a ukon�i program.
 * T�to funkcia je volan� hlavnou funkciou main.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytla�� hl�senie odpovedaj�ce chybov�mu k�du pomocou
 * v��tov�ho typu a pole re�azcov ECODEMSG.
 * @param code k�d chyby programu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Funkcia toEpsilon prevedie ��slo sigdig zadan�ho v pr�kazovom
 * riadku na ��slo epsilon, teda presnos� v�po�tu. Vypo�et presnosti,
 * vyh�dza zo vz�ahu (0.1)^sigdig. Funkcia je volan� funkciou getParams.
 * @param sigdig ur�uje po�et platn�ch cifier.
 * @return vypo��tan� hodnota epsilon
 */
double toEpsilon(int sigdig)
{
  double epsilon = C_ACURR;
  if (sigdig > DBL_DIG)
  {
    sigdig = DBL_DIG;
  }


  // cyklus n�sob� epsilon ��slom 0.1
  for (int a = 0; a < sigdig ; a++)
  {
    epsilon = epsilon * C_ACURR;
  }

  return epsilon;
}

/**
 * Funkcia getParams je volan� hlavnou funkciou main. T�to
 * funkcia spracuje argumenty pr�kazov�ho riadku. Prevedie ��sla
 * v re�azcovej podobe do podoby ��selnej. Funkcia vol� �a��ie funkcie
 * na kontrolu a prevod parametrov. Na za�iatku funkcie sa inicializuje �trukt�ra
 * do ktorej s� pod�a parametra pred�van� jednotliv� polo�ky.
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
    .epsilon = 0,
    .loga = 0,
    .ecode = EOK,
    .state = CTANGH,
  };

  int sigdig = 0; // pomocn� premenn� pre presnos�

  if(argc == 1)   // ak sa nezad� �iadny parameter
  {
    result.ecode = ECLVOID;
  }
  else if (argc == 2) // ak je jeden parameter
  {
    if(strcmp("-h", argv[1]) == 0)          // ak je parameter -h nastav n�povedu
    {
      result.state = CHELP;
    }
    else if(strcmp("--wam", argv[1]) == 0)  // ak je parameter --wam nastav aritmetick� priemer
    {
      result.state = CWAVERAGE;
    }
    else if(strcmp("--wqm", argv[1]) == 0)  // ak je parameter --wqm nastav kvadratick� priemer
    {
      result.state = CWQAVERAGE;
    }
    else                                    // inak nastav chybu
    {
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 3) // ak s� dva parametre
  {
    if(strcmp("--tanh",argv[1]) == 0)       // ak je --tanh
    {
      if(toInt(argv[2],&sigdig) && sigdig > 0)              // a ak je ��slo z parametru v poriadku, nastav epsilon
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
  else if (argc == 4) // ak s� parametre tri
  {
    if(strcmp("--logax",argv[1]) == 0) // ak je parameter --logax
    {
      if(toInt(argv[2],&sigdig) && sigdig > 0) // a ak je ��slo z parametru v poriadku
      {
        result.epsilon = toEpsilon(sigdig);
        if(((result.loga = strtod(argv[3], NULL)) != 0.0)) // ak je zle zadan� z�klad logaritmu
        {
          if (((result.loga > 0.0) && (result.loga < 1.0)) || (result.loga > 1.0)) // ak je z�klad v platnom intervale
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
  else  // p��li� mnoho parametrov
  {
    result.ecode = ECLWRONG;
  }

  return result; // vr�ti v�sledok
}

/**
 * Funkcia toInt prekonvertuje re�azec-pole znakov na datov� typ
 * integer. Algoritmus kontroloju �i dan� ��slo je zadan� v spr�vnom
 * tvare.
 * @param number re�azec predstavuj�ci ��slo.
 * @param accuracy odkaz na presnos�, do tejto premennej sa prekonvertuje ��slo.
 * @return hodnota ur�uj�ca �i bola konverzia �spe�n�.
 */
int toInt(char number[],int *accuracy)
{
  int j = strlen(number);                  // po�et cifier ��sla
  int l = j;                               // po�et cifier ��sla
  int num = 0;                             // pomocn� premenn�

  // algoritmus na prevod ��sla z re�azca do int
  for (int i = 0; i < l; i++)
  {
    // ak je znak v rozp�t� od 0 po 9
    if((number[i] >= '0') && (number[i] <= '9'))
    {
      // konvertovanie ��sla
      num = (int)(number[i]) - C_ZERO;
      for(int k = 0; k < j - 1; k++)
      {
        num = num * C_TEN;
      }

      j = j - 1;          // zn�enie n�sobenie o cifru
      *accuracy += num;   // pri��tanie ��sla do v�sledku
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
 * Funkcia sinh vypo��ta s�nus pomocou taylorovho radu
 * ==================================================
 *                 x^3     x^5     x^7     x^9
 *  sinh(x) = x + ----- + ----- + ----- + ----- + ...
 *                  3!      5!      7!      9!
 * ==================================================
 * @param param je parameter �pecifikuj�ci v�po�et
 * @param number je ��slo pre ktor� m�m po��ta�
 * @return vr�ti v�sledok s�nusu
 */
double sinhx(TParams *param, double number)
{
  // nastavenie nov�ch hodn�t pre sinh(x)
  double numerator = number;                                // hodnota pre �itate�
  double denumerator = 1;                                   // hodnota pre menovate�
  double actual = number;                                   // aktu�lna hodnota
  double previous = 0;                                      // predch�dzaj�ca hodnota
  double iteration = 1;                                     // po�iato�n� iter�cia
  double result = 0;                                        // v�sledok
  double num_2 = number * number;                           // mocnina

  //algoritmus pre sinh(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                                      // nastavenie predch�dzaj�cej hodnoty
    result += actual;                                       // pri��tanie do v�sledku

    iteration += 2;                                         // nastavenie pre vz�ah zo vzorca
    numerator *= num_2;                                     // v�po�et �itate�a
    denumerator = denumerator * (iteration) *(iteration -1);// v�po�et menovate�a
    actual = numerator/denumerator;                         // pridanie do aktu�lnej hodnoty
  }

  return (result += numerator/denumerator);                 // pri��tanie posledn�ho radu a vr�tenie v�sledku
}

/**
 * Funkcia cosh vypo��ta cos�nus pomocou taylorovho radu
 * ==================================================
 *                 x^2     x^4     x^6     x^8
 *  cosh(x) = 1 + ----- + ----- + ----- + ----- + ...
 *                  2!      4!      6!      8!
 * ==================================================
 * @param parameter pre v�po�et
 * @param number hodnota ktor� po��tame
 * @return v�sledok kos�nusu
 */
double coshx(TParams *param, double number)
{
  double num_2 = number * number;                           // hodnota number*number pre zr�chlenie v�po�tu algoritum
  double numerator = 1;                                     // nastavenie �itate�a
  double denumerator = 1;                                   // nastavenie menovate�a
  double result = 0;                                        // hodnota pre v�sledok
  double actual = 1;                                        // hodnota pre aktu�lny v�po�et, nastaven� pre cosh
  double previous = 0;                                      // hodnota pre predch�dzaj�ci v�po�et
  int iteration = 0;                                        // iter�cia pre v�po�et pod�a vzorca

  // algoritmus pre cosh(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                                      // nastavenie predch�dzaj�cej hodnoty
    result += actual;                                       // pri��tanie do v�sledku

    iteration += 2;                                         // nastavenie pre vz�aho zo vzorca
    numerator = numerator * num_2;                          // v�po�et �itate�a
    denumerator = denumerator * iteration * (iteration - 1);// v�po�et menovate�a
    actual = numerator/denumerator;                         // pridanie do aktu�lnej hodnoty
  }

  return (result += numerator/denumerator);                 // pri��tanie posledn�ho radu
}

/**
 * Funkcia tangens vypo��ta hyperbolick� tangens z parametra number.
 * Funkcia vyu��va v��ahu tanh(x) =sinh(x)/cosh(x). V�po�et je zaobstar�ny
 * cez taylorov rad sinh(x) a cosh(x).
 * Vz�ah pre hyperbolick� tangens a taylorov rad h.sinusu a h.cosinusu:
 * =========================
 *                sinh(x)
 *  tanh(x) =    ---------
 *                cosh(x)
 * =========================
 * @param param odkaz na �trukt�ru v ktorej je ulo�en� epsilon teda presnos�
 * @param number hodnota pre ktor� sa hyperbolick� tanges po��ta
 * @return hodnota vypo��tan�ho hyperbolick�ho tangensu
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
  // vr�tenie v�sledku
  return sinhx(param, number)/coshx(param, number);
}

/**
 * Funkcia countLnTen sl��i na zmenu intervalu ak je number > 2
 * m�m napr�klad ��slo ln(15.3) toto ��slo m��em rozlo�i� na ln(0.153) + ln(100).
 * preto uprav�m number na 0.153 a zv��im hodnotu pre v�skyt ln(10).
 * 2 * ln (10) je to ist� �o ln(10)^2.
 * @param number cislo pre ktory sa prirodzeny logaritmus pocita
 * @return pocet vyskytov ln(10)
 */
int countLnTen(double *number)
{
  int lncount = 0;
  if (*number >= C_INTERVAL)
  {
    while(*number >= C_INTERVAL)                      // vyn�sob� number s 0.1 a zv��i v�skyt ln(10)
    {
      *number = *number * C_ACURR;
      lncount++;
    }
  }
  return lncount;
}

/**
 * Funkcia logarithm vypo��ta logaritmus.
 * Pri�om prirodzen� logaritmus ln sa vypo��ta pomocou iter�cie
 * tayloroveho radu. Vzorec na taylorov rad je uk�zan� ni��ie.
 * Hodnotu ktor� chceme po��ta� mus�me upravi� na zadan� interval.
 * Prirodzen� logaritmus mo�eme definova� ako:
 * ===============================================================
 *            (x - 1)   (x - 1)^2   (x - 1)^3
 *   ln(x) =  ------- - --------- + --------- + .... ;  0 < x <= 2
 *               1          2           3
 * ===============================================================
 * @param param odkaz na �trukt�ru obsahuj�cu presnos�
 * @param number ��slo z ktor�ho sa
 * @return vypo��tan� hodnota obecn�ho logaritmu
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

  int lncount = countLnTen(&number);                // hodnota po�et v�skytov ln(10)
  double result1;                                   // hodnota pre v�sledok
  result1 = (lncount * C_NLOGTEN);                  // pri��tanie hodnoty pre x*ln(10)

  // premenn�
  double numminus = number - 1;                     // nastavenie hodnoty numminus pre skr�tenie v�po�tu
  double numerator = number - 1;                    // hodnota pre �itate�
  double denumerator = 1;
  double previous = 0;                              // predch�dzaj�ca hodnota v�po�tu
  double actual = numerator/denumerator;            // aktu�lna hodnota v�po�tu
  double sign = -1.0;                               // hodnota znamienka

  // algoritmus na v�po�et ln(x)
  while(fabs(actual - previous) >= param->epsilon)
  {
    previous = actual;                              // nastavenie predch�dzaj�cej hodnoty
    result1 += actual;                              // pridanie do v�sledku

    denumerator++;                                  // zv��enie menovate�a
    numerator *= numminus * sign;                   // vyn�sobenie �itate�a pod�a vzorca aj so znamienkom
    actual = (numerator/denumerator);               // nastavenia aktu�lnej hodnoty
  }
  result1 += numerator/denumerator;                 // pri��tanie posledn�ho v�sledku po skon�en�
  // vr�tenie v�sledku
  return result1;
}

/**
 * Funkcia mathm je funkcia v ktorej sa na��tavaj� hodnoty a
 * a posiela na v�stup v�sledok obecn�ho logaritmu/hyperbolck� tangens
 * z volan�ch funkci�.
 * @param param �trukt�ra obsahuj�ca parametry z pr�kazov�ho riadku
 * @param state stav pre v�po�et obecn�ho logaritmu/hyperbolick�ho tangensu
 */
void mathm(TParams param,int state)
{
  double number = 0;  // hodnota pre ��slo
  int succesfull = 1;  // premenn� ur�uj�ca �spe�nos� na��tania ��sla

  // cyklus pre na��tanie funkcie
  while((succesfull = scanf("%lf",&number)) != EOF)
  {
    if(succesfull == 1)
    {
      if(state == CLOGAX)
        printf("%.10e\n",logresult(param, number));
      else
        printf("%.10e\n",tangens(&param,number));
    }
    else // ak je neplatn� vstup tak nan
    {
      scanf("%*s");
      printf("%.10e\n",NAN);
    }
  }
}

/**
 * Funkcia logresult vol� funkciu logaritmus a zo vz�ahu
 * lnx/lna vypo��ta a vr�ti v�sledn� hodnotu.
 * ======================
 *              ln(x)
 *  logax(x) = -------
 *              ln(a)
 * ======================
 * @param param �trukt�ra ukladaj�ca parametre pre v�po�et
 * @param number ��slo ktor� chceme vypo��ta�
 * @param return v�sledok logaritmu
 */
double logresult(TParams param,double number)
{
    return logarithm(&param,number)/logarithm(&param,param.loga);
}

/**
 * Funkcia waverage v�po��ta v�en� aritmetick� priemer zo zadan�ch
 * hodn�t pod�a vzorca uveden�ho ni��ie. Funkcia vyu��va pred�vanie
 * �trukt�ry odkazom a preto v�sledok po��ta priebe�ne.
 *  ================================
 *
 *      x1h1 + x2h2 +x3h3 + ... xnhn
 *  x = ----------------------------
 *      h1  +  h2 +  h3 + ...   + hn
 *
 *  ================================
 * @param average odkaz na �trukt�ru obsahuj�ca v�po�et
 *        priemeru = �itate�,menovate�,v�sledok
 * @param weight je v�ha prvku pre v�po�et
 * @param value hodnota prvku pre v�po�et
 * @return hodnota v�en�ho aritmetick�ho priemeru
 */
double waverage(TAverage * average,double weight,double value)
{
  if(weight < 0)  //ak je v�ha z�porn�
  {
    average->denumerator = NAN;
    return NAN;
  }
  // v�po�et
  average->numerator += weight*value;
  average->denumerator += weight;

  return average->numerator/average->denumerator;
}

/**
 * Funkcia qaverage v�po��ta v�en� kvadratick� priemer zo zadan�ch
 * hodn�t pod�a vzorca uveden�ho ni��ie. Funkcia vyu��va pred�vanie
 * �trukt�ry odkazom a preto v�sledok po��ta priebe�ne.
 * ===================================================
 *        x1*x1*h1 + x2*x2*h2 + ... + xn*xn*hn
 * x = ( -------------------------------------- )^1/2
 *        h1 + h2 + ..................... + hn
 * ===================================================
 * @param average odkaz na �trukt�ru obsahuj�ca v�po�et
 *        priemeru = �itate�,menovate�,v�sledok
 * @param weight je v�ha prvku pre v�po�et
 * @param value hodnota prvku pre v�po�et
 * @return hodnota v�en�ho kvadratick�ho priemeru
 */
double qaverage(TAverage * average, double weight, double value)
{
  // z�porn� vaha nem��e by� pri priemere
  if(weight < 0)
  {
    average->denumerator = NAN;
    return NAN;
  }
  // v�po�et a vr�tenie v�sledku
  average->numerator += value*value*weight;
  average->denumerator += weight;

  return sqrt(average->numerator/average->denumerator);
}

/**
 * Funkcia avrg sl��i na zad�vanie hodn�t pre v�en�
 * kvadratick�/aritmetick� priemer. Na za�iatku funkcie sa vytvor�
 * �trukt�ra a potom sa pomocou cyklu while+scanf zad�vaj�
 * hodnoty pre funkciu qaverage.
 * @param state stav pre aritmeticky/kvadraticky priemer
 */
void avrg(int state)
{
  // inicializ�cia �trukt�ry
  TAverage aver =
  {
      .numerator = 0,
      .denumerator = 0,
  };

  double weight = 0;    // premenn� pre v�hu
  double value = 0;     // premenn� pre hodnotu
  int succesfull = 0;   // premenn� ur�uj�ca �spe�nos� na��tania znaku
  int count = 0;        // premenn� ur�uj�ca �i sa m� prid�va� do weight alebo value

  // cyklus pre na��tanie hodn�t
  while((succesfull = scanf("%lf",&weight)) != EOF)
  {
    if (count == 0) // ak je prv� hodnota teda value
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
    else            // ak je druh� hodnota teda weight
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
  // ak je pred�asne ukon�en� cyklus
  if (count == 1)
  {
      printf("%.10e\n",NAN);
  }
}

/**
 * Funkcia main najprv vol� funkciu getParams, ktor� vr�ti
 * �trukt�ru obsahuj�cu parametry z pr�kazov�ho riadku. Pod�a
 * toho �i je v�etko v poriadku a pod�a parametra z pr�kazov�ho
 * riadku vol� funkcie(printhelp,tangh,logax,avrg,qavrg).
 * @param argc po�et parametrov z pr�kazov�ho riadku
 * @param argv jednotliv� polo�ky z pr�kazov�ho riadku
 * @return �spe�nos� programu
 */
int main(int argc, char *argv[])
{
  // z�skanie parametrov z pr�kazov�ho riadka
  TParams result = getParams(argc, argv);

  if(result.ecode > EOK)      // ak je chyba
  {
    printECode(result.ecode);
    return EXIT_FAILURE;
  }
  else                        // inak zavolaj po�adovan� funkciu
  {
    switch(result.state)
    {
      case CHELP:
              printHelp();            // pomocn�k
              break;
      case CTANGH:
              mathm(result,CTANGH);   // hyperbolick� tangens
              break;
      case CLOGAX:
              mathm(result,CLOGAX);   // obecn� logaritmus
              break;
      case CWAVERAGE:
              avrg(CWAVERAGE);        // v�en� aritmetick� priemer
              break;
      case CWQAVERAGE:
              avrg(CWQAVERAGE);       // v�en� kvadratick� priemer
              break;
      default:break;
    }
  }

  // ukon�enie programu
  return EXIT_SUCCESS;
}
// koniec s�bora proj2.c
