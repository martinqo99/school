
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Pøikryl, prosinec 1994
**                                           Úpravy: Petr Pøikryl, listopad 1997
**                                                     Petr Pøikryl, kvìten 1998
**			  	                        Pøevod do jazyka C: Martin Tuèek, srpen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**
** S vyu¾itím dynamického pøidìlování pamìti, implementujte NEREKURZIVNÌ
** následující operace nad binárním vyhledávacím stromem (pøedpona BT znamená
** Binary Tree a je u identifikátorù uvedena kvùli mo¾né kolizi s ostatními
** pøíklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vlo¾ení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní prùchod typu pre-order
**     BTInorder ....... nerekurzivní prùchod typu in-order
**     BTPostorder ..... nerekurzivní prùchod typu post-order
**     BTDisposeTree ... zru¹ v¹echny uzly stromu
**
** U v¹ech funkcí, které vyu¾ívají nìkterý z prùchodù stromem, implementujte
** pomocnou funkci pro nalezení nejlevìj¹ího uzlu v podstromu.
**
** Pøesné definice typù naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na nìj je typu tBTNodePtr. Jeden uzel obsahuje polo¾ku int Cont,
** která souèasnì slou¾í jako u¾iteèný obsah i jako vyhledávací klíè 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Pøíklad slou¾í zejména k procvièení nerekurzivních zápisù algoritmù
** nad stromy. Ne¾ zaènete tento pøíklad øe¹it, prostudujte si dùkladnì
** principy pøevodu rekurzivních algoritmù na nerekurzivní. Programování
** je pøedev¹ím in¾enýrská disciplína, kde opìtné objevování Ameriky nemá
** místo. Pokud se Vám zdá, ¾e by nìco ¹lo zapsat optimálnìji, promyslete
** si v¹echny detaily Va¹eho øe¹ení. Pov¹imnìte si typického umístìní akcí
** pro rùzné typy prùchodù. Zamyslete se nad modifikací øe¹ených algoritmù
** napøíklad pro výpoèet poètu uzlù stromu, poètu listù stromu, vý¹ky stromu
** nebo pro vytvoøení zrcadlového obrazu stromu (pouze popøehazování ukazatelù
** bez vytváøení nových uzlù a ru¹ení starých).
**
** Pøi prùchodech stromem pou¾ijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodu¹ení práce máte pøedem pøipraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat pøi prùchodech stromem pro zpracování
** uzlu urèeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
{ 
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do¹lo k pøeteèení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Do¹lo k podteèení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do¹lo k pøeteèení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Do¹lo k podteèení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze pøed prvním pou¾itím binárního
** stromu, proto¾e neuvolòuje uzly neprázdného stromu (a ani to dìlat nemù¾e,
** proto¾e pøed inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zru¹ení binárního stromu slou¾í procedura BTDisposeTree.
**	
** V¹imnìte si, ¾e zde se poprvé v hlavièce objevuje typ ukazatel na ukazatel,	
** proto je tøeba pøi práci s RootPtr pou¾ít dereferenèní operátor *.
**/
	
	
  *RootPtr = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo¾í do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytváøený strom jako binární vyhledávací strom,
** kde uzly s hodnotou men¹í ne¾ má otec le¾í v levém podstromu a uzly vìt¹í
** le¾í vpravo. Pokud vkládaný uzel ji¾ existuje, neprovádí se nic (daná hodnota
** se ve stromu mù¾e vyskytnout nejvý¹e jednou). Pokud se vytváøí nový uzel,
** vzniká v¾dy jako list stromu. Funkci implementujte nerekurzivnì.
**/
	
	
		
  // ulozime si ukazatel
  tBTNodePtr *temp = RootPtr;
  // ak je strom prazdny
  if(*RootPtr == NULL)
  {
    // malloc a pridanie do uzlu polozky
    (*RootPtr) = (struct tBTNode *)malloc(sizeof(struct tBTNode));
    (*RootPtr)->Cont = Content;
    // lavy a pravy ukazatel si inicializujeme na null
    (*RootPtr)->LPtr = NULL;
    (*RootPtr)->RPtr = NULL;
    return;
  }
  // inak strom neni prazdny
  else
  {
    // prejdi strom a hladaj prazdny uzol
    while(*temp != NULL)
    {
      if((*temp)->Cont == Content)
        return;
      else if((*temp)->Cont < Content)
        temp = &(*temp)->RPtr;
      else if((*temp)->Cont > Content)
        temp = &(*temp)->LPtr;
    }

  // vytvorenie vyhladaneho uzlu
  *temp = (struct tBTNode *)malloc(sizeof(struct tBTNode));
  (*temp)->Cont = Content;
  (*temp)->LPtr = NULL;
  (*temp)->RPtr = NULL;
  }
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Preorder nav¹tívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na nì is ulo¾íme do zásobníku.
**/

	
	
  tBTNodePtr uk = ptr;
  // najprv push potom vypis
  while(uk != NULL)
  {
    SPushP(Stack,uk);
    BTWorkOut(uk);
    // posuneme sa do lava
    uk = uk->LPtr;
  }
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Prùchod stromem typu preorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Preorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	
	
  // pokial nie je strom prazdny
  if(RootPtr != NULL)
  {
    // vytvorenie pomocneho zasobniku
    tStackP stack;
    SInitP(&stack);
    tBTNodePtr uk = RootPtr;
    // zavolanie leftmost
    Leftmost_Preorder(uk,&stack);
    while(!SEmptyP(&stack))
    {
      // ziskame z vrchu zasobniku poslednu polozku
      uk = STopPopP(&stack);
      Leftmost_Preorder(uk->RPtr,&stack);
    }
  }
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Inorder ukládáme ukazatele na v¹echny nav¹tívené uzly do
** zásobníku. 
**/
	
	
	
  tBTNodePtr uk = ptr;
  // najprv push, uzol nevypisujeme
  while(uk != NULL)
  {
    SPushP(Stack,uk);
    // posuneme sa do lava
    uk = uk->LPtr;
  }
	
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Prùchod stromem typu inorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Inorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	
	
  // pokial nie je strom prazdny
  if(RootPtr != NULL)
  {
    // vytvorime si pomocny zasobnik
    tStackP stack;
    SInitP(&stack);
    tBTNodePtr uk = RootPtr;
    Leftmost_Inorder(uk,&stack);
    // prejdeme strom
    while(!SEmptyP(&stack))
    {
      // ziskame z vrcholu zasobniku uzol a vypiseme ho
      uk = STopPopP(&stack);
      BTWorkOut(uk);
      Leftmost_Inorder(uk->RPtr,&stack);
    }
  }
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Postorder ukládáme ukazatele na nav¹tívené uzly do zásobníku
** a souèasnì do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** nav¹tíven poprvé a ¾e se tedy je¹tì nemá zpracovávat. 
**/

	
	
  tBTNodePtr uk = ptr;
  // vyuzijeme zasobniku pravdivostnych hodnot posunu a zasobniku uzlov
  while(uk != NULL)
  {
    // pushneme na zasobniky
    SPushP(StackP,uk);
    SPushB(StackB,1);
    // ideme dolava
    uk = uk->LPtr;
  }
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Prùchod stromem typu postorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Postorder, zásobníku ukazatelù a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/

	
		
  tBTNodePtr uk = RootPtr;
  if(uk != NULL)
  {
    // premenna urcujuca ci ideme zlava alebo zprava
    int zlava = 0;
    // pomocne zasobniky
    tStackB stackB;
    tStackP stackP;
    SInitP(&stackP);
    SInitB(&stackB);
    Leftmost_Postorder(uk,&stackP,&stackB);
    // prejdi strom
    while(!SEmptyP(&stackP))
    {
      // topneme si zo zasobniku polozku
      uk = stackP.a[stackP.top];
      zlava = STopPopB(&stackB);
      // ideme zlava ?
      if(zlava)
      {
        SPushB(&stackB,0);
        Leftmost_Postorder(uk->RPtr,&stackP,&stackB);
      }
      else
      {
        // popneme si zasobnik a vypiseme
        stackP.top = stackP.top - 1;
        BTWorkOut(uk);
      }
    }
  }
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zru¹í v¹echny uzly stromu a korektnì uvolní jimi zabranou pamì».
**
** Funkci implementujte nerekurzivnì s vyu¾itím zásobníku ukazatelù.
**/
	
	
  // vytvorime si pomocne ukazatele
  struct tBTNode *uz = *RootPtr;
  struct tBTNode *uz2 = NULL;
  struct tBTNode *uz3 = NULL;

  // ak je strom prazdny nic nerobim
  if(uz != NULL)
  {
    // inicializujeme si zasobnik
    tStackP stack;
    SInitP(&stack);
    // existuje korenovy uzol s lavou diagonalou?
    while(uz != NULL)
    {
      SPushP(&stack,uz);
      uz = uz->LPtr;
    }
    // cyklus pre postupne uvolnovanie
    do {
      // ziskam polozku zo zasobniku ktoru nakoniec uvolnim
      uz = STopPopP(&stack);
      uz2 = uz;
      // polozke od zasobniku vyhladam pravy uzol
      if(uz->RPtr != NULL)
      {
        uz3 = uz->RPtr;
        // pravemu uzlu kuknem lavu diagonalu a dam do zasobniku
        while(uz3 != NULL)
        {
          SPushP(&stack,uz3);
          uz3 = uz3->LPtr;
        }
      }
      // uvolni polozku zo zasobniku
      free(uz2);
      uz2 = NULL;
    } while(!SEmptyP(&stack));
    // korenovy uzol na NULL
    *RootPtr = NULL;
  }
}

/* konec c402.c */

