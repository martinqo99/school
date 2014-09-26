	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2010
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/
/**
 * Autor implementacie: Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * Predmet: 1.uloha do predmetu Algoritmy na VUT FIT.
 */
#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* Chyba pøi práci se seznamem.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
    
	
    L->First = NULL;  // kazdy ukazatel na NULL
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
	
	
    if(L->First !=NULL)  // Pokial existuje vobec prvy prvok
    {
        tDLElemPtr item;
        while(L->First)  // Prejdi cely zoznam a uvolnuj
        {
            item = L->First;  // nastav prvy na uvolnenie
            L->First = item->rptr;
            free(item);  // uvolni polozku
        }
    }

    L->Act = NULL;  // kazdy ukazatel na NULL
    L->First = NULL;
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	
    tDLElemPtr item = NULL;
    if((item = (tDLElemPtr)malloc(sizeof(struct tDLElem))) == NULL)
    {
        DLError();
        return;
    }
    item->data = val;
    if(L->First == NULL)  // Ak sa jedna o vkladanie prveho prvku
    {
        item->lptr = NULL;
        item->rptr = NULL;
        L->First = item;
        L->Last = item;
    }
    else  // inak uz v zozname nejaka polozka existuje
    {
        item->lptr = NULL;
        item->rptr = L->First;
        L->First->lptr = item;
        L->First = item;
    }
}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
	
	
    tDLElemPtr item = NULL;
    if((item = (tDLElemPtr)malloc(sizeof(struct tDLElem))) == NULL)
    {
        DLError();
        return;
    }
    item->data = val;
    if(L->First == NULL)  // ak sa jedna vobec o prvy prvok
    {
        item->lptr = NULL;
        item->rptr = NULL;
        L->First = item;  // do prveho vloz polozku
        L->Last = item;   // a ak je vobec prvy tak nastav aj posledny ukazatel
    }
    else  // inak uz polozka v zozname existuje
    {
        item->rptr = NULL;
        item->lptr = L->Last;
        L->Last->rptr = item;  // inak pridaj normalne
        L->Last = item;
    }
}

void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->First;  // ukazatel prveho daj do aktivneho
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
	
    L->Act = L->Last;  // ukazatel posledneho daj do aktivneho
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	
	
    if(L->First != NULL)
        *val = L->First->data; // * pretoze to je odkaz, pointer
    else
        DLError();
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
	
    if(L->Last != NULL)
        *val = L->Last->data; // * pretoze to je odkaz, pointer
    else
        DLError();
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
	
	
    if(L->First != NULL)
    {
        tDLElemPtr item = L->First->rptr;
        if(L->First == L->Act)  // pokial prvy je aktualny, aktualny = NULL
            L->Act = NULL;
        if(L->Last == L->First)
        {
            free(L->First);
            L->Last = NULL;  // inicializacia na NULL
            L->First = NULL;
            L->Act = NULL;
            return;
        }
        free(L->First);
        item->lptr = NULL;  // lavy ukazatel nastavime na NULL
        L->First = item;
    }
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
	
	
    if(L->Last != NULL)
    {
        if(L->Last == L->Act)
            L->Act = NULL;  // Aktivny prvok nastavime na NULL
        if(L->Last == L->First)
        {
            free(L->Last);
            L->Last = NULL;  // inicializacia na NULL
            L->First = NULL;
            L->Act = NULL;
            return;
        }
        // Uvolnenie posledneho a nastavenie dalsieho prvku
        tDLElemPtr item = L->Last->lptr;
        free(L->Last);
        item->rptr = NULL;
        L->Last = item;
    }
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
	
		
    if(L->Act != NULL && L->Act != L->Last)
    {
        tDLElemPtr item = L->Act->rptr;  // pravy ukazatel aktivneho prvku uvolnime
        if(item == L->Last)
        {
            L->Last = L->Act;
            L->Act->rptr = NULL;  // pravy prvok za aktualny na NULL, koniec
            free(item);
            return;
        }
        tDLElemPtr item2 = L->Act->rptr->rptr;  // prvok za-za aktualnym prvkom
        item2->lptr = L->Act;
        L->Act->rptr = item2;
        free(item);
    }
}

void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
	
			
    if(L->Act != NULL && L->Act != L->First)
    {
        tDLElemPtr item = L->Act->lptr; // lavy ukazatel aktivneho prvku uvolnime
        if(item == L->First)
        {
            L->First = L->Act;
            L->Act->lptr = NULL;  // lavy prvok pred aktualnym prvkom
            free(item);
            return;
        }
        tDLElemPtr item2 = L->Act->lptr->lptr;  // prvok pred pred akt prvkom
        item2->rptr = L->Act;
        L->Act->lptr = item2;
        free(item);
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	
    if(L->Act != NULL)
    {
        tDLElemPtr item = NULL;
        if((item = (tDLElemPtr)malloc(sizeof(struct tDLElem))) == NULL)
        {
            DLError();
            return;
        }
        item->data = val;
        if(L->Act == L->Last)
        {
            item->rptr = NULL;  // kedze je aktualny = posledny pravy ukazatel dame na NULL
            item->lptr = L->Last;
            L->Last->rptr = item;
            L->Last = item;
            return;
        }
        // ak nie je aktualny = posledny
        item->rptr = L->Act->rptr;
        L->Act->rptr->lptr = item;
        item->lptr = L->Act;
        L->Act->rptr = item;  // pravy ukazatel ukazuje na novy prvok
    }
}

void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	
    if(L->Act != NULL)
    {
        tDLElemPtr item = NULL;
        if((item = (tDLElemPtr)malloc(sizeof(struct tDLElem))) == NULL)
        {
            DLError();
            return;
        }
        item->data = val;
        if(L->Act == L->First) // pokial je aktivny prvok prvy prvkom
        {
            item->lptr = NULL;  // lavu cast noveho prvku daj na NULL
            item->rptr = L->First;
            L->First->lptr = item;
            L->First = item;
            return;
        }
        // inak pridaj dalsi prvok pred aktivny
        item->lptr = L->Act->lptr;
        L->Act->lptr->rptr = item;
        item->rptr = L->Act;
        L->Act->lptr = item;  // lavy ukazatel ukazuje na novy prvok
    }
}

void DLCopy (tDLList *L, int *val)	{
/*
** Vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
	
	
    if(L->Act == NULL)
        DLError();
    else
        *val = L->Act->data;  // preda hodnotu cez odkaz
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	
	
    if(L->Act != NULL)
        L->Act->data = val;  // priradi novu hodnotu do hodnoty aktivneho prvku
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
	
	
    if(L->Act != NULL)
        L->Act = L->Act->rptr;  // posunie ukazatal na aktivny prvok na pravo
}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
	
	
    if(L->Act != NULL)
        L->Act = L->Act->lptr;  // posunie ukazatel na aktivny prvok na lavo
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
	
	
    return (L->Act == NULL) ? FALSE : TRUE; // vrat false pokial je neaktivny
}

/* Konec c206.c*/
