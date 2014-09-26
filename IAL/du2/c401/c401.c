
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvoøil: Petr Pøikryl, listopad 1994
**                                         Úpravy: Andrea Nìmcová, prosinec 1995
**                                                      Petr Pøikryl, duben 1996
**                                                   Petr Pøikryl, listopad 1997
**                                  Pøevod do jazyka C: Martin Tuèek, øíjen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**
** Implementujte rekurzivním zpùsobem operace nad binárním vyhledávacím
** stromem (BVS; v angliètinì BST - Binary Search Tree).
**
** Klíèem uzlu stromu je jeden znak (obecnì jím mù¾e být cokoliv, podle
** èeho se vyhledává). U¾iteèným (vyhledávaným) obsahem je zde integer.
** Uzly s men¹ím klíèem le¾í vlevo, uzly s vìt¹ím klíèem le¾í ve stromu
** vpravo. Vyu¾ijte dynamického pøidìlování pamìti.
** Rekurzivním zpùsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíèem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zru¹ení uzlu se zadaným klíèem
**   BSTDispose ... zru¹ení celého stromu
**
** ADT BVS je reprezentován koøenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíè (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). 
** Pøesnou definici typù naleznete v souboru c401.h.
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede poèáteèní inicializaci stromu pøed jeho prvním pou¾itím.
**
** Ovìøit, zda byl ji¾ strom pøedaný pøes RootPtr inicializován, nelze,
** proto¾e pøed první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor vyu¾ívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to pøed vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by toti¾ mohlo vést ke ztrátì pøístupu
** k dynamicky alokované pamìti (tzv. "memory leak").
**	
** V¹imnìte si, ¾e se v hlavièce objevuje typ ukazatel na ukazatel.	
** Proto je tøeba pøi pøiøazení pøes RootPtr pou¾ít dereferenèní operátor *.
** Ten bude pou¾it i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/
	
	
	
  // nastavime ukazatel na null
  *RootPtr = NULL;
	
}	

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíèem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v promìnné Content se
** vrací obsah pøíslu¹ného uzlu.´Pokud pøíslu¹ný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah promìnné Content není definován (nic do ní proto
** nepøiøazujte).
**
** Pøi vyhledávání v binárním stromu bychom typicky pou¾ili cyklus ukonèený
** testem dosa¾ení listu nebo nalezení uzlu s klíèem K. V tomto pøípadì ale
** problém øe¹te rekurzivním volání této funkce, pøièem¾ nedeklarujte ¾ádnou
** pomocnou funkci.
**/
							   
	
  // ak je uzol NULL skoncime hledani
  if(RootPtr == NULL)
    return FALSE;
  // inak prehladaj stromcek
  else
  {
    // ak sa rovna kluc
    if(RootPtr->Key == K)
    {
      *Content = RootPtr->BSTNodeCont;
      return TRUE;
    }
    // ak je vacsi tak chod do prava
    else if(RootPtr->Key < K)
      return BSTSearch((tBSTNodePtr)RootPtr->RPtr,K,Content);
    // ak je mensi tak chod do prava
    else
      return BSTSearch((tBSTNodePtr)RootPtr->LPtr,K,Content);
  }
  // nenasli sme, vratime false
  return FALSE;
	
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vlo¾í do stromu RootPtr hodnotu Content s klíèem K.
**
** Pokud ji¾ uzel se zadaným klíèem ve stromu existuje, bude obsah uzlu
** s klíèem K nahrazen novou hodnotou. Pokud bude do stromu vlo¾en nový
** uzel, bude vlo¾en v¾dy jako list stromu.
**
** Funkci implementujte rekurzivnì. Nedeklarujte ¾ádnou pomocnou funkci.
**
** Rekurzivní implementace je ménì efektivní, proto¾e se pøi ka¾dém
** rekurzivním zanoøení ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto pøípadì byla efektivnìj¹í jak z hlediska
** rychlosti, tak z hlediska pamì»ových nárokù. Zde jde ale o ¹kolní
** pøíklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
		
	
	
  // ak je uzol NULL tak ho vytvor
  if(*RootPtr == NULL)
  {
    // malloc a pridanie do uzlu polozky
    (*RootPtr) = (struct tBSTNode *)malloc(sizeof(struct tBSTNode));
    (*RootPtr)->Key = K;
    (*RootPtr)->BSTNodeCont = Content;
    // lavy a pravy ukazatel si inicializujeme na null
    (*RootPtr)->LPtr = NULL;
    (*RootPtr)->RPtr = NULL;
  }
  // inak postupuj v stromceku
  else
  {
    // Ak je kluc rovny prepis hodnotu
    if((*RootPtr)->Key == K)
      (*RootPtr)->BSTNodeCont = Content;
    // Ak je kluc vacsi chod do praveho uzlu
    else if((*RootPtr)->Key < K)
      BSTInsert(&(*RootPtr)->RPtr,K,Content);
    // Ak je kluc mensi chod do laveho uzlu
    else
      BSTInsert(&(*RootPtr)->LPtr,K,Content);
  }
	
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, pøesun a uvolnìní nejpravìj¹ího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude pøesunuta hodnota
** nejpravìj¹ího uzlu v podstromu, který je urèen ukazatelem RootPtr.
** Pøedpokládá se, ¾e hodnota ukazatele RootPtr nebude NULL (zajistìte to
** testováním pøed volání této funkce). Tuto funkci implementujte rekurzivnì. 
**
** Tato pomocná funkce bude pou¾ita dále. Ne¾ ji zaènete implementovat,
** pøeètìte si komentáø k funkci BSTDelete(). 
**/
	
	
		
  // Pozreme sa do praveho uzlu rekurzivne pokial neprideme na koniec
  if((*RootPtr)->RPtr != NULL)
    ReplaceByRightmost(PtrReplaced,&(*RootPtr)->RPtr);
  // inak nahradime uzol PtrReplaced
  else
  {
    // nahradime hodnoty
    PtrReplaced->Key = (*RootPtr)->Key;
    PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
    // posunieme a uvolnime aktualny uzol
    tBSTNodePtr temp = *RootPtr;
    *RootPtr = (*RootPtr)->LPtr;
    free(temp);
    // nastavime ukazatel na null
    RootPtr = NULL;
  }
	
}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zru¹í uzel stromu, který obsahuje klíè K.
**
** Pokud uzel se zadaným klíèem neexistuje, nedìlá funkce nic. 
** Pokud má ru¹ený uzel jen jeden podstrom, pak jej zdìdí otec ru¹eného uzlu.
** Pokud má ru¹ený uzel oba podstromy, pak je ru¹ený uzel nahrazen nejpravìj¹ím
** uzlem levého podstromu. Pozor! Nejpravìj¹í uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivnì s vyu¾itím døíve deklarované
** pomocné funkce ReplaceByRightmost.
**/
	
	
	
  // ak je uzol NULL skoncime hledani
  if(*RootPtr == NULL)
    return;
  // inak prehladaj stromcek
  else
  {
    // ak sa rovna kluc
    if((*RootPtr)->Key == K)
    {
      // ak existuju oba podstromy
      if((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr != NULL)
        ReplaceByRightmost(*RootPtr,&(*RootPtr)->LPtr);
      // ak existuje lavy podstrom
      else if((*RootPtr)->LPtr != NULL)
      {
        tBSTNodePtr temp = *RootPtr;
        *RootPtr = (*RootPtr)->LPtr;
        free(temp);
      }
      // ak existuje pravy podstrom
      else if((*RootPtr)->RPtr != NULL)
      {
        tBSTNodePtr temp = *RootPtr;
        *RootPtr = (*RootPtr)->RPtr;
        free(temp);
      }
      // ak nema podstromy, je to list
      else
      {
        tBSTNodePtr temp = *RootPtr;
        free(temp);
        *RootPtr = NULL;
      }
    }
    // ak je vacsi tak chod do prava
    else if((*RootPtr)->Key < K)
      BSTDelete(&(*RootPtr)->RPtr,K);
    // ak je mensi tak chod do lava
    else
      BSTDelete(&(*RootPtr)->LPtr,K);
  }
}

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zru¹í celý binární vyhledávací strom a korektnì uvolní pamì».
**
** Po zru¹ení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivnì bez deklarování pomocné
** funkce.
**/
	
  // zrus uzol ak nie je NULL
  if(*RootPtr != NULL)
  {
    // Pozri sa do pravej casti stromceka
    if(&(*RootPtr)->RPtr != NULL)
      BSTDispose(&(*RootPtr)->RPtr);
    // Pozri sa do lavej casti stromceka
    if(&(*RootPtr)->LPtr != NULL)
      BSTDispose(&(*RootPtr)->LPtr);
    // Uvolni uzol stromceka
    free(*RootPtr);
    // nastavime uzol na null
    *RootPtr = NULL;
  }
}

/* konec c401.c */

