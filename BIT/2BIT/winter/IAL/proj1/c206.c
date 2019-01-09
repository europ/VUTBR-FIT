
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
*********************
** IAL             **
** 1. domáca úloha **
** Adrián Tóth     **
** 6.10.2016       **
*********************
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
    /*
    ** Vytiskne upozornění na to, že došlo k chybě.
    ** Tato funkce bude volána z některých dále implementovaných operací.
    **/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
    /*
    ** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
    ** z následujících funkcí nebude volána nad neinicializovaným seznamem).
    ** Tato inicializace se nikdy nebude provádět nad již inicializovaným
    ** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
    ** že neinicializované proměnné mají nedefinovanou hodnotu.
    **/
    // Inicializujeme vsetky prvky listu
    L->First = NULL;
    L->Last  = NULL;
    L->Act   = NULL;
}

void DLDisposeList (tDLList *L) {
    /*
    ** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
    ** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
    ** uvolněny voláním operace free.
    **/
    tDLElemPtr tmp; // pomocny pointer na element
    while(L->First != NULL) { // cyklime az kym prvy element existuje
        tmp = L->First; // ulozime si prvy element
        L->First = L->First->rptr; // posunieme prvy element
        free(tmp); // uvolnime ulozeny element
    }
    // list neobsahuje ziadny element tak nastavime prvky listu
    L->Act  = NULL;
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
    /*
    ** Vloží nový prvek na začátek seznamu L.
    ** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
    ** volá funkci DLError().
    **/
    tDLElemPtr tmp; // pomocny pointer na element
    tmp = malloc(sizeof(struct tDLElem)); // pointer ukazuje na vytvorene miesto v pamati pre element
    if(tmp == NULL) { // kontrola alokovania pamati
        DLError();
        return;
    }
    else { // uspesne sme alokovali miesto
        // inicializujeme element
        tmp->lptr = NULL;
        tmp->rptr = NULL;
        tmp->data = val;
        if (L->First == NULL) { // list bol prazdny a toto je 1. element
            L->First = tmp;
            L->Last = tmp;
        }
        else { // list uz obsahuje dake elementy, viac ako 1
            // premapujeme pointre
            L->First->lptr = tmp;
            tmp->rptr = L->First;
            L->First = tmp;
        }
    }
}

void DLInsertLast(tDLList *L, int val) {
    /*
    ** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
    ** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
    ** volá funkci DLError().
    **/
    tDLElemPtr tmp; // pomocny pointer na element
    tmp = malloc(sizeof(struct tDLElem)); // pointer ukazuje na vytvorene miesto v pamati pre element
    if(tmp == NULL) { // kontrola alokovania pamati
        DLError();
        return;
    }
    else { // uspesne sme alokovali miesto
        // inicializujeme element
        tmp->lptr = NULL;
        tmp->rptr = NULL;
        tmp->data = val;
        if (L->Last == NULL) { // list bol prazdny a toto je 1. element
            L->Last = tmp;
            L->First = tmp;
        }
        else { // list uz obsahuje dake elementy, viac ako 1
            // premapujeme pointre
            L->Last->rptr = tmp;
            tmp->lptr = L->Last;
            L->Last = tmp;
        }
    }
}

void DLFirst (tDLList *L) {
    /*
    ** Nastaví aktivitu na první prvek seznamu L.
    ** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
    ** aniž byste testovali, zda je seznam L prázdný.
    **/
    // prvy element bude aktivny
    L->Act = L->First;
}

void DLLast (tDLList *L) {
    /*
    ** Nastaví aktivitu na poslední prvek seznamu L.
    ** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
    ** aniž byste testovali, zda je seznam L prázdný.
    **/
    // posledny element bude aktivny
    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
    /*
    ** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
    ** Pokud je seznam L prázdný, volá funkci DLError().
    **/
    if (L->First == NULL && L->Last == NULL) DLError(); // kontrola listu ci je prazdny
    else {
        *val = L->First->data; //vraciame hodnotu prveho prvku
    }
}

void DLCopyLast (tDLList *L, int *val) {
    /*
    ** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
    ** Pokud je seznam L prázdný, volá funkci DLError().
    **/
    if (L->First == NULL && L->Last == NULL) DLError(); // kontrola listu ci je prazdny
    else {
        *val = L->Last->data; //vraciame hodnotu posledneho prvku
    }
}

void DLDeleteFirst (tDLList *L) {
    /*
    ** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
    ** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
    **/
    if (L->First != NULL && L->Last != NULL) { // kontrola zda list obsahuje element/y
        tDLElemPtr tmp; // pomocny pointer na element
        if (L->First == L->Last) { // ak obsahuje 1 element
            // vymazeme jediny element a nastavime list
            tmp = L->First;
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
            free(tmp);
        }
        else { // list obsahuje viac ako jeden element
            if (L->First == L->Act) L->Act = NULL; // ak je element aktivny tak aktivita sa straca
            tmp = L->First; // ulozime si prvy element
            L->First = tmp->rptr; // posunieme first
            L->First->lptr = NULL; // zrusime vazbu
            free(tmp); // uvolnime
        }
    }
}

void DLDeleteLast (tDLList *L) {
    /*
    ** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
    ** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
    **/

    if (L->Last != NULL && L->Last != NULL) { // kontrola zda list obsahuje element/y
        tDLElemPtr tmp; // pomocny pointer na element
        if (L->First == L->Last) { // ak obsahuje 1 element
            // vymazeme jediny element a nastavime list
            tmp = L->Last;
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
            free(tmp);
        }
        else { // list obsahuje viac ako jeden element
            if (L->Last == L->Act) L->Act = NULL; // ak je element aktivny tak aktivita sa straca
            tmp = L->Last; // ulozime si posledny element
            L->Last = tmp->lptr; // posunieme last
            L->Last->rptr = NULL; // zrusime vazbu
            free(tmp); // uvolnime
        }
    }
}

void DLPostDelete (tDLList *L) {
    /*
    ** Zruší prvek seznamu L za aktivním prvkem.
    ** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
    ** posledním prvkem seznamu, nic se neděje.
    **/
    if (L->Act == NULL || L->Act == L->Last) return; // nic nerob ak nie je list aktivny alebo ak je posledny prvok aktivny
    else {
        if (L->Act->rptr != NULL) {
            tDLElemPtr tmp; // pomocny pointer na element
            tmp = L->Act->rptr; // ulozime si element za aktivnym elementom
            L->Act->rptr = tmp->rptr; // namapujeme aktivny element
            if (tmp == L->Last) {
                L->Last = L->Act; // presmerujeme pointer na last
            }
            else {
                tmp->rptr->lptr = L->Act; // namapujeme nasledujuci element na aktivny
            }
            free(tmp); // mazeme
        }
    }
}

void DLPreDelete (tDLList *L) {
    /*
    ** Zruší prvek před aktivním prvkem seznamu L .
    ** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
    ** prvním prvkem seznamu, nic se neděje.
    **/
    if (L->Act == NULL || L->Act == L->First) return;
    else {
        if (L->Act->lptr != NULL) {
            tDLElemPtr tmp; // pomocny pointer na element
            tmp = L->Act->lptr; // ulozime si element za aktivnym elementom
            L->Act->lptr = tmp->lptr; // namapujeme aktivny element
            if (tmp == L->First) {
                L->First = L->Act; // presmerujeme pointer na first
            }
            else {
                tmp->lptr->rptr = L->Act; // namapujeme nasledujuci element na aktivny
            }
            free(tmp); // mazeme
        }
    }
}

void DLPostInsert (tDLList *L, int val) {
    /*
    ** Vloží prvek za aktivní prvek seznamu L.
    ** Pokud nebyl seznam L aktivní, nic se neděje.
    ** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
    ** volá funkci DLError().
    **/
    if (L->Act == NULL) return; // sesnam je neaktivny nic sa nedeje
    else {
        tDLElemPtr tmp; // pomocny pointer na element
        tmp = malloc(sizeof(struct tDLElem)); // alokujeme si miesto pre novy element
        if (tmp == NULL) { // skontrolujeme ci sa alokacia uskutocnila
            DLError();
            return;
        }
        else {
            // namapujeme element ktory bude za aktivnym elementom
            tmp->lptr = L->Act;
            tmp->rptr = L->Act->rptr;
            tmp->data = val;
            // namapujeme aktivny prvok
            L->Act->rptr = tmp;
            // ak je posledny
            if (L->Act == L->Last) {
                L->Last = tmp;
            }
            //ak nie je posledny
            else {
                tmp->rptr->lptr = tmp;
            }
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
    /*
    ** Vloží prvek před aktivní prvek seznamu L.
    ** Pokud nebyl seznam L aktivní, nic se neděje.
    ** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
    ** volá funkci DLError().
    **/
    if (L->Act == NULL) return; // sesnam je neaktivny nic sa nedeje
    else {
        tDLElemPtr tmp; // pomocny pointer na element
        tmp = malloc(sizeof(struct tDLElem)); // alokujeme si miesto pre novy element
        if (tmp == NULL) { // skontrolujeme ci sa alokacia uskutocnila
            DLError();
            return;
        }
        else {
            // namapujeme element ktory bude pred aktivnym elementom
            tmp->lptr = L->Act->lptr;
            tmp->rptr = L->Act;
            tmp->data = val;
            // namapujeme aktivny prvok
            L->Act->lptr = tmp;
            // ak je prvy
            if (L->Act == L->First) {
                L->First = tmp;
            }
            //ak nie je prvy
            else {
                tmp->lptr->rptr = tmp;
            }
        }
    }
}

void DLCopy (tDLList *L, int *val) {
    /*
    ** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
    ** Pokud seznam L není aktivní, volá funkci DLError ().
    **/
    if (L->Act == NULL) { // ak neexistuje aktivny element tak DLError()
        DLError();
        return;
    }
    else { // vraciame hodnotu aktivneho elementu
        *val = L->Act->data;
    }
}

void DLActualize (tDLList *L, int val) {
    /*
    ** Přepíše obsah aktivního prvku seznamu L.
    ** Pokud seznam L není aktivní, nedělá nic.
    **/
    if (L->Act == NULL) return;  // ak neexistuje aktivny element tak nic nerob
    else {
        L->Act->data = val; // prepiseme obsah aktivneho elementu
    }
}

void DLSucc (tDLList *L) {
    /*
    ** Posune aktivitu na následující prvek seznamu L.
    ** Není-li seznam aktivní, nedělá nic.
    ** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
    **/
    if (L->Act == NULL) return; // ak neexistuje aktivny element tak nic nerob
    else {
        if (L->Act == L->Last) { // ak aktivny element je posledny tak zrus aktivitu
            L->Act = NULL;
            return;
        }
        else { // posun aktivitu na dalsi prvok
            L->Act = L->Act->rptr;
        }
    }
}


void DLPred (tDLList *L) {
    /*
    ** Posune aktivitu na předchozí prvek seznamu L.
    ** Není-li seznam aktivní, nedělá nic.
    ** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
    **/
    if (L->Act == NULL) return; // ak neexistuje aktivny element tak nic nerob
    else {
        if (L->Act == L->First) { // ak aktivny element je prvy tak zrus aktivitu
            L->Act = NULL;
            return;
        }
        else { // posun aktivitu na predosly prvok
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {
    /*
    ** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
    ** Funkci je vhodné implementovat jedním příkazem return.
    **/
    return (L->Act != NULL) ? TRUE : FALSE; //pri aktivite vracia funkcia true=1, pri neaktivite false=0
}

/* Konec c206.c*/
