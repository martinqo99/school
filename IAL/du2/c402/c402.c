
/* c402.c: ********************************************************************}
{* T�ma: Nerekurzivn� implementace operac� nad BVS 
**                                     Implementace: Petr P�ikryl, prosinec 1994
**                                           �pravy: Petr P�ikryl, listopad 1997
**                                                     Petr P�ikryl, kv�ten 1998
**			  	                        P�evod do jazyka C: Martin Tu�ek, srpen 2005
**                                         �pravy: Bohuslav K�ena, listopad 2009
**
** S vyu�it�m dynamick�ho p�id�lov�n� pam�ti, implementujte NEREKURZIVN�
** n�sleduj�c� operace nad bin�rn�m vyhled�vac�m stromem (p�edpona BT znamen�
** Binary Tree a je u identifik�tor� uvedena kv�li mo�n� kolizi s ostatn�mi
** p��klady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivn� vlo�en� nov�ho uzlu do stromu
**     BTPreorder ...... nerekurzivn� pr�chod typu pre-order
**     BTInorder ....... nerekurzivn� pr�chod typu in-order
**     BTPostorder ..... nerekurzivn� pr�chod typu post-order
**     BTDisposeTree ... zru� v�echny uzly stromu
**
** U v�ech funkc�, kter� vyu��vaj� n�kter� z pr�chod� stromem, implementujte
** pomocnou funkci pro nalezen� nejlev�j��ho uzlu v podstromu.
**
** P�esn� definice typ� naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na n�j je typu tBTNodePtr. Jeden uzel obsahuje polo�ku int Cont,
** kter� sou�asn� slou�� jako u�ite�n� obsah i jako vyhled�vac� kl�� 
** a ukazatele na lev� a prav� podstrom (LPtr a RPtr).
**
** P��klad slou�� zejm�na k procvi�en� nerekurzivn�ch z�pis� algoritm�
** nad stromy. Ne� za�nete tento p��klad �e�it, prostudujte si d�kladn�
** principy p�evodu rekurzivn�ch algoritm� na nerekurzivn�. Programov�n�
** je p�edev��m in�en�rsk� discipl�na, kde op�tn� objevov�n� Ameriky nem�
** m�sto. Pokud se V�m zd�, �e by n�co �lo zapsat optim�ln�ji, promyslete
** si v�echny detaily Va�eho �e�en�. Pov�imn�te si typick�ho um�st�n� akc�
** pro r�zn� typy pr�chod�. Zamyslete se nad modifikac� �e�en�ch algoritm�
** nap��klad pro v�po�et po�tu uzl� stromu, po�tu list� stromu, v��ky stromu
** nebo pro vytvo�en� zrcadlov�ho obrazu stromu (pouze pop�ehazov�n� ukazatel�
** bez vytv��en� nov�ch uzl� a ru�en� star�ch).
**
** P�i pr�chodech stromem pou�ijte ke zpracov�n� uzlu funkci BTWorkOut().
** Pro zjednodu�en� pr�ce m�te p�edem p�ipraveny z�sobn�ky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro pr�ci
** s pomocn�mi z�sobn�ky neupravujte 
**
** Pozor! Je t�eba spr�vn� rozli�ovat, kdy pou��t dereferen�n� oper�tor *
** (typicky p�i modifikaci) a kdy budeme pracovat pouze se samotn�m ukazatelem 
** (nap�. p�i vyhled�v�n�). V tomto p��kladu v�m napov� prototypy funkc�.
** Pokud pracujeme s ukazatelem na ukazatel, pou�ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocn� funkce, kterou budete volat p�i pr�chodech stromem pro zpracov�n�
** uzlu ur�en�ho ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla vol�na s NULL argumentem!\n");
  else 
    printf("V�pis hodnoty dan�ho uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro z�sobn�k hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace z�sobn�ku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo�� hodnotu na vrchol z�sobn�ku.
**/
{ 
                 /* P�i implementaci v poli m��e doj�t k p�ete�en� z�sobn�ku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do�lo k p�ete�en� z�sobn�ku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstran� prvek z vrcholu z�sobn�ku a sou�asn� vr�t� jeho hodnotu.
**/
{
                            /* Operace nad pr�zdn�m z�sobn�kem zp�sob� chybu. */
	if (S->top==0)  {
		printf("Chyba: Do�lo k podte�en� z�sobn�ku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li z�sobn�k pr�zdn�, vr�t� hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro z�sobn�k hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace z�sobn�ku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo�� hodnotu na vrchol z�sobn�ku.
**/
                 /* P�i implementaci v poli m��e doj�t k p�ete�en� z�sobn�ku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do�lo k p�ete�en� z�sobn�ku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstran� prvek z vrcholu z�sobn�ku a sou�asn� vr�t� jeho hodnotu.
**/
                            /* Operace nad pr�zdn�m z�sobn�kem zp�sob� chybu. */
	if (S->top==0) {
		printf("Chyba: Do�lo k podte�en� z�sobn�ku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li z�sobn�k pr�zdn�, vr�t� hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** N�sleduje j�dro dom�c� �lohy - funkce, kter� m�te implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci bin�rn�ho vyhled�vac�ho stromu.
**
** Inicializaci sm� program�tor volat pouze p�ed prvn�m pou�it�m bin�rn�ho
** stromu, proto�e neuvol�uje uzly nepr�zdn�ho stromu (a ani to d�lat nem��e,
** proto�e p�ed inicializac� jsou hodnoty nedefinovan�, tedy libovoln�).
** Ke zru�en� bin�rn�ho stromu slou�� procedura BTDisposeTree.
**	
** V�imn�te si, �e zde se poprv� v hlavi�ce objevuje typ ukazatel na ukazatel,	
** proto je t�eba p�i pr�ci s RootPtr pou��t dereferen�n� oper�tor *.
**/
	
	
  *RootPtr = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo�� do stromu nov� uzel s hodnotou Content.
**
** Z pohledu vkl�d�n� ch�pejte vytv��en� strom jako bin�rn� vyhled�vac� strom,
** kde uzly s hodnotou men�� ne� m� otec le�� v lev�m podstromu a uzly v�t��
** le�� vpravo. Pokud vkl�dan� uzel ji� existuje, neprov�d� se nic (dan� hodnota
** se ve stromu m��e vyskytnout nejv��e jednou). Pokud se vytv��� nov� uzel,
** vznik� v�dy jako list stromu. Funkci implementujte nerekurzivn�.
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Preorder nav�t�ven� uzly zpracujeme vol�n�m funkce BTWorkOut()
** a ukazatele na n� is ulo��me do z�sobn�ku.
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
** Pr�chod stromem typu preorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Preorder a z�sobn�ku ukazatel�. Zpracov�n� jednoho uzlu stromu
** realizujte jako vol�n� funkce BTWorkOut(). 
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Inorder ukl�d�me ukazatele na v�echny nav�t�ven� uzly do
** z�sobn�ku. 
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
** Pr�chod stromem typu inorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Inorder a z�sobn�ku ukazatel�. Zpracov�n� jednoho uzlu stromu
** realizujte jako vol�n� funkce BTWorkOut(). 
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
** Jde po lev� v�tvi podstromu, dokud nenaraz� na jeho nejlev�j�� uzel.
**
** P�i pr�chodu Postorder ukl�d�me ukazatele na nav�t�ven� uzly do z�sobn�ku
** a sou�asn� do z�sobn�ku bool hodnot ukl�d�me informaci, zda byl uzel
** nav�t�ven poprv� a �e se tedy je�t� nem� zpracov�vat. 
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
** Pr�chod stromem typu postorder implementovan� nerekurzivn� s vyu�it�m funkce
** Leftmost_Postorder, z�sobn�ku ukazatel� a z�sobn�ku hotdnot typu bool.
** Zpracov�n� jednoho uzlu stromu realizujte jako vol�n� funkce BTWorkOut(). 
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
** Zru�� v�echny uzly stromu a korektn� uvoln� jimi zabranou pam�.
**
** Funkci implementujte nerekurzivn� s vyu�it�m z�sobn�ku ukazatel�.
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

