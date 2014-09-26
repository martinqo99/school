/*
 * Súbor    :   proj3.c
 * Dátum    :   15.11.2010
 * Autor    :   Michal Lukáè, xlukac05@stud.fit.vutbr.cz
 * Projekt  :   Maticové operácie.
 * Popis    :   Program je tretím projektom do predmetu základy programovania na
 *              VUT FIT. Program slú¾i na výpoèet základných maticových a vektorových
 *              operácii. Program naèíta vektor-maticu zo súbora. Program pracuje s
 *              dynamickou alokáciou a bol odtestovaný valgrindom. Implementované su
 *              v¹etky povinné operácie. A to súèin matíc, súèin matíc A*B*A, súèet dvoch
 *              vektorov, skalárny súèin dvoch vektorov, test, osemsmerovka a hµadanie
 *              bublín.
 * P.riadkov:   1026
 * TODO     :   -
 *
 */

/*********************************** Hlavièkové súbory ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************** Globálne premenné, kon¹tanty, makra, ¹truktúry... ********************/
// Kódý chýb programu
enum tecodes
{
  EOK = 0,              /* Bez chyby.																		*/
  ECLWRONG = 1,         /* Chybný príkazový riadok.											*/
  ECLVOID = 2,          /* Chybný príkazový riadok - ¾iadny parameter.	*/
  EFILEO = 3,           /* Chyba pri otváranie­ súboru.									*/
  EMALLOC = 4,          /* Chyba pri alokovaní­ pamäte.									*/
  EFILEL = 5,           /* Chyba pri naèítaní­ zo súboru.								*/
  ENUMB = 6,	          /* Zle zadaná matica / vektor.									*/
	EVMAT = 7,						/* Zle zadaný vektor matíc.											*/
};

// Stavové kódy programu, spracovanie podµa parametra
enum tstates
{
  CHELP,                /* Nápoveda.																		*/
  CTEST,                /* Testovací­ výstup.														*/
  CSUMV,                /* Vektorový súèet.															*/
  CMULV,                /* Skalárny súèin vektorov.                    	*/
  CMULM,                /* Násobenie dvoch matí­c.                     	*/
  CABAM,                /* Násobenie matí­c A * B * A.                 	*/
  CCROS,		            /* Osemsmerovka.               			           	*/
  CBUBL,	              /* Hµadanie bublín.        		     	           	*/
};

// Kódy pre typ spracovania súboru
enum ttype
{
	CVEC = 1,				/* Stav pre vektor.		 			      */
	CMAT = 2,				/* Stav pre maticu.	  					  */
 	CVMAT = 3,			/* Stav pre vektor matic.					*/
};

// Chybové hlásenia odpovedajúce chybovým kódom tecodes.
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

// Správa nápovedy
const char *HELPMSG =
    "Program  :   Maticové operácie.\n"
    "Autor    :   Michal Lukac, xlukac05@stud.fit.vutbr.cz\n"
    "Info     :   Tento program vznikol ako projekt pre predmet Zaklady\n"
    "             programovania na VUT FIT. Ulohou programu sú maticové\n"
    "             a vektorové výpoèty popísané v parametroch s ktorými sa\n"
    "             program spú¹»a. Implementované sú v¹etky povinné operácie.\n"
		"Parametre:	 -h --test --vadd --vscal --mmult --mexpr --eight --bubbles.\n"
    "Popis parametrov:\n"
    "           -h  parameter pre vypis napovedy.\n"
    "           --test data.txt parameter pre kontrolu vstupneho suboru\n"
    "           --vadd a.txt b.txt parameter pre scitanie dvoch vektorov\n"
    "           --vscal a.txt b.txt parameter pre skalarny sucin dvoch vektorov\n"
    "           --mmult A.txt B.txt parameter pre sucin dvoch matic\n"
    "           --mexpr A.txt B.txt parameter pre sucin matic A*B*A\n"
    "           --eight v.txt M.txt parameter pre osemsmerovku\n"
    "           --bubbles M.txt parameter pre hµadanie bublín";

// ©truktúra obsahujúca hodnoty parametrov z prí­kazovej riadky
typedef struct params
{
  int ecode;        /* Chybový kód programu, odpovedá výètu tecodes.    */
  int state;        /* Stavový kód programu, odpovedá výètu tstates.    */
} TParams;

// ©truktúra obsahujúca maticu
typedef struct matrix
{
  int rows;         /* Poèet riadkov matice.                    	      */
  int columns;      /* Poèet ståpcov matice.                  		      */
  int **rc;         /* Dvojnásobný ukazatel na int resp. matica.    	  */
} TMatrix;

// ©truktúra obsahujúca vektor
typedef struct vector
{
  int count;         /* Poèet prvkov vektora.          			    	      */
  int *v;            /* Ukazatel na int resp. vektor.        	    	    */
} TVector;

// prototypy funkcií
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
 * Funkcia printhelp vytlaèí­ nápovedu a ukonèí program.
 */
void printHelp(void)
{
    printf("%s\n",HELPMSG);
}

/**
 * Vytlaèí­ hlásenie odpovedajúce chybovému kódu pomocou
 * výètového typu a pole re»azcov ECODEMSG.
 * @param code kód chyby programu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Funkcia getParams spracuje argumenty príkazového riadku do ¹truktúry.
 * @param argc poèet parametrov prí­kazového riadka.
 * @param argv pole parametrov prí­kazového riadka.
 * @return ©truktura obsahujúca naèíatané parametre z arg. prí­kazového riadka
 */
TParams getParams(int argc, char *argv[])
{
  // inicializácia ¹truktúry
  TParams result =
  {
    .ecode = ECLWRONG,
    .state = CHELP,
  };

  if(argc == 1)                                 // ak sa nezadá ¾iadny parameter
  {
    result.ecode = ECLVOID;
  }
  else if (argc == 2)                           // ak je jeden parameter
  {
    if(strcmp("-h", argv[1]) == 0)              // ak je parameter -h nastav nápovedu
    {
      result.ecode = EOK;
      result.state = CHELP;
    }
  }
  else if (argc == 3)                           // ak sú dva parametre
  {
    if(strcmp("--test",argv[1]) == 0)           // ak je parameter --test nastav test
    {
        result.ecode = EOK;
        result.state = CTEST;
    }
    else if(strcmp("--bubbles",argv[1]) == 0)   // ak je parameter --bubbles nastav hµadanie bublín
    {
        result.ecode = EOK;
        result.state = CBUBL;
    }
  }
  else if(argc == 4)                            // ak sú tri parametre
  {
    if(strcmp("--vadd",argv[1]) == 0)           // ak je parameter --vadd nastav vektorový súèet
    {
        result.ecode = EOK;
        result.state = CSUMV;
    }
    else if(strcmp("--vscal",argv[1]) == 0)     // ak je parameter --vscal nastav vektorový súèin
    {
        result.ecode = EOK;
        result.state = CMULV;
    }
    else if(strcmp("--mmult",argv[1]) == 0)     // ak je parameter --mmult vynásob dve matice
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
  return result; // vráti výsledok
}

/**
 * Funkcia printfmatrix vypí¹e celú maticu
 * @param matrix ¹truktúra pre maticu
 */
void printfMatrix(TMatrix *matrix)
{
	if(matrix->rc != NULL)																// ak nie je NULL
	{
    printf("2\n%d %d\n",matrix->rows,matrix->columns);
    for(int a = 0; a < matrix->rows; a++)								// prejdi riadky
    {
      for(int b = 0; b < matrix->columns; b++)					// prejdi ståpce
      {
          printf("%d ", matrix->rc[a][b]);
      }
      printf("\n");
    }
	}
}

/**
 * Funkcia vypí¹e polo¾ky vektoru pomocou cyklu
 * @param vector ¹truktúra pre daný vektor
 */
void printfVector(TVector * vector)
{
	if(vector->v != NULL)
	{
  	printf("1\n%d\n",vector->count);
  	for(int a = 0; a < vector->count; a++)  // prejde polo¾ky vektora
    	printf("%d ",vector->v[a]);
	}
}

/**
 * Funkcia allocMatrix, alokuje dvojrozmerné pole
 * @param matrix
 * @return úspe¹nos» alokácie
 */
int allocMatrix(TMatrix *matrix)
{
	// alokácia matice
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
        freeMatrix(matrix,a);             // ak sa stala chyba tu uvolní sa matica s poètom alokovaných riadkov
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
 * Funkcia allocVector alokuje pole ulo¾ené v ¹truktúre vector.
 * @param vector odkaz na vektor
 * @return úspe¹nos» funkcie
 */
int allocVector(TVector *vector)
{
	// alokácia vektora
	if((vector->v = (int *) malloc(sizeof(int)*vector->count)) == NULL)
		return EMALLOC;
	return EOK;
}

/**
 * Funkcia freeMatrix uvolní maticu, resp. dvojrozmerné pole.
 * @param matrix daná matica
 * @param rows poèet alokovaných riadkov
 */
void freeMatrix(TMatrix *matrix, int rows)
{
	if(matrix->rc != NULL)
	{
		for(int a = 0; a < rows; a++)     // uvolni po riadkoch
			free(matrix->rc[a]);
		free(matrix->rc);                 // uvolní prvý rozmer
		matrix->rc = NULL;
	}
}

/**
 * Funkcia freeVector uvolní pole
 * @param vector odkaz na dynammické pole
 */
void freeVector(TVector *vector)
{
	if(vector->v != NULL)
	{
		free(vector->v);                  // uvolní vektor
		vector->v = NULL;
	}
}

/**
 * Funkcia readVector volá funkciu na alokovanie a naèítanie vektora.
 * @param vector odkaz na vektor
 * @param file1 názov súboru
 * @return úspe¹nos» funkcie
 */
int readVector(TVector *vector,char *file1)
{
	int number, succes;
	FILE *fr;

	if((fr = fopen(file1,"r")) == NULL) 									// otvorenie súbora
		return EFILEO;

	if((succes = fscanf(fr,"%d",&number)) == EOF || succes == 0)  					// preèítanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(number != CVEC)														        // ak to neni vektor
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if((succes = fscanf(fr,"%d",&vector->count)) == EOF || succes == 0)	    // preèítanie poètu prvkov
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(vector->count == 0)													      // kontrola veµkosti vektora
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return ENUMB;
	}

	if(allocVector(vector) != EOK)											  // alokovanie pola
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EMALLOC;
	}

  int a = 0; int c = 0;																	// algoritmus pre naèítanie matice
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

	if(a < vector->count)																	// finálna podmienka
	{
		freeVector(vector);
		if(fclose(fr)==EOF) return EFILEO;
    return ENUMB;
	}
	if(fclose(fr)==EOF) return EFILEO;
	return EOK;
}

/**
 * Funkcia readMatrix volá funkciu na alokovanie matice a naplní maticu.
 * @param matrix je ¹truktúra pre maticu
 * @param file1 názov súbora 1
 * @return úspe¹nos» funkcie
 */
int readMatrix(TMatrix *matrix,char *file1)
{
	int number, succes;	// premenná pre èíslo a úspech naèítania
	FILE *fr;

	if((fr = fopen(file1,"r")) == NULL) 										// otvorenie súbora
		return EFILEO;

	if((succes = fscanf(fr,"%d",&number)) == EOF || succes == 0)  			    // preèítanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(number != CMAT)															        // ak to neni matica
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	// preèítanie poètu riadkov
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

  int a = 0; int b = 0; int c = 0;												// algoritmus pre naèítanie matice
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

	if(a < matrix->rows || b != 0)												// finálna podmienka
	{
		if(fclose(fr) == EOF) return EFILEO;
		freeMatrix(matrix,matrix->rows);
		return ENUMB;
	}

	if(fclose(fr) == EOF) return EFILEO;
	return EOK;
}

/**
 * Funkcia testf odtestuje maticu, prípadne vektor zo súboru
 * data.txt. Takáto matica, vektor je dynamicky alokovaná a vypísaná.
 * @param file1 názov súbora
 * @return úspech testovania
 */
int testf(char *file1)
{
	int rvalue = EOK;   //	hodnota pre návratovú hodnotu
	int succes,type = 0;// hodnota pre úspech èítania a typ súboru
	FILE *fr;           // ¹truktúra súboru

	if((fr = fopen(file1,"r")) == NULL)                         // otvorenie súbora
		return EFILEL;

	if((succes = fscanf(fr,"%d",&type)) == EOF || succes == 0)	// preèítanie typu
	{
		if(fclose(fr) == EOF)	return EFILEO;
		return EFILEL;
	}

	if(fclose(fr) == EOF)                                       // uzavretie súbora
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
 * Funkcia sumVec sèíta dva vektory zo súboru.
 * @param state stav pre súèin a súèet vektorov.
 * @param file1 názov súbora 1
 * @param file2 názov súbora 2
 * @return úspe¹nos» funkcie.
 */
int sumVecMul(int state,char *file1, char *file2)
{
	int rvalue;		// návratová hodnota funkcie
	TVector a,b;	// vektory a,b
  int value = 0;// výsledok pre skalárny súèin
	if((rvalue = readVector(&a,file1)) == EOK)
	{
		if ((rvalue = readVector(&b,file2)) == EOK)
		{
			if (a.count == b.count)									// ak sa vektory rovnajú
			{
				if(state == CSUMV )									  // ak je sèítanie dvoch vektorov
				{
					for(int	x = 0; x < a.count; x++)
					{
						a.v[x] = a.v[x] + b.v[x];					// prejde cyklus a sèíta
					}
					printfVector(&a);
				}
				else												          // ak je skalárny súèin dvoch vektorov
				{
					for(int x = 0; x < a.count; x++)
					{
						value += a.v[x] * b.v[x];					// prejde cyklus a vynásobí
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
 * Funkcia multiplyMat vynásobí dve matice a výsledok ulo¾í do tretej
 * @param a,b,c dané matice pre operáciu násobenia
 */
void multiplyMat(TMatrix *a, TMatrix *b, TMatrix *c)
{
	for(int x = 0; x < a->rows; x++)                            // prejde riadky matice a
	{
		for(int y = 0; y < b->columns; y++)                       // prejde ståpce matice b
		{
			for(int z = 0; z < b->rows; z++)                        // prejde riadky matice b
			{
				c->rc[x][y] += a->rc[x][z] * b->rc[z][y];
			}
		}
	}
}

/**
 * Funkcia mulMatABA vypoèíta súèin matíc AB prípadne
 * @param state operácia ktorá sa má vykona»
 * @param file1 názov súboru 1
 * @param file2 názov súboru 2
 * @return úspe¹nos» funkcie.
 */
int mulMatABA(int state,char *file1, char *file2)
{
	int rvalue = EOK;   // návratová hodnota funkcie
	TMatrix a,b,c,d;    // matica a,b,c,d
	d.rc = NULL;        // nastavi» na null

	if((rvalue = readMatrix(&a,file1)) == EOK)
	{
		if((rvalue = readMatrix(&b,file2)) == EOK)
		{
			c.rows = a.rows;                                    // priraï riadky
			c.columns = b.columns;                              // priraï ståpce
			if((rvalue = allocMatrix(&c)) == EOK)               // alokuj maticu c
			{
				if(a.columns == b.rows)                           // ak ståpce z matice a == riadky matice b
				{
					multiplyMat(&a,&b,&c);                          // vynásob dve matice
					if(state == CABAM)                              // ak je operácia A-B-A
					{
						d.rows = a.rows;
						d.columns = c.columns;
						if((rvalue = allocMatrix(&d)) == EOK)         // alokuj maticu d
						{
							if(a.columns == c.rows)
							{
								multiplyMat(&a,&c,&d);                    // vynásob dve matice
								printfMatrix(&d);                         // vypí¹ maticu
							}
							else
								printf("false");
							freeMatrix(&d,d.rows);                      // uvolnenie matice d
						}
					}
					else
						printfMatrix(&c);                             // vypí¹ maticu
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
 * Funkcia findVector nájde vector v matici.
 * @param matrix matica v ktorej sa hµadá.
 * @param vector vektor ktorý je hladaný.
 * @return úspech funkcie pri hµadaní vektora.
 */
int findVector(TMatrix *matrix, TVector *vector)
{
	int fvec = 0;     // poèet polo¾iek nájdených pri vektore

	// algoritmus prejde v¹etky prvky
	for(int a = 0;a < matrix->rows; a++)                          // prejdenie riadkov
	{
		for(int b = 0;b < matrix->columns; b++)                     // prejdenie ståpcov
		{
			if(matrix->rc[a][b] == vector->v[0])                      // ak sa daný prvok rovná
			{
				if(a - vector->count + 1 >= 0)                          // z dola hore
				{
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom hore
					{
						if(matrix->rc[a-c][b] == vector->v[c])
							fvec++;                                           // ak sa rovná prvok zvý¹ fvec
						else
							c = vector->count;
					}
					if(fvec == vector->count)                             // ak sa fvec rovná poètu prvkov vektora
						return 1;                                           // tak vrát pravdu
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
					for(int c = 0; c < vector->count; c++)                // prejdenie smerom doµava
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
					for(int c = 0; c < vector->count; c++)                                  // prejdenie smerov vµavo hore
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
 * Funkcia crossWord volá funkciu na vyhµadanie vektora.
 * @param file1 názov súboru 1
 * @param file2 názov súboru 2
 * @return úspe¹nos» funkcie
 */
int crossWord(char *file1,char *file2)
{
	TMatrix a;          // ¹truktúra pre maticu
	TVector b;          // ¹truktúra pre vektor
	int rvalue = EOK;   // návratová hodnota funkcie

	if((rvalue = readVector(&b,file1)) == EOK)      // preèítanie vektora zo súboru
	{
		if((rvalue = readMatrix(&a,file2)) == EOK)    // preèítanie matice zo súboru
		{
			if(findVector(&a,&b))                       // nájdi vektor v matici
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
 * Funkcia repairMatrix nahradí prvky repair hodnotou value v matici.
 * @param matB matica ktorá sa opravuje
 * @param repair hodnota ktorá je opravovaná
 * @param value hodnota ktorá sa pou¾íva na nahradenie
 */
void repairMatrix(TMatrix *matB,int repair, int value)
{
	for(int a = 0; a < matB->rows;a++)              // prejde riadky
	{
		for(int b = 0; b < matB->columns; b++)        // prejde ståpce
		{
			if(matB->rc[a][b] == repair)                // kontrola matice a oprava
				matB->rc[a][b] = value;
		}
	}
}

/**
 * Funkcia checkMatrix skontroluje èi v matici nieje záporný prvok.
 * @param matA matica ktorá sa kontroluje
 * @return úspe¹nos» funkcie

 */
int checkMatrix(TMatrix *matA)
{
	for(int a = 0; a < matA->rows; a++)         // prejde riadky
	{
		for(int b = 0; b < matA->columns; b++)    // prejde ståpce
		{
			if(matA->rc[a][b] < 0)                  // kontrola matice
				return ENUMB;
		}
	}
	return EOK;
}

/**
 * Funkcia findbubbles(TMatrix a, TMatrix b)
 * @param a matica v ktorej sa hµadajú bubliny
 * @param b matica v ktorej sa kontroluje
 * @return poèet nájdených bublín
 */
int findBubbles(TMatrix matA, TMatrix matB)
{
	int value = 0;  // premenná pre aktuálnu hodnotu
	int count = 0;  // premenná pre poèet bublín

	for(int a = 0; a < matA.rows; a++)                          // prejdenie riadkov
	{
		for(int b = 0; b < matB.columns; b++)                     // prejdenie ståpcov
		{
			if(matA.rc[a][b] == 0)                                  // ak sa v matici na¹la 0
			{
				if(matB.rc[a][b] == 0)                                // ak v matici B nie je oznaèený prvok
				{
					count++;
					matB.rc[a][b] = count;
					value = count;
				}
				else                                                  // inak je oznaèený
				{
					value = matB.rc[a][b];
				}

				if(a - 1 >= 0)						                            // hore
				{
					if(matB.rc[a-1][b] != 0 && matB.rc[a-1][b] != value)// ak horný prvok nie je 0 v matici B
					{
						if(value > matB.rc[a-1][b])                       // ak je hodnota value väè¹ia tak sa opraví matica
						{
							repairMatrix(&matB,value,matB.rc[a-1][b]);
							value = matB.rc[a-1][b];
						}
						else                                              // inak oprav maticu opaène
							repairMatrix(&matB,matB.rc[a-1][b],value);
						count--;
					}
					if(matA.rc[a-1][b] == 0)                            // priraï danú hodnotu do matice
						matB.rc[a-1][b] = value;
				}
				if(a + 1 < matA.rows)                                 // dole
				{
					if(matB.rc[a+1][b] != 0 && matB.rc[a+1][b] != value)// ak dolný prvok nie je 0 v matici B
					{
						if(value > matB.rc[a+1][b])                       // ak hodnota je väè¹ia tak sa opraví matica
						{
							repairMatrix(&matB,value,matB.rc[a+1][b]);
							value = matB.rc[a+1][b];
						}
						else
							repairMatrix(&matB,matB.rc[a+1][b],value);      // inak oprav maticu opaène
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
						count--;                                          // zní¾enie poètu bublín
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
						count--;                                          // zní¾enie poètu bublín
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
 * Funkcia bubbles volá funkciu pre hµadanie bublín.
 * @param file1 názov textového súboru
 * @return úspe¹nos» funkcie
 */
int bubbles(char *file1)
{
	TMatrix a,b;
	int rvalue = EOK;
	if((rvalue = readMatrix(&a,file1)) == EOK)      // preèítanie matice
	{
		b.rows = a.rows;
		b.columns = a.columns;
		if((rvalue = checkMatrix(&a)) == EOK)         // skontrolovanie matice pre túto operáciu
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
 * Funkcia main volá funkciu getParams, a podµa parametrov volá
 * jednotlivé funkcie.
 * @param argc poèet parametrov z prí­kazového riadku
 * @param argv jednotlivé polo¾ky z prí­kazového riadku
 * @return úspe¹nos» programu
 */
int main(int argc, char *argv[])
{
  TParams result = getParams(argc, argv);  // získanie parametrov z prí­kazového riadka
  int error = 0;  						             // premenná pre chybu

  if(result.ecode > EOK)  								                    // ak je chyba
  {
    error = result.ecode;
  }
  else
  {
    switch(result.state)
    {
      case CHELP: printHelp();    		                        // Volanie nápovedy
              		break;
      case CTEST:	error = testf(argv[2]);                     // Volanie testovania súboru
              		break;
      case CSUMV:	error = sumVecMul(CSUMV,argv[2],argv[3]);   // Volanie vektorového súètu
                  break;
      case CMULV: error = sumVecMul(CMULV,argv[2],argv[3]);   // Volanie skalárneho súèinu
              		break;
      case CMULM:	error = mulMatABA(CMULM,argv[2],argv[3]);   // Volanie súèinu dvoch matí­c
              		break;
      case CABAM:	error = mulMatABA(CABAM,argv[2],argv[3]);   // Volanie súèinu matí­c A*B*A
              		break;
      case CCROS: error = crossWord(argv[2],argv[3]); 		    // Volanie osemsmerovky
                  break;
      case CBUBL:	error = bubbles(argv[2]);			              // Volanie úlohy bubliny
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
