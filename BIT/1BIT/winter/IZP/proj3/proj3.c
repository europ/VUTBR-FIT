/*
 * IZP
 * Projekt 3
 * Adrian Toth (xtotha01)
 * 13.12.2015
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */

/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/slsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <stdbool.h>
#include <ctype.h> // isdigit
#include <errno.h>
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    // cap = 0
    if (cap == 0) {
        c->size = 0;
        c->capacity = 0;
        c->obj = NULL;
        return;
    }
    else {
        c->obj = malloc(cap*sizeof(struct obj_t));
        // nedostatok miesta pre pole objektov
        if (c->obj == NULL) {
            c->size = 0;
            c->capacity = 0;
            return;
        }
        else {
            c->size = 0;
            c->capacity = cap;
        }
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    /*
     * Zrusime CLUSTER
     * size = 0
     * capacity = 0
     * obj = NULL
     */
    c->size = 0;
    c->capacity = 0;
    free(c->obj);
    c->obj = NULL;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);
    if (c->capacity >= new_cap)
        return c;
    size_t size = sizeof(struct obj_t) * new_cap;
    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;
    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // ak nie je volny ani jeden objekt
    if (c->size == c->capacity) {
        // vytvori novy prazdny objekt na konci
        resize_cluster(c, (c->capacity + CLUSTER_CHUNK));
    }
    // do prazdneho objektu prida objekt (parameter funkcie)
    c->obj[c->size] = obj;
    // pridal objekt => zvacsil sa pocet obsadenych objektov
    c->size += 1;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);


/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    // vklada objekty z c2 za objekty ktore su uz v c1
    for (int i = 0; i < c2->size; i++) {
        append_cluster(c1, (c2->obj[i]));
    }
    // zoradi vsetky objekty od najmensieho po najvacsi v c1 (uz po prepisani objektov z c2 do c1)
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    /*
     * Priravi CLUSTER => odstrani vsetky obsahujuce objekty
     * size = 0
     * cap = 0
     * obj = NULL
     */
    clear_cluster(&carr[idx]);
    /*
     * z pola CLUSTER-ov sme odstranili 1 CLUSTER a teraz vsetky posunieme o 1 aby
     * nebol volny slot medzi nimi
     */
    for (int i = idx; i < (narr-1); i++) {
        carr[i] = carr[i+1];
    }
    // vrati novy pocet CLUSTER-ov
    return (narr-1);
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    // implementovany vzorec
    return (sqrtf(((o1->x - o2->x)*(o1->x - o2->x)) + ((o1->y - o2->y)*(o1->y - o2->y))));
}

/*
 Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
 souseda.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    float w;
    float min = obj_distance(&c1->obj[0],&c2->obj[0]);
    /*
     * Kazdy objekt z CLUSTER 1 porovna s kazdym objektom z CLUSTER 2
     */
    for (int j = 0; j < c1->size; j++) {
        for (int k = 0; k < c2->size; k++) {
            w = obj_distance(&c1->obj[j], &c2->obj[k]);
            // hladanie najkratsej vzdialenosti medzi objektom z C1 a objektom z C2
            if (w < min) {
                min = w;
            }
        }
    }
    // vracia najmensiu vzdialenost ktoru naslo pri porovnavani objektov z C1 a C2
    return min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
 identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
 pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    float distance;
    float w = cluster_distance(&carr[0], &carr[1]);
    /*
     * Kazdy CLUSTER kotronluje s kazdym.
     */
    for (int i = 0; i < (narr-1); i++) {
        for (int j = (i+1); j < narr; j++) {
            // hladanie najblizsich clusterov (zhlukov)
            distance = cluster_distance(&carr[i], &carr[j]);
            if (distance <= w) {
                w = distance;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    // file initialization
    FILE *f;
    // file(filename) open
    f = fopen(filename, "r");
    // chybny subor
    if (f == NULL) {
        (*arr) = NULL;
        return -1;
    }
    else {
        // object_count
        int object_count;
        char str[7];
        // object_count = N = number
        if ((fscanf(f,"%6s%d", str, &object_count) != 2) || (strcmp(str,"count=") != 0) || (object_count <= 0)) {
            (*arr) = NULL;
            fclose(f);
            return -2;
        }
        else {
            /*
             * alokacia pamati pre POLE CLUSTER[object_count]
             * kazdy CLUSTER ma prave 1 obj_t
             */
            *arr = malloc(object_count*sizeof(struct cluster_t));
            if (*arr == NULL) {
                fclose(f);
                return -3;
            }
            // inicializacia prechodneho OBJEKTU
            struct obj_t temp_obj;
            // inicializacia premennych ktore su potrebne pre kontrolu (ID, X, Y)
            int checker;
            char str1[25];
            char str2[6];
            char str3[6];
            char *ptr1;
            char *ptr2;
            char *ptr3;
            long int id, x, y;
            bool checker_num = false;
            /*
             * Cyklus ktorym priradim do kazdeho CLUSTER-a prave 1 OBJEKT
             */
            for (int i = 0; i < object_count; i++) {
                // kontrola
                checker = fscanf(f,"%24s %5s %5s\n", str1, str2, str3);
                // ID check
                id = strtol(str1, &ptr1, 10);
                if ((errno == ERANGE) || (*ptr1 != '\0') || (id < INT_MIN) || (id > INT_MAX)) {
                	checker_num = true;
                }
                // X check
                x = strtol(str2, &ptr2, 10);
                if ((errno == ERANGE) || (*ptr2 != '\0') || (x < 0) || (x > 1000)) {
                	checker_num = true;
                }
                // Y check
                y = strtol(str3, &ptr3, 10);
                if ((errno == ERANGE) || (*ptr3 != '\0') || (y < 0) || (y > 1000)) {
                	checker_num = true;
                }
                // wrong id || x || y
                if ((checker != 3) || (checker_num == true)) {
                    for (int k = 0; k < i; k++) {
                        clear_cluster(&((*arr)[k]));
                    }
                    free(*arr);
                    *arr = NULL;
                    fclose(f);
                    return -5;
                }
                //temp_obj = ID, X, Y
                temp_obj.id = id;
                temp_obj.x = x;
                temp_obj.y = y;
                // funkcia alokuje pamat pre CLUSTER s 1 OBJEKTOM (temp_obj)
                init_cluster(&(*arr)[i], 1);
                if (&arr[0][i].obj == NULL) {
                    fclose(f);
                    return -4;
                }
                // funkcia prida OBJEKT do CLUSTERU (kedze je tam 1 volny slot tak do neho)
                append_cluster(&(*arr)[i], temp_obj);
            }
            // file close
            fclose(f);
            // funkcia vracia pocet objektov
            return object_count;
        }
    }
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    // TUTO FUNKCI NEMENTE
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
 * Hlavna funkcia ktora vyhodnocuje vstup (argumenty), zhlukuje.
 */
int main(int argc, char *argv[])
{
    int n;
    if (argc == 2) {
        n = 1;
    }
    else {
        if (argc == 3) {
            char *ptr = NULL;
            n = strtol(argv[2], &ptr, 10);
             if ((errno == ERANGE) || (*ptr != '\0') || (n <= 0) || (n > INT_MAX)) {
                fprintf(stderr, "ERROR!\nIncorrect number value!\n");
                return 1;
             }
        }
        else {
            // bad arguments
            char HELP[] =
            "CHYBA!\n\n"
            "Nevhodne argumetny\n"
            "\nProgram sa spusta nasledovne:\n"
            "1.) file.txt N\n"
            "\tN je cele cislo, N > 0.\n"
            "2.) file.txt\n"
            "\tINFO: Bez argumentu N sa to hodnoti ako N = 1.\n";
            fprintf(stderr, "%s\n", HELP);
            return 1;
        }
    }
    int clusters_size;
    struct cluster_t *clusters;
    clusters_size = load_clusters(argv[1], &clusters);
    if (clusters_size == -1) {
        fprintf(stderr, "ERROR!\nWrong file!\n");
        return 1;
    }
    else {
        if (clusters_size == -2) {
            fprintf(stderr, "ERROR!\nFile does not contain \"count=number\" !\n");
            return 1;
        }
        else {
            if (clusters_size == -3) {
                fprintf(stderr, "ERROR!\nOut of memory (cluster field)!\n");
                return 1;
            }
            else {
                if (clusters_size == -4) {
                    fprintf(stderr, "ERROR!\nCluster initialization failed (cluster)!\n");
                    return 1;
                }
                else {
                    if (clusters_size == -5) {
                        fprintf(stderr, "ERROR!\nWrong data (id,x,y)!\n");
                        return 1;
                    }
                    else {
                        if (n > clusters_size) {
                            fprintf(stderr, "ERROR!\nCount in file < entered number!\n");
                            for (int i = 0; i < clusters_size; i++) {
                                clear_cluster(&clusters[i]);
                            }
                            free(clusters);
                            return 1;
                        }
                        else {
                            // zhlukovanie
                            int a, b;
                            while (clusters_size > n) {
                                find_neighbours(clusters, clusters_size, &a, &b);
                                merge_clusters(&clusters[a], &clusters[b]);
                                clusters_size = remove_cluster(clusters, clusters_size, b);
                            }
                            print_clusters(clusters, clusters_size);
                            // cyklus maze vsetky objekty kazdeho clustra v poli
                            for (int i = 0; i < clusters_size; i++) {
                                clear_cluster(&clusters[i]);
                            }
                            // vymaze pole clustrov
                            free(clusters);
                            return 0;
                        }
                    }
                }
            }
        }
    }
}
