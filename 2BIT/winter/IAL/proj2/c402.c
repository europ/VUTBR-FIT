
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**                                      Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**                                         Úpravy: Radek Hranický, říjen 2016
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

/*******************
** IAL            **
** 2 domaca uloha **
** Adrian Toth    **
** 2.11.2016      **
*******************/


#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)     {
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
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
** Vloží hodnotu na vrchol zásobníku.
**/
{
				 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
	printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
							/* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
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
** Vloží hodnotu na vrchol zásobníku.
**/
				 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
							/* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
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

void BTInit (tBTNodePtr *RootPtr)   {
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	*RootPtr = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
	if ((*RootPtr) != NULL) { // neprázdny strom

		tBTNodePtr old_node     = NULL;       // pointer na otca
		tBTNodePtr current_node = (*RootPtr); // pointer na aktualny uzol
		while (current_node != NULL) {
			if (current_node->Cont < Content) { // obsah je mensi => go RIGHT
				old_node     = current_node;
				current_node = current_node->RPtr;
			}
			else if (current_node->Cont > Content) { // obsah je vacsi => go LEFT
				old_node     = current_node;
				current_node = current_node->LPtr;
			}
			else { // (current_node->Cont == Content) // obsah je rovnaky => do NOTHING
				return;
			}
		}

		tBTNodePtr new_node; // vytvoríme si uzol
		new_node = malloc(sizeof(struct tBTNode));
		if (new_node != NULL) { // uzol bol uspesne vytvorený
			// nastavíme jeho hodnoty
			new_node->LPtr = NULL;
			new_node->RPtr = NULL;
			new_node->Cont = Content;
			if (old_node->Cont < Content) { // pripojime na pravu stranu
				old_node->RPtr = new_node;
			}
			else { // pripojime na lavu stranu
				old_node->LPtr = new_node;
			}
		}

	}

	else { // prázdny strom
		tBTNodePtr new_node; // vytvoríme si uzol
		new_node = malloc(sizeof(struct tBTNode));
		if (new_node != NULL) { // uzol bol uspesne vytvorený
			// nastavíme jeho hodnoty
			new_node->LPtr = NULL;
			new_node->RPtr = NULL;
			new_node->Cont = Content;
			(*RootPtr) = new_node; // uzol pripojime do stromu
		}
	}
}


/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack) {
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/
	while (ptr != NULL) {   // prejdeme všetky uzly
		SPushP(Stack, ptr); // pushni ukazatel na uzol na zasobnik
		BTWorkOut(ptr);     // spracovanie uzla
		ptr = ptr->LPtr;    // go LEFT
	}
}

void BTPreorder (tBTNodePtr RootPtr)    {
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/

	// vytvorenie zásobníka ukazateľov (inicializácia)
	tStackP StackP;
	SInitP(&StackP);

	Leftmost_Preorder(RootPtr, &StackP); // spracovanie najlavejsich uzlov
	
	tBTNodePtr node; // vytvorenie ukazatela na uzol (pomocny)

	while(SEmptyP(&StackP) == FALSE) {          // kým zásobník nie je prázdny
		node = STopPopP(&StackP);               // posledny spracovany uzol nacitame
		Leftmost_Preorder(node->RPtr, &StackP); // spracovanie lavych uzlov praveho podstromu
	}
}


/*                                  INORDER                                   */

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)       {
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku.
**/
	while (ptr != NULL) {   // prejdeme všetky uzly
		SPushP(Stack, ptr); // pushni ukazatel na uzol na zasobnik
		ptr = ptr->LPtr;    // go LEFT
	}
}

void BTInorder (tBTNodePtr RootPtr) {
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/
	// vytvorenie zásobníka ukazateľov (inicializácia)
	tStackP StackP;
	SInitP(&StackP);

	Leftmost_Inorder(RootPtr, &StackP); // spracovanie najlavejsich uzlov

	tBTNodePtr node; // vytvorenie ukazatela na uzol (pomocny)

	while(SEmptyP(&StackP) == FALSE) {         // kým zásobník nie je prázdny
		node = STopPopP(&StackP);              // posledny spracovany uzol nacitame
		BTWorkOut(node);                       // spracujeme uzol
		Leftmost_Inorder(node->RPtr, &StackP); // spracovanie lavych uzlov praveho podstromu
	}
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/
	while (ptr != NULL) { 		// prejdeme všetky uzly
		SPushP(StackP, ptr); 	// pushni ukazatel na uzol na zasobnik
		SPushB(StackB, true); 	// uzol ktory sme prešli tak označíme TRUE
		ptr = ptr->LPtr;		// go LEFT
	}
}

void BTPostorder (tBTNodePtr RootPtr)   {
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/

	// vytvorenie zásobníka ukazateľov (inicializácia)
	tStackP StackP;
	SInitP(&StackP);

	// vytvorenie zásobníka bool hodnôt (inicializácia)
	tStackB StackB;
	SInitB(&StackB);

	tBTNodePtr node;  // vytvorenie ukazatela na uzol (pomocny)

	Leftmost_Postorder(RootPtr, &StackP, &StackB); // spracovanie najlavejsich uzlov bool hodnotami

	while (SEmptyP(&StackP) == FALSE) {   // kým zásobník nie je prázdny
		node = STopPopP(&StackP); 		  // posledny spracovany uzol nacitame
		SPushP(&StackP, node);			  // nacitany uzol vratime ho naspat
		if (STopPopB(&StackB) == FALSE) { // uzol sme ešte nespracovali
			STopPopP(&StackP); 			  // odstranime uzol zo zasobniku
			BTWorkOut(node); 			  // uzol mozeme spracovat
		}
		else { // uzol sme už spracovali
			SPushB(&StackB, FALSE);
			Leftmost_Postorder(node->RPtr, &StackP, &StackB);

		}
	}
}


void BTDisposeTree (tBTNodePtr *RootPtr)    {
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/
	if (*RootPtr != NULL) {
		tStackP StackP; // vytvorime zasovnik
		SInitP(&StackP); // inicializujeme zasobnik
		tBTNodePtr node; // pomocny uzol
		while (SEmptyP(&StackP) == false || *RootPtr != NULL) { // opakuj kym nie je prazdny zasobni alebo kym nie je prazdny strom
			if (*RootPtr != NULL) {
				if ((*RootPtr)->RPtr != NULL) {
					SPushP(&StackP, (*RootPtr)->RPtr); // vlozime na vrchol zasobnika
				}
				node     = *RootPtr; // ulozime si uzol
				*RootPtr = (*RootPtr)->LPtr; // posunieme koren
				free(node); // uvolnime uzol
			}
			else { // *RootPtr == NULL
				if (SEmptyP(&StackP) == FALSE) { // zasobnik nie je prazdny
					*RootPtr = STopPopP(&StackP);
				}
			}
		}
	}
}

/* konec c402.c */

