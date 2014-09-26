/*
 * S�bor    :   proj3.c
 * D�tum    :   15.11.2010
 * Autor    :   Michal Luk��, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Maticov� oper�cie.
 * Popis    :   Program je tret�m projektom do predmetu z�klady programovania na
 *              VUT FIT. Program sl��i na v�po�et z�kladn�ch maticov�ch a vektorov�ch
 *              oper�cii. Program na��ta vektor-maticu zo s�bora. Program pracuje s
 *              dynamickou alok�ciou a bol odtestovan� valgrindom. Implementovan� su
 *              v�etky povinn� oper�cie. A to s��in mat�c, s��in mat�c A*B*A, s��et dvoch
 *              vektorov, skal�rny s��in dvoch vektorov, test, osemsmerovka a h�adanie
 *              bubl�n.
 * P.riadkov:   1026
 * TODO     :   -
 *
 */

/*********************************** Hlavi�kov� s�bory ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************** Glob�lne premenn�, kon�tanty, makra, �trukt�ry... ********************/
// K�d� ch�b programu
enum tecodes
{
  EOK = 0,              /* Bez chyby.																		*/
  ECLWRONG = 1,         /* Chybn� pr�kazov� riadok.											*/
  ECLVOID = 2,          /* Chybn� pr�kazov� riadok - �iadny parameter.	*/
  EFILEO = 3,           /* Chyba pri otv�ranie� s�boru.									*/
  EMALLOC = 4,          /* Chyba pri alokovan� pam�te.									*/
  EFILEL = 5,           /* Chyba pri na��tan� zo s�boru.								*/
  ENUMB = 6,	          /* Zle zadan� matica / vektor.									*/
	EVMAT = 7,						/* Zle zadan� vektor mat�c.											*/
};

// Stavov� k�dy programu, spracovanie pod�a parametra
enum tstates
{
  CHELP,                /* N�poveda.																		*/
  CTEST,                /* Testovac� v�stup.														*/
  CSUMV,                /* Vektorov� s��et.															*/
  CMULV,                /* Skal�rny s��in vektorov.                    	*/
  CMULM,                /* N�sobenie dvoch mat�c.                     	*/
  CABAM,                /* N�sobenie mat�c A * B * A.                 	*/
  CCROS,		            /* Osemsmerovka.               			           	*/
  CBUBL,	              /* H�adanie bubl�n.        		     	           	*/
};

// K�dy pre typ spracovania s�boru
enum ttype
{
	CVEC = 1,				/* Stav pre vektor.		 			      */
	CMAT = 2,				/* Stav pre maticu.	  					  */
 	CVMAT = 3,			/* Stav pre vektor matic.					*/
};

// Chybov� hl�senia odpovedaj�ce chybov�m k�dom tecodes.
const char *ECODEMSG[] =
{
  "\nVsetko je v poriadku.\n",                            /* EOK      */
  "\nChybne parametry prikazoveho riadku!\n",             /* ECLWRONG */
  "\nNebol zadany ziadny parameter prikazoveho riadku!\n",/* ECLVOID  */
  "\nChyba pri otvarani-zatvarani suboru!\n",             /* EFILEO   */
  "\nChyba pri alokovani pamate!\n",                      /* EMALLOC  */
  "\nChyba pri nacitani zo suboru!\n",                    /* EFILEL   */
  "\nZle zadana matica / vektor!\n",                      /* ENUMB   	*/
	"\nZle zadany vektor matic\n",													/* EVMAT		*/
};

// Spr�va n�povedy
const char *HELPMSG =
    "Program  :   Maticov� oper�cie.\n"
    "Autor    :   Michal Lukac, xlukac05@stud.fit.vutbr.cz\n"
    "Info     :   Tento program vznikol ako projekt pre predmet Zaklady\n"
    "             programovania na VUT FIT. Ulohou programu s� maticov�\n"
    "             a vektorov� v�po�ty pop�san� v parametroch s ktor�mi sa\n"
    "             program sp���a. Implementovan� s� v�etky povinn� oper�cie.\n"
		"Parametre:	 -h --test --vadd --vscal --mmult --mexpr --eight --bubbles.\n"
    "Popis parametrov:\n"
    "           -h  parameter pre vypis napovedy.\n"
    "           --test data.txt parameter pre kontrolu vstupneho suboru\n"
    "           --vadd a.txt b.txt parameter pre scitanie dvoch vektorov\n"
    "           --vscal a.txt b.txt parameter pre skalarny sucin dvoch vektorov\n"
    "           --mmult A.txt B.txt parameter pre sucin dvoch matic\n"
    "           --mexpr A.txt B.txt parameter pre sucin matic A*B*A\n"
    "           --eight v.txt M.txt parameter pre osemsmerovku\n"
    "           --bubbles M.txt parameter pre h�adanie bubl�n";

// �trukt�ra obsahuj�ca hodnoty parametrov z pr�kazovej riadky
typedef struct params
{
  int ecode;        /* Chybov� k�d programu, odpoved� v��tu tecodes.    */
  int state;        /* Stavov� k�d programu, odpoved� v��tu tstates.    */
} TParams;

// �trukt�ra obsahuj�ca maticu
typedef struct matrix
{
  int rows;         /* Po�et riadkov matice.                    	      */
  int columns;      /* Po�et st�pcov matice.                  		      */
  int **rc;         /* Dvojn�sobn� ukazatel na int resp. matica.    	  */
} TMatrix;

// �trukt�ra obsahuj�ca vektor
typedef struct vector
{
  int count;         /* Po�et prvkov vektora.          			    	      */
  int *v;            /* Ukazatel na int resp. vektor.        	    	    */
} TVector;

// prototypy funkci�
//////////////////////////////////////////////////////////////////////////
void printHelp(void);                                                   //
void printECode(int ecode);                                             //
TParams getParams(int argc, char *argv[]);                              //
void freeMatrix(TMatrix *matrix, int rows);                             //
void freeVector(TVector *vector);                                       //
void printfMatrix(TMatrix *matrix);                                     //
void printfVector(TVector * vector);                                    //
int allocMatrix(TMatrix *matrix);                                       //
int allocVector(TVector *vector);                                       //
int readVector(TVector *vector,char *path);                             //
int readMatrix(TMatrix *matrix,char *path);                             //
int testf(char *file1);                                                 //
int sumVecMul(int state,char *file1, char *file2);                      //
void multiplyMat(TMatrix *a, TMatrix *b, TMatrix *c);                   //
int mulMatABA(int state,char *file1, char *file2);                      //
int findVector(TMatrix *matrix, TVector *vector);                       //
int crossWord(char *file1,char *file2);                                 //
void repairMatrix(TMatrix *matB,int repair, int value);                 //
int checkMatrix(TMatrix *matA);                                         //
int findBubbles(TMatrix matA, TMatrix matB);                            //
int bubbles(char *file1);                                               //
//////////////////////////////////////////////////////////////////////////

/******************************* FUNKCIE  PROGRAMU **************************************/
/**
 * Funkcia printhelp vytla�� n�povedu a ukon�� program.
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
 * Funkcia getParams spracuje argumenty pr�kazov�ho riadku do �trukt�ry.
 * @param argc po�et parametrov pr�kazov�ho riadka.
 * @param argv pole parametrov pr�kazov�ho riadka.
 * @return �truktura obsahuj�ca na��atan� parametre z arg. pr�kazov�ho riadka
 */
TParams getParams(int argc, char *argv[])
{
  // inicializ�cia �trukt�ry
  TParams result =
  {
    .ecode = ECLWRONG,
    .state = CHELP,
  };

  if(argc == 1)                                 // ak sa nezad� �iadny parameter
  {
    result.ecode = ECLVOID;
  }
  else if (argc == 2)                           // ak je jeden parameter
  {
    if(strcmp("-h", argv[1]) == 0)              // ak je parameter -h nastav n�povedu
    {
      result.ecode = EOK;
      result.state = CHELP;
    }
  }
  else if (argc == 3)                           // ak s� dva parametre
  {
    if(strcmp("--test",argv[1]) == 0)           // ak je parameter --test nastav test
    {
        result.ecode = EOK;
        result.state = CTEST;
    }
    else if(strcmp("--bubbles",argv[1]) == 0)   // ak je parameter --bubbles nastav h�adanie bubl�n
    {
        result.ecode = EOK;
        result.state = CBUBL;
    }
  }
  else if(argc == 4)                            // ak s� tri parametre
  {
    if(strcmp("--vadd",argv[1]) == 0)           // ak je parameter --vadd nastav vektorov� s��et
    {
        result.ecode = EOK;
        result.state = CSUMV;
    }
    else if(strcmp("--vscal",argv[1]) == 0)     // ak je parameter --vscal nastav vektorov� s��in
    {
        result.ecode = EOK;
        result.state = CMULV;
    }
    else if(strcmp("--mmult",argv[1]) == 0)     // ak je parameter --mmult vyn�sob dve matice
    {
        result.ecode = EOK;
        result.state = CMULM;
    }
    else if(strcmp("--mexpr",argv[1]) == 0)     // ak je parameter --mexpr tak A*B*A
    {
        result.ecode = EOK;
        result.state = CABAM;
    }
    else if(strcmp("--eight",argv[1]) == 0)     // ak je parameter --eight tak osemsmerovka
    {
        result.ecode = EOK;
        result.state = CCROS;
    }
  }
  return result; // vr�ti v�sledok
}

/**
 * Funkcia printfmatrix vyp�e cel� maticu
 * @param matrix �trukt�ra pre maticu
 */
void printfMatrix(TMatrix *matrix)
{
	if(matrix->rc != NULL)																// ak nie je NULL
	{
    printf("2\n%d %d\n",matrix->rows,matrix->columns);
    for(int a = 0; a < matrix->rows; a++)								// prejdi riadky
    {
      for(int b = 0; b < matrix->columns; b++)					// prejdi st�pce
      {
          printf("%d ", matrix->rc[a][b]);
      }
      printf("\n");
    }
	}
}

/**
 * Funkcia vyp�e polo�ky vektoru pomocou cyklu
 * @param vector �trukt�ra pre dan� vektor
 */
void printfVector(TVector * vector)
{
	if(vector->v != NULL)
	{
  	printf("1\n%d\n",vector->count);
  	for(int a = 0; a < vector->count; a++)  // prejde polo�ky vektora
    	printf("%d ",vector->v[a]);
	}
}

/**
 * Funkcia allocMatrix, alokuje dvojrozmern� pole
 * @param matrix
 * @return �spe�nos� alok�cie
 */
int allocMatrix(TMatrix *matrix)
{
	// alok�cia matice
  if((matrix->rc = (int **) malloc(matrix->rows * sizeof(int *))) ==  NULL)
  {
    return EMALLOC;
  }
  else
  {
    for(int a = 0; a < matrix->rows; a++) // prejdenie riadkov
    {
      if((matrix->rc[a] = (int *) malloc(sizeof(int)*matrix->columns))== NULL)
      {
        freeMatrix(matrix,a);             // ak sa stala chyba tu uvoln� sa matica s po�tom alokovan�ch riadkov
        return EMALLOC;
      }
    }
  }
	// vynulovanie matice
	for(int a = 0; a < matrix->rows; a++)
		for(int b = 0; b < matrix->columns; b++)
			matrix->rc[a][b] = 0;
  return EOK;
}

/**
 * Funkcia allocVector alokuje pole ulo�en� v �trukt�re vector.
 * @param vector odkaz na vektor
 * @return �spe�nos� funkcie
 */
int allocVector(TVector *vector)
{
	// alok�cia vektora
	if((vector->v = (int *) malloc(sizeof(int)*vector->count)) == NULL)
		return EMALLOC;
	return EOK;
}

/**
 * Funkcia freeMatrix uvoln� maticu, resp. dvojrozmern� pole.
 * @param matrix dan� matica
 * @param rows po�et alokovan�ch riadkov
 */
void freeMatrix(TMatrix *matrix, int rows)
{
	if(matrix->rc != NULL)
	{
		for(int a = 0; a < rows; a++)     // uvolni po riadkoch
			free(matrix->rc[a]);
		free(matrix->rc);                 // uvoln� prv� rozmer
		matrix->rc = NULL;
	}
}

/**
 * Funkcia freeVector uvoln� pole
 * @param vector odkaz na dynammick� pole
 */
void freeVector(TVector *vector)
{
	if(vector->v != NULL)
	{
		free(vector->v);                  // uvoln� vektor
		vector->v = NULL;
	}
}

/**
 * Funkcia readVector vol� funkciu na alokovanie a na��tanie vektora.
 * @param vector odkaz na vektor
 * @param file1 n�zov s�boru
 * @return �spe�nos� funkcie
 */
int readVector(TVector *vector,char *file1)
{
	int number, succes;
	FILE *fr;

	if((fr = fopen(file1,"r")) == NULL) 									// otvorenie s�bora
		return EFILEO;

	if((succes = fscanf(fr,"%d",&number)) == EOF || succes == 0)  					// pre��tanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(number != CVEC)														        // ak to neni vektor
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if((succes = fscanf(fr,"%d",&vector->count)) == EOF || succes == 0)	    // pre��tanie po�tu prvkov
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(vector->count == 0)													      // kontrola ve�kosti vektora
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return ENUMB;
	}

	if(allocVector(vector) != EOK)											  // alokovanie pola
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EMALLOC;
	}

  int a = 0; int c = 0;																	// algoritmus pre na��tanie matice
  while((succes = fscanf(fr,"%d",&c)) != EOF)
  {
    if(succes == 0)
    {
      freeVector(vector);
      if(fclose(fr)==EOF) return EFILEO;
      return ENUMB;
    }
    if(a < vector->count)
		{
			vector->v[a] = c;
      a++;
		}
    else
    {
      freeVector(vector);
      if(fclose(fr)==EOF) return EFILEO;
      return ENUMB;
    }
  }

	if(a < vector->count)																	// fin�lna podmienka
	{
		freeVector(vector);
		if(fclose(fr)==EOF) return EFILEO;
    return ENUMB;
	}
	if(fclose(fr)==EOF) return EFILEO;
	return EOK;
}

/**
 * Funkcia readMatrix vol� funkciu na alokovanie matice a napln� maticu.
 * @param matrix je �trukt�ra pre maticu
 * @param file1 n�zov s�bora 1
 * @return �spe�nos� funkcie
 */
int readMatrix(TMatrix *matrix,char *file1)
{
	int number, succes;	// premenn� pre ��slo a �spech na��tania
	FILE *fr;

	if((fr = fopen(file1,"r")) == NULL) 										// otvorenie s�bora
		return EFILEO;

	if((succes = fscanf(fr,"%d",&number)) == EOF || succes == 0)  			    // pre��tanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(number != CMAT)															        // ak to neni matica
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	// pre��tanie po�tu riadkov
	if((succes = fscanf(fr,"%d %d",&matrix->rows, &matrix->columns)) == EOF || succes < 2)
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(!(matrix->columns > 0 && matrix->rows > 0))					// kontrola rozmerov
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return ENUMB;
	}

	if(allocMatrix(matrix) != EOK)												  // alokovanie pola
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EMALLOC;
	}

  int a = 0; int b = 0; int c = 0;												// algoritmus pre na��tanie matice
  while((succes = fscanf(fr,"%d",&c)) != EOF)
  {
    if(succes == 0)
    {
       freeMatrix(matrix,matrix->rows);
       if(fclose(fr) == EOF) return EFILEO;
       return ENUMB;
    }

    if(a < matrix->rows && b < matrix->columns)
      matrix->rc[a][b] = c;
    else
    {
      freeMatrix(matrix,matrix->rows);
      if(fclose(fr) == EOF) return EFILEO;
      return ENUMB;
    }
    if(b == matrix->columns-1)
    {
      b = 0;
      a++;
    }
    else
      b++;
  }

	if(a < matrix->rows || b != 0)												// fin�lna podmienka
	{
		if(fclose(fr) == EOF) return EFILEO;
		freeMatrix(matrix,matrix->rows);
		return ENUMB;
	}

	if(fclose(fr) == EOF) return EFILEO;
	return EOK;
}

/**
 * Funkcia testf odtestuje maticu, pr�padne vektor zo s�boru
 * data.txt. Tak�to matica, vektor je dynamicky alokovan� a vyp�san�.
 * @param file1 n�zov s�bora
 * @return �spech testovania
 */
int testf(char *file1)
{
	int rvalue = EOK;   //	hodnota pre n�vratov� hodnotu
	int succes,type = 0;// hodnota pre �spech ��tania a typ s�boru
	FILE *fr;           // �trukt�ra s�boru

	if((fr = fopen(file1,"r")) == NULL)                         // otvorenie s�bora
		return EFILEL;

	if((succes = fscanf(fr,"%d",&type)) == EOF || succes == 0)	// pre��tanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(fclose(fr) == EOF)                                       // uzavretie s�bora
		return EFILEO;

	if(type == CVEC)											                      // ak je vektor
	{
		TVector vector;
		if((rvalue = readVector(&vector,file1)) != EOK)
			return	rvalue;
        printfVector(&vector);
		freeVector(&vector);
	}
	else if(type == CMAT)										                    // ak je matica
	{
		TMatrix matrix;
		if((rvalue = readMatrix(&matrix,file1)) != EOK)
			return rvalue;
		printfMatrix(&matrix);
		freeMatrix(&matrix,matrix.rows);
	}
	else							                            							// inak chyba
		return EFILEL;

	return rvalue;
}

/**
 * Funkcia sumVec s��ta dva vektory zo s�boru.
 * @param state stav pre s��in a s��et vektorov.
 * @param file1 n�zov s�bora 1
 * @param file2 n�zov s�bora 2
 * @return �spe�nos� funkcie.
 */
int sumVecMul(int state,char *file1, char *file2)
{
	int rvalue;		// n�vratov� hodnota funkcie
	TVector a,b;	// vektory a,b
  int value = 0;// v�sledok pre skal�rny s��in
	if((rvalue = readVector(&a,file1)) == EOK)
	{
		if ((rvalue = readVector(&b,file2)) == EOK)
		{
			if (a.count == b.count)									// ak sa vektory rovnaj�
			{
				if(state == CSUMV )									  // ak je s��tanie dvoch vektorov
				{
					for(int	x = 0; x < a.count; x++)
					{
						a.v[x] = a.v[x] + b.v[x];					// prejde cyklus a s��ta
					}
					printfVector(&a);
				}
				else												          // ak je skal�rny s��in dvoch vektorov
				{
					for(int x = 0; x < a.count; x++)
					{
						value += a.v[x] * b.v[x];					// prejde cyklus a vyn�sob�
					}
					printf("%d\n",value);
				}
			}
			else
				printf("false");
			freeVector(&b);											    // uvolni vektor b
		}
		freeVector(&a);												    // uvolni vektor a
	}
	return rvalue;
}

/**
 * Funkcia multiplyMat vyn�sob� dve matice a v�sledok ulo�� do tretej
 * @param a,b,c dan� matice pre oper�ciu n�sobenia
 */
void multiplyMat(TMatrix *a, TMatrix *b, TMatrix *c)
{
	for(int x = 0; x < a->rows; x++)                            // prejde riadky matice a
	{
		for(int y = 0; y < b->columns; y++)                       // prejde st�pce matice b
		{
			for(int z = 0; z < b->rows; z++)                        // prejde riadky matice b
			{
				c->rc[x][y] += a->rc[x][z] * b->rc[z][y];
			}
		}
	}
}

/**
 * Funkcia mulMatABA vypo��ta s��in mat�c AB pr�padne
 * @param state oper�cia ktor� sa m� vykona�
 * @param file1 n�zov s�boru 1
 * @param file2 n�zov s�boru 2
 * @return �spe�nos� funkcie.
 */
int mulMatABA(int state,char *file1, char *file2)
{
	int rvalue = EOK;   // n�vratov� hodnota funkcie
	TMatrix a,b,c,d;    // matica a,b,c,d
	d.rc = NULL;        // nastavi� na null

	if((rvalue = readMatrix(&a,file1)) == EOK)
	{
		if((rvalue = readMatrix(&b,file2)) == EOK)
		{
			c.rows = a.rows;                                    // prira� riadky
			c.columns = b.columns;                              // prira� st�pce
			if((rvalue = allocMatrix(&c)) == EOK)               // alokuj maticu c
			{
				if(a.columns == b.rows)                           // ak st�pce z matice a == riadky matice b
				{
					multiplyMat(&a,&b,&c);                          // vyn�sob dve matice
					if(state == CABAM)                              // ak je oper�cia A-B-A
					{
						d.rows = a.rows;
						d.columns = c.columns;
						if((rvalue = allocMatrix(&d)) == EOK)         // alokuj maticu d
						{
							if(a.columns == c.rows)
							{
								multiplyMat(&a,&c,&d);                    // vyn�sob dve matice
								printfMatrix(&d);                         // vyp� maticu
							}
							else
								printf("false");
							freeMatrix(&d,d.rows);                      // uvolnenie matice d
						}
					}
					else
						printfMatrix(&c);                             // vyp� maticu
				}
				else
					printf("false");
				freeMatrix(&c,c.rows);                            // uvolnenie matice c
			}
			freeMatrix(&b,b.rows);                              // uvolnenie matice b
		}
		freeMatrix(&a,a.rows);                                // uvolnenie matice a
	}
	return rvalue;
}

/**
 * Funkcia findVector n�jde vector v matici.
 * @param matrix matica v ktorej sa h�ad�.
 * @param vector vektor ktor� je hladan�.
 * @return �spech funkcie pri h�adan� vektora.
 */
int findVector(TMatrix *matrix, TVector *vector)
{
	int fvec = 0;     // po�et polo�iek n�jden�ch pri vektore

	// algoritmus prejde v�etky prvky
	for(int a = 0;a < matrix->rows; a++)                          // prejdenie riadkov
	{
		for(int b = 0;b < matrix->columns; b++)                     // prejdenie st�pcov
		{
			if(matrix->rc[a][b] == vector->v[0])                      // ak sa dan� prvok rovn�
			{
				if(a - vector->count + 1 >= 0)                          // z dola hore
				{
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom hore
					{
						if(matrix->rc[a-c][b] == vector->v[c])
							fvec++;                                           // ak sa rovn� prvok zv�� fvec
						else
							c = vector->count;
					}
					if(fvec == vector->count)                             // ak sa fvec rovn� po�tu prvkov vektora
						return 1;                                           // tak vr�t pravdu
					fvec = 0;
				}
				if(a + vector->count <= matrix->rows)                   // z hora dole
				{
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom dole
					{
						if(matrix->rc[a+c][b] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}

				if(b + vector->count <= matrix->columns)                // z lava doprava
				{
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom doprava
					{
						if(matrix->rc[a][b+c] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}
				if(b - vector->count + 1>= 0)                           // z prava dolava
				{
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom do�ava
					{
						if(matrix->rc[a][b-c] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}
				if((b - vector->count + 1 >= 0) && (a - vector->count + 1 >= 0))					// vlavo hore
				{
					for(int c = 0; c < vector->count; c++)                                  // prejdenie smerov v�avo hore
					{
						if(matrix->rc[a-c][b-c] == vector->v[c])
							fvec++;
						else
							c = vector->count;

					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}
				if((b + vector->count <= matrix->columns ) && (a - vector->count + 1 >= 0))			// vpravo hore
				{
					for(int c = 0; c < vector->count; c++)                                        // prejdenie smerom vpravo hore
					{
						if(matrix->rc[a-c][b+c] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}

				if((b - vector->count + 1 >= 0) && (a + vector->count <= matrix->rows))				// vlavo dole
				{
					for(int c = 0; c < vector->count; c++)                                      // prejdenie smerov vlavo dole
					{
						if(matrix->rc[a+c][b-c] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}

				if((b + vector->count  <= matrix->columns) && (a + vector->count <= matrix->rows))  // vpravo dole
				{
					for(int c = 0; c < vector->count; c++)                                            // prejdenie smerom vpravodole
					{
						if(matrix->rc[a+c][b+c] == vector->v[c])
							fvec++;
						else
							c = vector->count;
					}
					if(fvec == vector->count)
						return 1;
					fvec = 0;
				}
			}
		}
	}
	return 0;
}

/**
 * Funkcia crossWord vol� funkciu na vyh�adanie vektora.
 * @param file1 n�zov s�boru 1
 * @param file2 n�zov s�boru 2
 * @return �spe�nos� funkcie
 */
int crossWord(char *file1,char *file2)
{
	TMatrix a;          // �trukt�ra pre maticu
	TVector b;          // �trukt�ra pre vektor
	int rvalue = EOK;   // n�vratov� hodnota funkcie

	if((rvalue = readVector(&b,file1)) == EOK)      // pre��tanie vektora zo s�boru
	{
		if((rvalue = readMatrix(&a,file2)) == EOK)    // pre��tanie matice zo s�boru
		{
			if(findVector(&a,&b))                       // n�jdi vektor v matici
				printf("true");
			else

				printf("false");

			freeMatrix(&a,a.rows);
		}
		freeVector(&b);
	}
	return rvalue;
}

/**
 * Funkcia repairMatrix nahrad� prvky repair hodnotou value v matici.
 * @param matB matica ktor� sa opravuje
 * @param repair hodnota ktor� je opravovan�
 * @param value hodnota ktor� sa pou��va na nahradenie
 */
void repairMatrix(TMatrix *matB,int repair, int value)
{
	for(int a = 0; a < matB->rows;a++)              // prejde riadky
	{
		for(int b = 0; b < matB->columns; b++)        // prejde st�pce
		{
			if(matB->rc[a][b] == repair)                // kontrola matice a oprava
				matB->rc[a][b] = value;
		}
	}
}

/**
 * Funkcia checkMatrix skontroluje �i v matici nieje z�porn� prvok.
 * @param matA matica ktor� sa kontroluje
 * @return �spe�nos� funkcie

 */
int checkMatrix(TMatrix *matA)
{
	for(int a = 0; a < matA->rows; a++)         // prejde riadky
	{
		for(int b = 0; b < matA->columns; b++)    // prejde st�pce
		{
			if(matA->rc[a][b] < 0)                  // kontrola matice
				return ENUMB;
		}
	}
	return EOK;
}

/**
 * Funkcia findbubbles(TMatrix a, TMatrix b)
 * @param a matica v ktorej sa h�adaj� bubliny
 * @param b matica v ktorej sa kontroluje
 * @return po�et n�jden�ch bubl�n
 */
int findBubbles(TMatrix matA, TMatrix matB)
{
	int value = 0;  // premenn� pre aktu�lnu hodnotu
	int count = 0;  // premenn� pre po�et bubl�n

	for(int a = 0; a < matA.rows; a++)                          // prejdenie riadkov
	{
		for(int b = 0; b < matB.columns; b++)                     // prejdenie st�pcov
		{
			if(matA.rc[a][b] == 0)                                  // ak sa v matici na�la 0
			{
				if(matB.rc[a][b] == 0)                                // ak v matici B nie je ozna�en� prvok
				{
					count++;
					matB.rc[a][b] = count;
					value = count;
				}
				else                                                  // inak je ozna�en�
				{
					value = matB.rc[a][b];
				}

				if(a - 1 >= 0)						                            // hore
				{
					if(matB.rc[a-1][b] != 0 && matB.rc[a-1][b] != value)// ak horn� prvok nie je 0 v matici B
					{
						if(value > matB.rc[a-1][b])                       // ak je hodnota value v��ia tak sa oprav� matica
						{
							repairMatrix(&matB,value,matB.rc[a-1][b]);
							value = matB.rc[a-1][b];
						}
						else                                              // inak oprav maticu opa�ne
							repairMatrix(&matB,matB.rc[a-1][b],value);
						count--;
					}
					if(matA.rc[a-1][b] == 0)                            // prira� dan� hodnotu do matice
						matB.rc[a-1][b] = value;
				}
				if(a + 1 < matA.rows)                                 // dole
				{
					if(matB.rc[a+1][b] != 0 && matB.rc[a+1][b] != value)// ak doln� prvok nie je 0 v matici B
					{
						if(value > matB.rc[a+1][b])                       // ak hodnota je v��ia tak sa oprav� matica
						{
							repairMatrix(&matB,value,matB.rc[a+1][b]);
							value = matB.rc[a+1][b];
						}
						else
							repairMatrix(&matB,matB.rc[a+1][b],value);      // inak oprav maticu opa�ne
						count--;
					}
					if(matA.rc[a+1][b] == 0)
						matB.rc[a+1][b] = value;

				}
				if(b + 1 < matA.columns)                              // doprava
				{
					if(matB.rc[a][b+1] != 0 && matB.rc[a][b+1] != value)
					{
						if(value > matB.rc[a][b+1])
						{
							repairMatrix(&matB,value,matB.rc[a][b+1]);
							value = matB.rc[a][b+1];
						}
						else
							repairMatrix(&matB,matB.rc[a][b+1],value);
						count--;                                          // zn�enie po�tu bubl�n
					}
					if(matA.rc[a][b+1] == 0)
						matB.rc[a][b+1] = value;
				}
				if(b - 1 >= 0)						                            // dolava
				{
					if(matB.rc[a][b-1] != 0 && matB.rc[a][b-1] != value)
					{
						if(value > matB.rc[a][b-1])
						{
							repairMatrix(&matB,value,matB.rc[a][b-1]);
							value = matB.rc[a][b-1];
						}
						else
							repairMatrix(&matB,matB.rc[a][b-1],value);
						count--;                                          // zn�enie po�tu bubl�n
					}
					if(matA.rc[a][b-1] == 0)
						matB.rc[a][b-1] = value;
				}
			}
		}
	}

	return count;
}

/**
 * Funkcia bubbles vol� funkciu pre h�adanie bubl�n.
 * @param file1 n�zov textov�ho s�boru
 * @return �spe�nos� funkcie
 */
int bubbles(char *file1)
{
	TMatrix a,b;
	int rvalue = EOK;
	if((rvalue = readMatrix(&a,file1)) == EOK)      // pre��tanie matice
	{
		b.rows = a.rows;
		b.columns = a.columns;
		if((rvalue = checkMatrix(&a)) == EOK)         // skontrolovanie matice pre t�to oper�ciu
		{
			if((rvalue = allocMatrix(&b)) == EOK)       // alokovanie druhej matice
			{
				printf("%d",findBubbles(a,b));            // volanie funkcie findbubbles
				freeMatrix(&b,b.rows);
			}
		}
		freeMatrix(&a,a.rows);
	}
	return rvalue;
}

/**
 * Funkcia main vol� funkciu getParams, a pod�a parametrov vol�
 * jednotliv� funkcie.
 * @param argc po�et parametrov z pr�kazov�ho riadku
 * @param argv jednotliv� polo�ky z pr�kazov�ho riadku
 * @return �spe�nos� programu
 */
int main(int argc, char *argv[])
{
  TParams result = getParams(argc, argv);  // z�skanie parametrov z pr�kazov�ho riadka
  int error = 0;  						             // premenn� pre chybu

  if(result.ecode > EOK)  								                    // ak je chyba
  {
    error = result.ecode;
  }
  else
  {
    switch(result.state)
    {
      case CHELP: printHelp();    		                        // Volanie n�povedy
              		break;
      case CTEST:	error = testf(argv[2]);                     // Volanie testovania s�boru
              		break;
      case CSUMV:	error = sumVecMul(CSUMV,argv[2],argv[3]);   // Volanie vektorov�ho s��tu
                  break;
      case CMULV: error = sumVecMul(CMULV,argv[2],argv[3]);   // Volanie skal�rneho s��inu
              		break;
      case CMULM:	error = mulMatABA(CMULM,argv[2],argv[3]);   // Volanie s��inu dvoch mat�c
              		break;
      case CABAM:	error = mulMatABA(CABAM,argv[2],argv[3]);   // Volanie s��inu mat�c A*B*A
              		break;
      case CCROS: error = crossWord(argv[2],argv[3]); 		    // Volanie osemsmerovky
                  break;
      case CBUBL:	error = bubbles(argv[2]);			              // Volanie �lohy bubliny
                  break;
      default:		break;
    }
  }
  if(error)													                          // ak nastala chyba
  {
      printECode(error);
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* koniec proj3.c */
