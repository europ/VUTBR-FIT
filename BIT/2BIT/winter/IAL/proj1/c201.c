
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
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
** 1. domaca uloha **
** Adrian Toth     **
** 7.10.2016       **
*********************
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
	/*
	** Vytiskne upozornění na to, že došlo k chybě.
	** Tato funkce bude volána z některých dále implementovaných operací.
	**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
	/*
	** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
	** z následujících funkcí nebude volána nad neinicializovaným seznamem).
	** Tato inicializace se nikdy nebude provádět nad již inicializovaným
	** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
	** že neinicializované proměnné mají nedefinovanou hodnotu.
	**/
	// Inicializacia prvkov listu
	L->Act   = NULL;
	L->First = NULL;
}

void DisposeList (tList *L) {
	/*
	** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
	** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
	** uvolněna voláním operace free.
	***/
	tElemPtr tmp; // pomocny pointer na element
	while (L->First != NULL) { // cyklime az kym existuje zaciatocny element
		tmp = L->First; // ulozime prvy element listu
		L->First = L->First->ptr; // posunieme pointer na dalsi element
		free(tmp); // uvolnime ulozeny element
	}
	L->Act = NULL; // v prazdnom liste sa nenachadza ziadny aktivny element
}

void InsertFirst (tList *L, int val) {
	/*
	** Vloží prvek s hodnotou val na začátek seznamu L.
	** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
	** volá funkci Error().
	**/
	tElemPtr new_element; // pomocny pointer na element
	new_element = malloc(sizeof(struct tElem)); // kontrola alokacie
	if (new_element == NULL) { // alokacia sa neuskutocnila
		Error();
		return;
	}
	else { // alokacia prebehla v poriadku
		new_element->data = val; // nastavime hodnotu elementu
		new_element->ptr  = L->First; // namapujeme ho
		L->First = new_element; // posunieme First pointer na neho
	}
}

void First (tList *L) {
	/*
	** Nastaví aktivitu seznamu L na jeho první prvek.
	** Funkci implementujte jako jediný příkaz, aniž byste testovali,
	** zda je seznam L prázdný.
	**/
	L->Act = L->First; // nastavime aktivitu na prvy prvok
}

void CopyFirst (tList *L, int *val) {
	/*
	** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
	** Pokud je seznam L prázdný, volá funkci Error().
	**/
	if (L->First == NULL) { // prazdny list
		Error();
		return;
	}
	else { // list obsahuje elementy
		*val = L->First->data; // priradime hodnotu prveho elementu
	}
}

void DeleteFirst (tList *L) {
	/*
	** Zruší první prvek seznamu L a uvolní jím používanou paměť.
	** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
	** Pokud byl seznam L prázdný, nic se neděje.
	**/
	if (L->First == NULL) { // prazdny list
		return;
	}
	else { // list obsahuje elementy
		if (L->First == L->Act) { //aktivita sa straca
			L->Act = NULL;
		}
		// rusime prvy element a presmerujeme pointer first
		tElemPtr tmp;
		tmp = L->First;
		L->First = L->First->ptr;
		free(tmp); // uvolnime prvy element
	}
}

void PostDelete (tList *L) {
	/*
	** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
	** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
	** nic se neděje.
	**/
	if (L->Act == NULL || L->Act->ptr == NULL) { // list je neaktivny alebo je aktivny posledny element
		return;
	}
	else {
		// rusime posledny element a presmerujeme pointer Act
		tElemPtr tmp;
		tmp = L->Act->ptr;
		L->Act->ptr = tmp->ptr;
		free(tmp); // uvolnime posledny element
	}
}

void PostInsert (tList *L, int val) {
	/*
	** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
	** Pokud nebyl seznam L aktivní, nic se neděje!
	** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
	** zavolá funkci Error().
	**/
	if (L->Act == NULL) { // list je neaktivny
		return;
	}
	tElemPtr tmp; // pomocny pointer na element
	tmp = malloc(sizeof(struct tElem)); // alokacia pamate
	if (tmp == NULL) { // alokacia sa neuskutocnila
		Error();
		return;
	}
	// vlozime element a presmerujeme pointre
	tmp->data = val;
	tmp->ptr  = L->Act->ptr;
	L->Act->ptr = tmp;

}

void Copy (tList *L, int *val) {
	/*
	** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
	** Pokud seznam není aktivní, zavolá funkci Error().
	**/
	if (L->Act == NULL) { // list je neaktivny
		Error();
		return;
	}
	else { // vraciame hodnotu aktivneho prvku
		*val = L->Act->data;
	}
}

void Actualize (tList *L, int val) {
	/*
	** Přepíše data aktivního prvku seznamu L hodnotou val.
	** Pokud seznam L není aktivní, nedělá nic!
	**/
	if (L->Act == NULL) { // list je neaktivny
		return;
	}
	else {
		L->Act->data = val; // prepiseme data aktivneho elementu
	}
}

void Succ (tList *L) {
	/*
	** Posune aktivitu na následující prvek seznamu L.
	** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
	** Pokud není předaný seznam L aktivní, nedělá funkce nic.
	**/
	if (L->Act == NULL) { // list je neaktivny
		return;
	}
	else {
		if (L->Act->ptr == NULL) { // ak je aktivny posledny prvok aktivita sa straca
			L->Act = NULL;
			return;
		}
		else { // posunieme aktivitu dalej
			L->Act = L->Act->ptr;
		}
	}
}

int Active (tList *L) {
	/*
	** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
	** Tuto funkci je vhodné implementovat jedním příkazem return.
	**/
	// ak je list aktivny vracia true=1 ak nie false=0
	return ((L->Act != NULL) ? TRUE : FALSE);
}

/* Konec c201.c */
