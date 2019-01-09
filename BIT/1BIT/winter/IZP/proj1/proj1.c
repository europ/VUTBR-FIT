/*
 * proj1.c
 *
 * Tento program sluzi na jednoduche spracovanie slov. Program detekuje kladne cislo, datum a slovo.
 * Uvadza aj palindrome ak sa jedna o slovo, prime ak sa jedna o cislo. Datum musi byt vo formate
 * DDDD-DD-DD kde D predstavuje cislo od 0 po 9.
 *
 * Adrian Toth (xtotha01), 28.10.2015
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

// funkcia ktora vypise funkciu tohto programu, nevracia ziadnu hodnotu
void function_help() {
    printf("Tento program sluzi na filtrovanie slov (date/word/number) a ich presnejsie urcenie (palindrom/prime).\n");
}

// funkcia ktora vracia hodnotu true(number), false(word)
bool function_number_check(char *str) { // odovzdava sa do funkcie hodnota pointra str
    char *ptr = str; // priradenie
    while (*ptr != '\0') {  // while cyklus ktory konci ak narazi na ukoncovaci znak retazca
        if (!isdigit(*ptr)) return false; // kontrola znaku, ak to nie je cislo tak vracia hodnotu false
        ptr++;
    }
    return true;
}

/*
 * funkcia ktora vyhodnocuje cislo vzhladom na INT_MAX, true(cislo <= INT_MAX), false(cislo > INT_MAX)
 * INT_MAX je najvacsia mozna hodnota ktoru moze premennna typu integer nadobudnut
 */
bool function_intmax_check(char *str, int *num) {
    // [strtol(input string, adress of invalid character, number type)],strtol = string to long integer function
    if ((strtol(str, NULL, 10) <= INT_MAX) && (errno != ERANGE)) {
        /* AK (cislo <= INT_MAX a nepretiekol long int) tak prirad hodnotu z funkcie ktora prekonvertuje
           string na integer */
        *num = atoi(str);
        return true;
    }
    else return false;
}

// funkcia ktora vracia integer hodnotu zodpovedajucej dlzky vstupu str[101]
int function_string_length(char *str) {
    int length = 0;
    while (str[length] != '\0') {  // hladanie konca retazca, koniec retazca je vzdy znaceny zakom \0
        length++;
    }
    return length; // vracia integer hodnotu dlzky
}

// funkcia ktora kontroluje znak po znaku, z 10(0-9) znakov 4. a 7. su pomlcky ostatne su cisla
bool function_date_input_type_check(char *str) {
    // predosla podmienka s funkciou v function_date_check kontroluje dlzku(10znakov), ak to splnilo musi mat 10 znakov
    for (int i = 0; i < 10; i++) {
        switch (i) {
            case 4:
            case 7:
                if (str[i]!='-') return false; // ak 4. a 7. znak nie je pomlcka tak vrat false hodnotu
                break;
            default:
                if (!isdigit(str[i])) return false; // ak ostatne znaky nie su cisla tak vrat false hodnotu
                break;
        }
    }
    return true; // vstup zodpoveda tvaru NNNN-NN-NN (N=number)
}

/*
 * funkcia ktora vyhodnocuje datum
 * funkcia mktime() pracuje spravne ak vsetupny datum je v intervale <01.01.1970; 19.01.2038>, ak vsupny datum
 * nie je z tohto intervalu vracia hodnotu -1
 */
bool function_date_check(char *str) {
    struct tm time_info;
    char *dni[] = {"Sun", "Mon", "Tue", "Wen", "Thu", "Fri", "Sat"};
    if (function_string_length(str) == 10) { // datum je dlhy 10 znakov
        if (function_date_input_type_check(str) == true) { // datum je v tvare NNNN-NN-NN (N=number)
            int my_day = ((str[8]-'0')*10 + (str[9]-'0')*1);
            int my_month = ((str[5]-'0')*10 + (str[6]-'0')*1);
            int my_year = ((str[0]-'0')*1000 + (str[1]-'0')*100 + (str[2]-'0')*10 + (str[3]-'0')*1);
            if (my_month > 12) return false; // mesiac > 12
            if (my_day > 31) return false; // den > 31
            time_info.tm_sec = 0;
            time_info.tm_min = 0;
            time_info.tm_hour = 12;
            time_info.tm_year = (my_year - 1900);
            time_info.tm_mon = (my_month - 1);
            time_info.tm_mday = my_day;
            if (mktime(&time_info) != -1 ) { // mktime nehlasi chybu (datum bol z intervalu <01.01.1970; 19.01.2038>)
                printf("date: %s %s\n", dni[time_info.tm_wday], str);
            }
            else return false; // mktime hlasi chybu tj. -1 (datum nebol z intervalu <01.01.1970; 19.01.2038>)
            return true;
        }
    }
    return false;
}

// funkcia ktora vracia hodnotu true(je-palindrom), false(nie-je-palindrom)
bool function_palindrome(char *str) {
    int length = function_string_length(str); // priradenie hodnoty(dlzka retaca) do premennej
    int half = length / 2;
    for (int i = 0; i < half; ++i) {
        if (str[i] != str[length-i-1]) return false; // kontrola znakov po stred slova
    }
    return true; // ak sa znaky rovnaju vracia true
}

// funkcia ktora vracia hodnotu true(je-prvocislo), false(nie-je-prvocislo)
bool function_prime(int number) {
    if (number < 2) return false;
    for (int i = 2; i < number; i++) { // 2=najmensie prvocislo
        // %=MOD, ak nie je zvysok false, (prvocislo MOD cislo [bez 1 a sameho seba]) = dostavame zvysok
        if (number % i == 0) {
            return false;
        }
    }
    return true; // vsade bol zvysok => je to prvocislo => vracia hodnotu true
}

/*
 * funkcia v ktorej prebieha filtracia v poradi date, number(is-prime, is-not-prime),
 * word(is-palindrome, is-not-palindrome)
 */
int function_run_program() {
    char str[101]; //str[101] = buffer
    int num;
    while (scanf("%100s", str) != EOF) { // nacitavanie slova po slove az po EOF
        if (function_date_check(str)){ // DATE
        }
        else { // NOT-DATE
            if (function_number_check(str)) { // NUMBER
                num = -1;
                if (function_intmax_check(str, &num)) { // NUMBER < INTMAX => (prime filter)
                    if (function_prime(num)) { // NUMBER-PRIME
                        printf("number: %d (prime)\n", num);
                    }
                    else { // NUMBER-NOT-PRIME
                        printf("number: %ld\n", strtol(str, NULL, 10));
                    }
                }
                else { // NUMBER > INTMAX => (skip prime filter)
                    printf("number: %s\n", str);
                }
            }
            else { // NOT-NUMBER => WORD
                if (function_palindrome(str)) { // WORD-PALINDROME
                        printf("word: %s (palindrome)\n", str);
                    }
                else { // WORD-NOT-PALINDROME
                    printf("word: %s\n", str);
                }
            }
        }
    }
    return 0;
}

// hlavna funkcia
int main(int argc, char *argv[]) {
    (void)argv; // pretypovanie argv na void pretoze argv nie je nikde pouzite v programe
    if (argc > 1) { // pocet argumentov > 1 tak program vypise svoj kratky popis
        function_help();
        return 0; // vypisal svoj kratky popis tak sa ukonci
    }
    else { // program bol spusteny s jednym arguemtom tak nacitava data zo vstupu a spracovava ich
        return function_run_program();
    }
}
