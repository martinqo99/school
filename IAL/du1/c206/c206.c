	
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2010
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu, 
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek, 
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku, 
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu, 
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem, 
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu, 
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam 
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
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
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/	
    printf ("*ERROR* Chyba p�i pr�ci se seznamem.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
    
	
    L->First = NULL;  // kazdy ukazatel na NULL
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free. 
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
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	
    L->Act = L->First;  // ukazatel prveho daj do aktivneho
}

void DLLast (tDLList *L)	{
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	
	
    L->Act = L->Last;  // ukazatel posledneho daj do aktivneho
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/

	
	
    if(L->First != NULL)
        *val = L->First->data; // * pretoze to je odkaz, pointer
    else
        DLError();
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
	
	
    if(L->Last != NULL)
        *val = L->Last->data; // * pretoze to je odkaz, pointer
    else
        DLError();
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita 
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
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
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
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
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
**/
		
	
	
    if(L->Act == NULL)
        DLError();
    else
        *val = L->Act->data;  // preda hodnotu cez odkaz
}

void DLActualize (tDLList *L, int val) {
/*
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/
	
	
    if(L->Act != NULL)
        L->Act->data = val;  // priradi novu hodnotu do hodnoty aktivneho prvku
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/
	
	
    if(L->Act != NULL)
        L->Act = L->Act->rptr;  // posunie ukazatal na aktivny prvok na pravo
}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/
	
	
    if(L->Act != NULL)
        L->Act = L->Act->lptr;  // posunie ukazatel na aktivny prvok na lavo
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/
	
	
    return (L->Act == NULL) ? FALSE : TRUE; // vrat false pokial je neaktivny
}

/* Konec c206.c*/
