/*
 * proj2.c
 * IZP - 2. projekt
 *
 * Program sluzi na vypocet prirodzeneho logaritmu pomocou matematickych operacii +,-,*,/.
 *
 * Poznamka: BISEKCIA
 *
 * Adrian Toth (xtotha01)
 * 29.11.2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>

/*
 * start_max_interval sa pouziva v:
 * 1.) function_cfrac_log_count
 * 2.) function_taylor_log_count
 *
 * urcuje hornu medzu intervalu z ktoreho sa pocita bijekcia
 * <0 , start_max_interval>
 *
 * cim je start_max_interval vacsie cislo tym je pomalsie, ak je cislo mensie ako UINT_MAX hrozi chyba pri vypocte
 */
#define start_max_interval 10

// funkcia ktora sa spusti v pripade ze vstupne argumenty boli zle zadane
void function_help() {
    // argument input error => printf HELP MESSAGE
    char HELP[] =
    "Chyba (neplatne argumenty)!\n\n"
    "Program sa spusta s nasledovnymi argumentami:\n"
    "1.) --log X N\n"
    "    Pre X musi platit podmienka:\n    1.) X je realne cislo\n    2.) X > 0\n"
    "    Pre N musi platit podmienka:\n    1.) N je cele cislo\n    2.) N > 0\n"
    "2.) --iter MIN MAX EPS\n"
    "    Pre MIN musi platit podmienka:\n    1.) MIN je realne cislo\n    2.) MIN > 0\n"
    "    Pre MAX musi platit podmienka:\n    1.) MAX je realne cislo\n    2.) MAX > 0\n"
    "    Pre EPS musi platit podmienka:\n    1.) EPS je realne cislo\n    2.) EPS >= 1e-12\n";
    fprintf(stderr, "%s\n", HELP);
}

// funkcia ktora vracia absolutnu hodnotu zo vstupnej hodnoty
double function_abs(double a) {
    return (a < 0) ? ((-1)*a) : (a); // if (a<0) then [return ((-1)*a)] else [return (a)]
}

/*
 * funkcia ktora vracia true(string reprezentujuce cislo je z intervalu (DBL_MIN <= number <= DBL_MAX>),
 * false(string reprezentujuce cislo nie je z intervalu (DBL_MIN <= number <= DBL_MAX>) )
 */
bool function_double_check(char *str) {
    char *endptr;
    if (((strtod(str, &endptr)) || (errno == 0)) && (errno != ERANGE) && (endptr != str)) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * funckia ktora zaokruhluje cisla
 */
unsigned int function_round_the_number(double num) {
    unsigned int tmp = num; // priradim cislo num do tmp bez desatinnych cisel
    double num_and_half = tmp + 0.5; // nastavim prve desatinne miesto tmp na 5
    if (num < num_and_half) return num; // porovnavam vtupne cislo s xxx,5 ak je num<xxx,5 tak zaokruhli nadol
    else return (num+1); // ak je num>=xxx,5 tak zaokruhli nahor
}

/*
 * 1.) TAYLOR SERIES
 * 1.) TAYLOROV POLYNOM
 *
 * funkcia ktora sluzi na vypocet prirodzeneho logaritmu pomocou taylorovho polynomu
 * n - udava rozvoj polynomu
 * x - argument prirodzeneho logaritmu
 *
 * A) implementacia taylorovho vzorca pre logaritmus podla [ ln(1-x)= -x -(x^2/2) -(x^3/3) -... -(x^i/i) ]
 * A) plati pre x z intervalu (0, 1>
 *
 * B) implementacia taylorovho vzorca pre logaritmus podla [ ln(x)= SUM[n=1, inf] ( ((x-1)/x)^n / n ) ]
 * B) plati pre x z intervalu (1, inf)
 *
 * SUM - suma
 * inf - nekonecno (infinity)
 */
double taylor_log(double x, unsigned int n) {
    if (n == 0) {
        return 0;
    }
    else {
        // X = 0 => -INF
        if (x == 0) {
            return (-INFINITY);
        }
        else {
            // X < 0 => NAN
            if (x < 0) {
                return NAN;
            }
            else {
                if (x == INFINITY) {
                    return INFINITY;
                }
                else {
                    double citatel;
                    double w;
                    // B)
                    // implementacia sumy
                    if (x > 1) {
                        w = 0;
                        citatel = 1;
                        // implementacia ln(x)= SUM[n=1, inf] ( ((x-1)/x)^n / n )
                        for (unsigned int i = 1; i <= n; i++) {
                            citatel *= ((x-1)/(x));
                            w += (citatel / i);
                        }
                        return w;
                    }
                    // A)
                    // implementacia polynoma
                    else {
                        w = 0;
                        // implementacia ln(1-x)= -x -(x^2/2) -(x^3/3) -... -(x^i/i)
                        x = 1-x; // priradenie argumentu z ln(1-x)
                        citatel = 1;
                        for (unsigned int i = 1; i <= n; i++) {
                            citatel *= x;
                            w -= (citatel/i);
                        }
                        return w;
                    }
                }
            }
        }
    }
}

/*
 * 2.) CONTINUED FRACTION
 * 2.) ZRETAZENY ZLOMOK
 *
 * funkcia ktora sluzi na vypocet prirodzeneho logaritmu pomocou zretezeneho zlomku
 * n - udava rozvoj polynomu
 * x - argument prirodzeneho logaritmu
 * last_denominator - denominator of last fraction - menovatel poslednej najviac vnoreneho zlomku
 * last_fraction - last possible fraction - najviac vnoreny zlomok ktory existuje
 *
 * implementacia zretazeneho zlomku podla
 *  [
 *   ln(1+x/1-x)= ( 2*x / 1-(last_fraction))
 *   last_fraction= ((i-1)^2)*(x^2) / ((i*2)-1)-last_fraction
 *   last_possible_computable_fraction= ((n-1)^2)*(x^2) / ((n*2)-1)
 *  ]
 */
double cfrac_log(double x, unsigned int n) {
    if (n == 0) {
        return 0;
    }
    else {
        // X = 0 => -INF
        if (x == 0) {
            return (-INFINITY);
        }
        else {
            // X < 0 => NAN
            if (x < 0) {
                return NAN;
            }
            else {
                if (x == INFINITY) {
                    return INFINITY;
                }
                else {
                    // priradenie argumentu z ln(1+x/1-x)
                    x = ((x-1)/(1+x));
                    // vypocet cisla v menovateli
                    double last_denominator = ((n*2)-1);
                    // vypocet posledneho zlomku
                    double last_fraction = ( (((n-1)*(n-1)) * (x*x)) / (last_denominator) );
                    for (unsigned int i = (n-1); i > 1; i--) {
                        last_fraction = ( ( ((i-1) * x) * (x * (i-1)) ) / ( ((i * 2) - 1) - (last_fraction) ) );
                    }
                    return ((2*x)/(1-last_fraction)); // posledna iteracia neprebehla preto to treba dopocitat
                }
            }
        }
    }
}

/*
 * iteracie <=> continued fraction
 * funckia ktora sluzi na vypocet pozadovanych iteracii aby
 * odchylka bola mensia ako eps pre zretazeny zlomok
 */
unsigned int function_cfrac_log_count(double min, double max, double eps) {
    // VYPOCET POCTU ITERACII PRE CONTINUED FRACTION POMOCOU BIJEKCE
    double correct_value, middle, difference;
    double low; // spodna krajna hodnota
    double high; // horna krajna hodnota
    unsigned int a;
    unsigned int b;
    /*
     * vypocet potrebneho intervalu (a - b) pre min
     * interval sa zvysuje 10nasobne a pri (1e+8 - 1e+9) sa nastavy na (1e+9 - UINT_MAX)
     */
    // nastavenie pociatocnych hodnot
    a = 1;
    b = start_max_interval;
    while (function_abs(log(min) - cfrac_log(min, b)) > eps) {
        if ((a == 1e+8) && (b == 1e+9)) {
            a = 1e+9;
            b = UINT_MAX;
            if (function_abs(log(min) - cfrac_log(min, b)) > eps) {
                return 0;
                break;
            }
            else {
                break;
            }
        }
        a *= 10;
        b *= 10;
    }
    // vypocet spodnej odchylky (min)
    low = a;
    high = b; // high = UINT_MAX ale dhly je vypocet
    correct_value = log(min); // presny vysledok
    // low = (x) && high = (x+1) => low a high su vzdialene o 1 od seba
    while (function_round_the_number(function_abs(low - high)) >= 1) {
        middle = ((low + high) / 2); // hladanie strednej hodnoty medzi low a high
        if (middle < 1) {
            middle = 1;
            break;
        }
        else {
            difference = function_abs(correct_value - cfrac_log(min, middle));
            if (difference < eps) {
                high = middle;
            }
            else {
                if (difference > eps) {
                    low = middle;
                }
                else {
                    break;
                }
            }
        }
    }
    unsigned int i = function_round_the_number(middle);
    if ((min == INFINITY) || (min == -INFINITY) || (min == NAN) || (min <= 0)) {
        i=1;
    }
    /*
     * vypocet potrebneho intervalu (a - b) pre max
     * interval sa zvysuje 10nasobne a pri (1e+8 - 1e+9) sa nastavy na (1e+9 - UINT_MAX)
     */
    // nastavenie pociatocnych hodnot
    a = 1;
    b = start_max_interval;
    while (function_abs(log(max) - cfrac_log(max, b)) > eps) {
        if ((a == 1e+8) && (b == 1e+9)) {
            a = 1e+9;
            b = UINT_MAX;
            if (function_abs(log(max) - cfrac_log(max, b)) > eps) {
                return 0;
                break;
            }
            else {
                break;
            }
        }
        a *= 10;
        b *= 10;
    }
    // vypocet hornej odchylky (max)
    low = a;
    high = b; // high = UINT_MAX ale dhly je vypocet
    correct_value = log(max); // presny vysledok
    // low = (x) && high = (x+1) => low a high su vzdialene o 1 od seba
    while (function_round_the_number(function_abs(low - high)) >= 1) {
        middle = ((low + high) / 2); // hladanie strednej hodnoty medzi low a high
        if (middle < 1) {
            middle = 1;
            break;
        }
        else {
            difference = function_abs(correct_value - cfrac_log(max, middle));
            if (difference < eps) {
                high = middle;
            }
            else {
                if (difference > eps) {
                    low = middle;
                }
                else {
                    break;
                }
            }
        }
    }
    unsigned int j = function_round_the_number(middle);
    if ((max == INFINITY) || (max == -INFINITY) || (max == NAN) || (max <= 0)) {
        j=1;
    }
    return (i >= j) ? i : j;

    /*
    // VYPOCET POCTU ITERACII PRE CONTINUED FRACTION BEZ POMOCI BIJEKCE
    double correct_value;
    // vypocet spodnej odchylky
    unsigned int i;
    correct_value = log(min);
    for (i = 1; function_abs(correct_value - cfrac_log(min, i)) > eps; i++) {
        ;
    }
    // vypocet hornej odchylky
    unsigned int j;
    correct_value = log(max);
    for (j = 1; function_abs(correct_value - cfrac_log(max, j)) > eps; j++) {
        ;
    }
    return (i >= j) ? i : j;
    */
}

/*
 * iteracie <=> taylor series
 * funckia ktora sluzi na vypocet pozadovanych iteracii aby
 * odchylka bola mensia ako eps pre taylorov polynom
 */
unsigned int function_taylor_log_count(double min, double max, double eps) {
    // VYPOCET POCTU ITERACII PRE TAYLOROV POLYNOM POMOCOU BIJEKCE
    double correct_value, middle, difference;
    double low; // spodna krajna hodnota
    double high; // horna krajna hodnota
    unsigned int a;
    unsigned int b;
    /*
     * vypocet potrebneho intervalu (a - b) pre min
     * interval sa zvysuje 10nasobne a pri (1e+8 - 1e+9) sa nastavy na (1e+9 - UINT_MAX)
     */
    // nastavenie pociatocnych hodnot
    a = 1;
    b = start_max_interval;
    while (function_abs(log(min) - taylor_log(min, b)) > eps) {
        if ((a == 1e+8) && (b == 1e+9)) {
            a = 1e+9;
            b = UINT_MAX;
            if (function_abs(log(min) - taylor_log(min, b)) > eps) {
                return 0;
                break;
            }
            else {
                break;
            }
        }
        a *= 10;
        b *= 10;
    }
    // vypocet spodnej odchylky (min)
    low = a;
    high = b; // high = UINT_MAX ale dhly je vypocet
    correct_value = log(min); // presny vysledok
    // low = (x) && high = (x+1) => low a high su vzdialene o 1 od seba
    while (function_round_the_number(function_abs(low - high)) >= 1) {
        middle = ((low + high) / 2); // hladanie strednej hodnoty medzi low a high
        if (middle < 1) {
            middle = 1;
            break;
        }
        else {
            difference = function_abs(correct_value - taylor_log(min, middle));
            if (difference < eps) {
                high = middle;
            }
            else {
                if (difference > eps) {
                    low = middle;
                }
                else {
                    break;
                }
            }
        }
    }
    unsigned int i = function_round_the_number(middle);
    if ((min == INFINITY) || (min == -INFINITY) || (min == NAN) || (min <= 0)) {
        i=1;
    }
    /*
     * vypocet potrebneho intervalu (a - b) pre min
     * interval sa zvysuje 10nasobne a pri (1e+8 - 1e+9) sa nastavy na (1e+9 - UINT_MAX)
     */
    // nastavenie pociatocnych hodnot
    a = 1;
    b = start_max_interval;
    while (function_abs(log(max) - taylor_log(max, b)) > eps) {
        if ((a == 1e+8) && (b == 1e+9)) {
            a = 1e+9;
            b = UINT_MAX;
            if (function_abs(log(max) - taylor_log(max, b)) > eps) {
                return 0;
                break;
            }
            else {
                break;
            }
        }
        a *= 10;
        b *= 10;
    }
    // vypocet hornej odchylky (max)
    low = a;
    high = b; // high = UINT_MAX ale dhly je vypocet
    correct_value = log(max); // presny vysledok
    // low = (x) && high = (x+1) => low a high su vzdialene o 1 od seba
    while (function_round_the_number(function_abs(low - high)) >= 1) {
        middle = ((low + high) / 2); // hladanie strednej hodnoty medzi low a high
        if (middle < 1) {
            middle = 1;
            break;
        }
        else {
            difference = function_abs(correct_value - taylor_log(max, middle));
            if (difference < eps) {
                high = middle;
            }
            else {
                if (difference > eps) {
                    low = middle;
                }
                else {
                    break;
                }
            }
        }
    }
    unsigned int j = function_round_the_number(middle);
    if ((max == INFINITY) || (max == -INFINITY) || (max == NAN) || (max <= 0)) {
        j=1;
    }
    return (i >= j) ? i : j;

    /*
    // VYPOCET POCTU ITERACII PRE TAYLOROV POLYNOM BEZ POMOCI BIJEKCE
    double correct_value;
    // vypocet spodnej odchylky
    unsigned int i;
    correct_value = log(min);
    for (i = 1; function_abs(correct_value - taylor_log(min, i)) > eps; i++) {
        ;
    }
    // vypocet hornej odchylky
    unsigned int j;
    correct_value = log(max);
    for (j = 1; function_abs(correct_value - taylor_log(max, j)) > eps; j++) {
        ;
    }
    return (i >= j) ? i : j;
    */
}

/*
 * --LOG
 *
 * funkcia ktora vypise udaje pri splneni podmienky argc = 4, argv[1] = "--log"
 */
int function_alternative_main_one(char *argv[]) {
    if (function_double_check(argv[2])) { // (DBL_MIN <= X <= DBL_MAX) => TRUE
        double x = strtod(argv[2], NULL);
        /*
         * (function_string_to_unsigned_int = 0) => N is NOT from (0 < N <= UINT_MAX)
         */
        char *endptr;
        unsigned int n = strtoul(argv[3], &endptr, 10);
        if ((endptr!=argv[3]) && (errno!=ERANGE) && (errno!=EINVAL) && (n<=UINT_MAX) && (argv[3][0] != '-')) {
        printf("       log(%g) = %.12g\n", x, log(x));
        printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
        printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
        return 0;
        }
        else { // (0 <= N <= UINT_MAX) => FALSE
            fprintf(stderr, "Chyba argumentu N!\n\nMusi platit podmienka:\n"
                            "1.) N je cele cislo\n2.) N >= 0\n");
            return 1;
        }
    }
    else { // (DBL_MIN <= X <= DBL_MAX) => FALSE
        fprintf(stderr, "Chyba argumentu X!\n\nMusi platit podmienka:\n"
                        "1.) X je hodnota z intervalu pre datovy typ double\n");
        return 1;
    }
}

/*
 * --ITER
 *
 * funkcia ktora vypise udaje pri splneni podmienky argc = 5, argv[1] = "--iter"
 */
int function_alternative_main_two(char *argv[]) {
    if (function_double_check(argv[2])) { // (DBL_MIN <= MIN <= DBL_MAX) => TRUE
        double min = strtod(argv[2], NULL);
        if (function_double_check(argv[3])) { // (DBL_MIN <= MAX <= DBL_MAX) => TRUE
            double max = strtod(argv[3], NULL);
            if (function_double_check(argv[4])) { // (DBL_MIN <= EPS <= DBL_MAX) => TRUE
                double eps = strtod(argv[4], NULL);
                unsigned int pocet;
                printf("       log(%g) = %.12g\n", min, log(min));
                printf("       log(%g) = %.12g\n", max, log(max));
                pocet = function_cfrac_log_count(min, max, eps);
                if (pocet == 0) {
                    printf("continued fraction iterations = OUT OF RANGE\n");
                }
                else {
                    printf("continued fraction iterations = %d\n", pocet);
                    printf("    cf_log(%g) = %.12g\n", min, cfrac_log(min, pocet));
                    printf("    cf_log(%g) = %.12g\n", max, cfrac_log(max, pocet));
                }
                pocet = function_taylor_log_count(min, max, eps);
                if (pocet == 0) {
                    printf("continued fraction iterations = OUT OF RANGE\n");
                }
                else {
                    printf("taylor polynomial iterations = %d\n", pocet);
                    printf("taylor_log(%g) = %.12g\n", min, taylor_log(min, pocet));
                    printf("taylor_log(%g) = %.12g\n", max, taylor_log(max, pocet));
                }
                return 0;
            }
            else { // (DBL_MIN <= EPS <= DBL_MAX) => FALSE
                fprintf(stderr, "Chyba argumentu EPS!\n\nMusi platit podmienka:\n"
                                "1.) EPS je hodnota z intervalu pre datovy typ double\n");
                return 1;
            }
        }
        else { // (DBL_MIN <= MAX <= DBL_MAX) => FALSE
            fprintf(stderr, "Chyba argumentu MAX!\n\nMusi platit podmienka:\n"
                            "1.) MAX je hodnota z intervalu pre datovy typ double\n");
            return 1;
        }
    }
    else { // (DBL_MIN <= MIN <= DBL_MAX) => FALSE
        fprintf(stderr, "Chyba argumentu MIN!\n\nMusi platit podmienka:\n"
                        "1.) MIN je hodnota z intervalu pre datovy typ double\n");
        return 1;
    }
}

// hlavna funkcia
int main(int argc, char *argv[]) {
    if ((argc == 4) && (strcmp(argv[1], "--log") == 0)) { // VSTUP "--log"
        function_alternative_main_one(argv);
    }
    else {
        if ((argc == 5) && (strcmp(argv[1], "--iter") == 0)) { // VSTUP "--iter"
            function_alternative_main_two(argv);
        }
        else { // CHYBNY VSTUP (chyba v zadani argumentov)
            function_help(); // printf HELP MESSAGE
            return 1;
        }
    }
}
