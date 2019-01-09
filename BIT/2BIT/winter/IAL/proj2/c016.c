
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

/*******************
** IAL            **
** 2 domaca uloha **
** Adrian Toth    **
** 2.11.2016      **
*******************/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/
void htInit ( tHTable* ptrht ) {
	for(int i=0; i<HTSIZE; i++) { // cyklus od 0 od HTSIZE-1
		(*ptrht)[i] = NULL; // tvorenie položiek tabulky
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/
tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	int index = hashCode(key); // ziskame index v položkách
	tHTItem *element = (*ptrht)[index]; // ulozime si prvy prvok
	while ((element != NULL) && (element->key != key)) { // prechadzame prvkami a kontrolujeme kluc
		element = element->ptrnext; // prechod zoznamom
	}
	return element; // prvok sa našiel vrati sa prvok, prvok sa nenašiel vrati sa NULL
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/
void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	tHTItem *element = htSearch(ptrht, key); // vyhladame element
	if (element == NULL) { // neexistuje žiadny taky element
		int index = hashCode(key); // ziskame index
		tHTItem *new_element = malloc(sizeof(tHTItem)); // vytvorime si novy element
		if (new_element != NULL) { // uspesne sa vytvoril
			new_element->data = data; // pridame mu data
			new_element->key = key; // pridame mu kluc
			new_element->ptrnext = (*ptrht)[index]; // nech ukazuje na prvy element
			(*ptrht)[index] = new_element; // nech prvy ukazuje na nový element
		}
		// else MALLOC ERROR
	}
	else { // element != NULL, už existuje taky element takže len aktualizuje jeho datovu časť
		element->data = data;
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/
tData* htRead ( tHTable* ptrht, tKey key ) {
	tHTItem *element = htSearch(ptrht, key); // vyhladame element
	if (element != NULL) { // nasiel sa element
		return &(element->data); // vraciame pointer na data
	}
	else { // element sa nenasiel
		return NULL; // vraciame NULL
	}
}


/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/
void htDelete ( tHTable* ptrht, tKey key ) {
	int index = hashCode(key); // ziskame index v položkách
	tHTItem *element = (*ptrht)[index]; // ulozime si prvy element
	tHTItem *before_element = (*ptrht)[index]; // ulozime si predosly element
	if (element != NULL) {
		if (element->key != key) { // kluc nie je na prvom elemenete
			while (element != NULL) {
				if (element->key == key) { // nasli sme kluc
					before_element->ptrnext = element->ptrnext; // nastavime pointre elemenetov
					free(element); // vymazeme element
				}
				// posuvame poradie
				before_element = element;
				element = element->ptrnext;		
			}
		}
		else { // kluc obsahoval prvy element
			element = (*ptrht)[index]; // prvy element si zapamatame
			(*ptrht)[index] = element->ptrnext; // druhy element bude prvy
			free(element); // vymazeme elemenet
		}
	}
}


/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/
void htClearAll ( tHTable* ptrht ) {

	tHTItem *tmp_element; // pomocny pointer na element

	for(int i=0; i<HTSIZE; i++) { // prechadzame polozky
		tmp_element = (*ptrht)[i]; // zaciname prvym elementom, pozor na while podmienku

		while (tmp_element != NULL) { // kym obsahuje elementy
			(*ptrht)[i] = (*ptrht)[i]->ptrnext; // do prveho dame druhy
			free(tmp_element); // vymazeme
			tmp_element = (*ptrht)[i]; // priradime prvy
		}
	}
}
