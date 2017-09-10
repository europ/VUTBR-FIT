/**
 * @brief The Roller Coaster Solution
 * @author Adrian Toth <xtotha01@stud.fit.vutbr.cz>
 * @date April 30, 2016
 * @file proj2.c
 */

/**
 * IOS - Operacni systemy
 * Projekt 2
 * The Roller Coaster Solution
 * Inspiracia z (kapitola 5.7) http://www.greenteapress.com/semaphores/downey08semaphores.pdf
 * Adrian Toth
 * xtotha01
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>

/******************************************************************************************************************/
// global variables (at fork they are cloned)
    int P,              // passengers count
        C,              // capacity of cart
        PT,             // maximalni hodnota doby (v milisekundach), po ktere je generovan novy proces pro pasazera
        RT;             // maximalni hodnota doby (v milisekundach) prujezdu trati
    FILE *file_output;  // file, where is output written

/******************************************************************************************************************/
// shared variables (necessary variables for synchronization)
    int *L,                 // need for sedning message from Little_Boy to MAIN
        *pid_array,         // array of pids
        *K,                 // index of free space in pid_array[]

        *A,                 // number of line
        *boarders,          // order number at boarding
        *unboarders;        // order number at unboarding
    //============================================================================================
    sem_t *mutex,           // ancillary semaphore for boarding
          *mutex_2,         // ancillary semaphore for unboarding
          *board_queue,     // "nastupiste", allowed platform for C passengers who are coming in
          *unboard_queue,   // "vystupiste", allowed platform for C passengers who are coming out
          *all_aboard,      // entrance door to cart, last passenger have to close it
          *all_ashore,      // exit door of cart, last passenger have to close it
          *access_to_file,  // mutex for write into file

          *exit_command,    // sync semaphore, if cart exits he do a sem_post(exit_command) n-times

          *pid_mutex;       // sync the pid saving into pid_array

/******************************************************************************************************************/

void destroy_shared_data();
void destroy_PID_things();

/**
 * @brief      Function called after a fork error which kill every single process from proj2 even MAIN process.
 */
void Little_Boy() {
    int e, my_pid;
    e=(*K)-1;
    my_pid = getpid();
    while (e>0) // kill everybody except MAIN
    {
        if (pid_array[e] == my_pid) // do not kill yourself because you are the killer
        {
            e--; // skip yourself
        }
        else // kill everybody except you and main
        {
            kill(pid_array[e], SIGKILL);
            e--;
        }
    }
    destroy_shared_data();
    *L = 1; // send message to main about that you kill everybody and you will go off (you die too)
    kill(getpid(), SIGKILL); // kill yourself and main will do the remaining job for you (main have to just put exit status to 2 and destroy pid things)
}

/**
 * @brief      At error terminate the program with assigned exit value and with error description.
 *
 * @param[in]  x     Type of error.
 */
void end_with_error(int x) {
    switch (x) {
        case 1: {
            fprintf(stderr,"Wrong arguments!\n");
            exit(1);
            break;
        }
        case 2: {
            fprintf(stderr,"Condition fault (P > 0 and P is integer)!\n");
            exit(1);
            break;
        }
        case 3: {
            fprintf(stderr,"Condition fault (C > 0 and C is integer)!\n");
            exit(1);
            break;
        }
        case 4: {
            fprintf(stderr,"Condition fault (P > C)!\n");
            exit(1);
            break;
        }
        case 5: {
            fprintf(stderr,"Condition fault (P = k*C where k is integer)!\n");
            exit(1);
            break;
        }
        case 6: {
            fprintf(stderr,"Condition fault (PT >= 0 and PT < 5001 and PT is integer)!\n");
            exit(1);
            break;
        }
        case 7: {
            fprintf(stderr,"Condition fault (RT >= 0 and RT < 5001 and RT is integer)!\n");
            exit(1);
            break;
        }
        case 8: {
            fprintf(stderr,"Error in fopen (return value of fopen == NULL)!\n");
            exit(1);
            break;
        }
        case 9: {
            fprintf(stderr,"An error occurred in system call - fork (return value of fork < 0)!\n");
            Little_Boy();
            break;
        }
        case 10: {
            fprintf(stderr,"An error occurred in system call - sem_init (return value of sem_init < 0)!\n");
            exit(2);
            break;
        }
        case 11: {
            fprintf(stderr,"An error occurred in system call - mmap (return value of mmap = MAP_FAILED)!\n");
            exit(2);
            break;
        }
        case 12: {
            fprintf(stderr,"An error occurred in system call - sem_destroy (return value of sem_destroy < 0)!\n");
            exit(2);
            break;
        }
        case 13: {
            fprintf(stderr,"An error occurred in system call - munmap (return value of munmap < 0)!\n");
            exit(2);
            break;
        }
    }
}

/**
 * @brief      Evaluate the correct type of necessary arguments.
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 */
void argument_check(int argc, char *argv[]) {

    // proj2 P C PT RT
    if (argc != 5) {
        end_with_error(1);
    }

    // variable for check strtol
    char *ptr;

    // P > 0
    ptr = NULL;
    long int p;
    p = strtol(argv[1], &ptr, 10);
    if ( (errno == ERANGE) || (*ptr != '\0') || (p <= 0) || (p > INT_MAX) ) {
        end_with_error(2);
    }

    // C > 0
    ptr = NULL;
    long int c;
    c = strtol(argv[2], &ptr, 10);
    if ( (errno == ERANGE) || (*ptr != '\0') || (c <= 0) || (c > INT_MAX) ) {
        end_with_error(3);
    }

    // P > C
    if (p <= c) {
        end_with_error(4);
    }

    // P = k*C
    if (p % c != 0) {
        end_with_error(5);
    }

    // PT >= 0 && PT < 5001
    ptr = NULL;
    long int pt;
    pt = strtol(argv[3], &ptr, 10);
    if ( (errno == ERANGE) || (*ptr != '\0') || (pt < 0) || (pt >= 5001) ) {
        end_with_error(6);
    }

    // RT >= 0 && RT < 5001
    ptr = NULL;
    long int rt;
    rt = strtol(argv[4], &ptr, 10);
    if ( (errno == ERANGE) || (*ptr != '\0') || (rt < 0) || (rt >= 5001) ) {
        end_with_error(7);
    }

    // everything is allright, check passed
    return;
}

/**
 * @brief      Initializate the variables which will be cloned to every single process.
 *
 * @param      argv  The argv
 */
void initialize_global_variables(char *argv[]) {

    P  = strtol(argv[1], NULL, 10);
    C  = strtol(argv[2], NULL, 10);
    PT = strtol(argv[3], NULL, 10);
    RT = strtol(argv[4], NULL, 10);

    file_output = fopen("proj2.out", "w+");
    if (file_output == NULL)
    {
        end_with_error(8);
    }

    return;
}

/**
 * @brief      Removes mapping (shared memory between processes).
 */
void destroy_shared_data() {

    // variables used at checking the sem_destroy and munmap
    int error_munmap      = 0;
    int error_sem_destroy = 0;

    if ( mutex          != NULL ) error_sem_destroy = sem_destroy(mutex);
    if ( mutex_2        != NULL ) error_sem_destroy = sem_destroy(mutex_2);
    if ( board_queue    != NULL ) error_sem_destroy = sem_destroy(board_queue);
    if ( unboard_queue  != NULL ) error_sem_destroy = sem_destroy(unboard_queue);
    if ( all_aboard     != NULL ) error_sem_destroy = sem_destroy(all_aboard);
    if ( all_ashore     != NULL ) error_sem_destroy = sem_destroy(all_ashore);
    if ( access_to_file != NULL ) error_sem_destroy = sem_destroy(access_to_file);
    if ( exit_command   != NULL ) error_sem_destroy = sem_destroy(exit_command);

    if ( mutex          != NULL ) error_munmap = munmap(mutex, sizeof(sem_t));
    if ( mutex_2        != NULL ) error_munmap = munmap(mutex_2, sizeof(sem_t));
    if ( board_queue    != NULL ) error_munmap = munmap(board_queue, sizeof(sem_t));
    if ( unboard_queue  != NULL ) error_munmap = munmap(unboard_queue, sizeof(sem_t));
    if ( all_aboard     != NULL ) error_munmap = munmap(all_aboard, sizeof(sem_t));
    if ( all_ashore     != NULL ) error_munmap = munmap(all_ashore, sizeof(sem_t));
    if ( access_to_file != NULL ) error_munmap = munmap(access_to_file, sizeof(sem_t));
    if ( exit_command   != NULL ) error_munmap = munmap(exit_command, sizeof(sem_t));

    if ( A              != NULL ) error_munmap = munmap(A, sizeof(int));
    if ( boarders       != NULL ) error_munmap = munmap(boarders, sizeof(int));
    if ( unboarders     != NULL ) error_munmap = munmap(unboarders, sizeof(int));

    // error check
    if (error_sem_destroy != 0) end_with_error(12);
    if (error_munmap      != 0) end_with_error(13);

    fclose(file_output);
    // everything is allright, shared memory is now canceled
    return;
}

/**
 * @brief      Creates mapping (shared memory between processes).
 */
void create_shared_data() {

    A              = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    boarders       = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    unboarders     = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // check the fruitfulness of mmap(int)
    if (A == NULL || boarders == NULL || unboarders == NULL)
    {
        destroy_shared_data();
        end_with_error(11);
    }

    *A=1;
    *boarders=0;
    *unboarders=0;

    mutex          = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mutex_2        = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    board_queue    = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    unboard_queue  = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    all_aboard     = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    all_ashore     = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    access_to_file = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    exit_command   = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // check the fruitfulness of mmap(sem_t)
    if ( mutex == NULL      || mutex_2 == NULL    || board_queue == NULL    || unboard_queue == NULL ||
         all_aboard == NULL || all_ashore == NULL || access_to_file == NULL || exit_command == NULL  )
    {
        destroy_shared_data();
        end_with_error(11);
    }

    // variable used for check the success of sem_init
    bool error = false;

    if ( sem_init(mutex,          1, 1) < 0 ) error = true;
    if ( sem_init(mutex_2,        1, 1) < 0 ) error = true;
    if ( sem_init(board_queue,    1, 0) < 0 ) error = true;
    if ( sem_init(unboard_queue,  1, 0) < 0 ) error = true;
    if ( sem_init(all_aboard,     1, 0) < 0 ) error = true;
    if ( sem_init(all_ashore,     1, 0) < 0 ) error = true;
    if ( sem_init(access_to_file, 1, 1) < 0 ) error = true;
    if ( sem_init(exit_command,   1, 0) < 0 ) error = true;

    // check the fruitfulness of sem_init
    if (error == true)
    {
        destroy_shared_data();
        end_with_error(10);
    }

    // everything is allright, shared memory is now created
    return;
}

void destroy_PID_things() {

    // variables used at checking the sem_destroy and munmap
    int error_munmap      = 0;
    int error_sem_destroy = 0;

    if ( pid_mutex != NULL ) error_sem_destroy = sem_destroy(pid_mutex);

    if ( L         != NULL ) error_munmap = munmap(L, sizeof(int));
    if ( K         != NULL ) error_munmap = munmap(K, sizeof(int));
    if ( pid_mutex != NULL ) error_munmap = munmap(pid_mutex, sizeof(sem_t));
    if ( pid_array != NULL ) error_munmap = munmap(pid_array, sizeof(int)*(P+3));

    // error check
    if (error_sem_destroy != 0) end_with_error(12);
    if (error_munmap      != 0) end_with_error(13);

    return;
}

void make_PID_things() {

    L         =   (int*)mmap(NULL, sizeof(int),       PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    K         =   (int*)mmap(NULL, sizeof(int),       PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_array =   (int*)mmap(NULL, sizeof(int)*(P+3), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_mutex = (sem_t*)mmap(NULL, sizeof(sem_t),     PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if ( L == NULL || K == NULL || pid_array == NULL || pid_mutex == NULL )
    {
        destroy_PID_things();
        end_with_error(11);
    }

    *K=0;
    *L=0;

    if ( sem_init(pid_mutex,1, 1) < 0 )
    {
        destroy_PID_things();
        end_with_error(10);
    }

    return;
}

/**
 * @brief      Child of "main". Second fork from main process. Process representating CART.
 */
void vozik() {

    sem_wait(pid_mutex);
    pid_array[*K] = getpid();
    (*K)++;
    sem_post(pid_mutex);

    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: C 1\t: started\n", *A);
        (*A)++;
    sem_post(access_to_file);

    /*****************************************************************************************************************/
    for(int i=0; i<(P/C); i++) // cart turns over the track
    {



            // cart has given signal for launching the load process
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: C 1\t: load\n", *A);
                (*A)++;
            sem_post(access_to_file);
            for(int i=0; i<C; i++)
            {
                sem_post(board_queue);
            }



            // cart has obtained signal that everybody is on board, he can start driving on the track
            sem_wait(all_aboard);
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: C 1\t: run\n", *A);
                (*A)++;
            sem_post(access_to_file);
            if (RT == 0)
            {
                ;
            }
            else
            {
                usleep( (rand() % RT) * 1000 );
            }



            // cart has returned, he has given signal for launching the unload process
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: C 1\t: unload\n", *A);
                (*A)++;
            sem_post(access_to_file);
            for(int i=0; i<C; i++)
            {
                sem_post(unboard_queue);
            }



            // cart has obtained signal that nobody is on board
            sem_wait(all_ashore);



    }
    /*****************************************************************************************************************/

    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: C 1\t: finished\n", *A);
        (*A)++;
    sem_post(access_to_file);

    fclose(file_output);
    return;
}

/**
 * @brief      Child of "generator". Process representating passenger.
 *
 * @param[in]  my_born_number  Order of birth given from parent "generator".
 */
void clovek(int my_born_number) {

    sem_wait(pid_mutex);
    pid_array[*K] = getpid();
    (*K)++;
    sem_post(pid_mutex);

    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: P %d\t: started\n", *A, my_born_number);
        (*A)++;
    sem_post(access_to_file);

    /*****************************************************************************************************************/

    // passenger has obtained signal that he can go on board
    sem_wait(board_queue);
    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: P %d\t: board\n", *A, my_born_number);
        (*A)++;
    sem_post(access_to_file);
    // passenger has to say in which order he came on board
    sem_wait(mutex);
        (*boarders)++;
        if (*boarders == C)
        {
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: P %d\t: board last\n", *A, my_born_number);
                (*A)++;
            sem_post(access_to_file);
            *boarders = 0;
            // the last one have to close the entry door => signal to cart that everybody is on board
            sem_post(all_aboard);
        }
        else
        {
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: P %d\t: board order %d\n", *A, my_born_number, *boarders);
                (*A)++;
            sem_post(access_to_file);
        }
    sem_post(mutex);



    // passenger has obtained signal that he can step off from cart
    sem_wait(unboard_queue);
    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: P %d\t: unboard\n", *A, my_born_number);
        (*A)++;
    sem_post(access_to_file);
    // passenger has to say in which order he stepped off from cart
    sem_wait(mutex_2);
        (*unboarders)++;
        if (*unboarders == C)
        {

            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: P %d\t: unboard last\n", *A, my_born_number);
                (*A)++;
            sem_post(access_to_file);
            *unboarders = 0;
            // the last one have to close the exit door => signal to cart that nobody is on board
            sem_post(all_ashore);
        }
        else
        {
            sem_wait(access_to_file);
                fprintf(file_output, "%d\t: P %d\t: unboard order %d\n", *A, my_born_number, *unboarders);
                (*A)++;
            sem_post(access_to_file);
        }
    sem_post(mutex_2);

    /*****************************************************************************************************************/

    sem_wait(exit_command);

    sem_wait(access_to_file);
        fprintf(file_output, "%d\t: P %d\t: finished\n", *A, my_born_number);
        (*A)++;
    sem_post(access_to_file);

    fclose(file_output);
    exit(0);
}

/**
 * @brief      Child of "main" and parent of every "clovek". First fork from main process.
 */
void generator() {

    sem_wait(pid_mutex);
    pid_array[*K] = getpid();
    (*K)++;
    sem_post(pid_mutex);

    int pid_of_everybody[P];
    int born_order = 0;
    pid_t pid_clovek;

    for(int i=0; i<P; i++)
    {
        born_order++;
        if ( (pid_clovek = fork()) < 0 ) // vytvor cloveka
        {
            end_with_error(9);
        }
        else if (pid_clovek == 0) // process clovek
        {
            clovek(born_order);
        }
        else // process generator
        {
            pid_of_everybody[i] = pid_clovek;
            if (PT == 0)
            {
                ;
            }
            else
            {
                usleep( (rand() % PT) * 1000 );
            }
        }
    }
    for(int j=0; j<P; j++)
    {
        waitpid(pid_of_everybody[j], NULL, 0);
    }
    fclose(file_output);
    exit(0);
}

/**
 * @brief      Parent of every fork. Main process.
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     Success exit value.
 */
int main (int argc, char *argv[]) { // main proces

    argument_check(argc, argv);
    initialize_global_variables(argv);

    make_PID_things();
    sem_wait(pid_mutex);
    pid_array[*K] = getpid();
    (*K)++;
    sem_post(pid_mutex);

    create_shared_data();

    setbuf(file_output, NULL);
    srandom(getpid());

    pid_t pid_vozik;
    pid_t pid_generator;

    if ( (pid_generator = fork()) < 0 ) // vytvor generator
    {
        end_with_error(9);
    }
    else if (pid_generator == 0) // generator proces
    {
        generator();
        exit(0);
    }
    else // main proces
    {
        if ( (pid_vozik = fork()) < 0 ) // vytvor vozik
        {
            end_with_error(9);
        }
        else if (pid_vozik == 0) // vozik proces
            {
                vozik();
                for(int i=0; i<P; i++) // mame P ludi
                {
                    sem_post(exit_command); // prikaz nutny pre ukoncenie 1 cloveka
                }
                exit(0);
            }
        else // main proces
        {
            waitpid(pid_generator, NULL, 0); // main pocka na generator nech sa ukonci
            waitpid(pid_vozik, NULL, 0);     // main pocka na vozik nech sa ukonci

            // if there was a fork error than the killing was done and shared memory was destroyed main have to exit with status 2
            // *L has the message from the past = there was a fork error and we do the job for you, just quit now
            if (*L == 1)
            {
                remove("proj2.out");
                destroy_PID_things();
                exit(2);
            }

            // else (Little_Boy wasn't called) do your job as you do
            // kazdy process skoncil, main process moze skoncit
        }
    }

    destroy_shared_data();

    destroy_PID_things();

    // uz iba main proces existuje, kazdy uz "zomrel", main moze skoncit
    return 0;
}
