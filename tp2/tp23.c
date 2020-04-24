#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>

union semun arg;

//1er processus
int process1(int key, int keySHM, int initValue, int N) {

    //initialisation 
    int * shm = NULL;
    int valeur = initValue;
    int sem1, sem2, shmid;
    struct sembuf buf;
    //init pour le calcul du debit
    struct timeval tf, td, tdiff; 
    float debit = 0, tdiff_usec = 0;
    /*
    * sem1 pour acceder en lecture
    * sem2 pour liberer apres ecriture
    */


    //creation semaphore
    sem1 = semget((key_t) key, 1, 0666 | IPC_CREAT);
    sem2 = semget((key_t) key + 1, 1, 0666 | IPC_CREAT);

    //initialisation des semaphores
    arg.val = 1; 
    semctl(sem1, 0, SETVAL, arg);
    semctl(sem2, 0, SETVAL, arg);
    buf.sem_num = 0;
    buf.sem_flg = 0;
    //buf.sem_op = 1;

    //creation zone de memoire partagée (SHM)
    shmid = shmget((key_t) keySHM, sizeof(int *), 0666 | IPC_CREAT);
    shm = (int *) shmat(shmid, 0, 0);

    /* Boucle */
    //init timer
    gettimeofday(&td, NULL);

    for (int i = 0; i < N; i++) {
        //ecriture SHM
        *shm = valeur;
        printf("Val shm : %d \n", *shm);
        //gestion syncho V2(1)
        buf.sem_op = 1; 
        semop(sem2, &buf, 1);
        //demande d'accés en lecture
        buf.sem_op = -1;
        semop(sem1, &buf, 1);
        //lecture
        valeur = *shm;
        printf("Val shm recu : %d \n", *shm);
        valeur++;
    }

    //fin timer
    gettimeofday(&tf, NULL);
    
    //calcul debit
    tdiff.tv_sec = tf.tv_sec - td.tv_sec;
    tdiff.tv_usec = tf.tv_usec - td.tv_usec;
    tdiff_usec = tdiff.tv_sec * 1000 * 1000 + (tdiff.tv_usec);
    debit = (N * sizeof(int) * 8) / tdiff_usec;
    printf("\nTemps d'execution : %f s\n", tdiff_usec/1000000);
    printf("Nb de bits transmis : %f Kbits\n",(float) (N * sizeof(int) * 8) / 1000);
    printf("Debit : %f Kbits/s\n", debit * 1000);

    //destruction SHM
    shmdt(&shm);
    shmctl(shmid, IPC_RMID, 0);

    //destruction semaphore
    shmctl(sem1, IPC_RMID, 0);
    shmctl(sem2, IPC_RMID, 0);

    return debit * 1000;

}


//2eme processus
int process2(int key, int keySHM, int initValue, int N) {

    //initialisation 
    int * shm = NULL;
    int valeur = initValue;
    int sem1, sem2, shmid;
    struct sembuf buf;
    //init pour le calcul du debit
    struct timeval tf, td, tdiff; 
    float debit = 0, tdiff_usec = 0;
    /*
    * sem1 à liberer apres ecriture
    * sem2 pour acceder en lecture
    */


    //creation semaphore
    sem1 = semget((key_t) key, 1, 0666 | IPC_CREAT);
    sem2 = semget((key_t) key + 1, 1, 0666 | IPC_CREAT);
    buf.sem_num = 0;
    buf.sem_flg = 0;
    /*
    //initialisation des semaphores
    arg.val = 1; 
    semctl(sem1, 0, SETVAL, arg);
    semctl(sem2, 0, SETVAL, arg);
    buf.sem_num = 0;
    buf.sem_flg = 0;
    //buf.sem_op = 1;
    */

    //creation zone de memoire partagée (SHM)
    shmid = shmget((key_t) keySHM, sizeof(int *), 0666 | IPC_CREAT);
    shm = (int *) shmat(shmid, 0, 0);

    /* Boucle */
    //init timer
    gettimeofday(&td, NULL);

    for (int i = 0; i < N; i++) {

        //demande d'accés en lecture
        buf.sem_op = -1;
        semop(sem2, &buf, 1);
        //lecture
        valeur = *shm;
        printf("Val shm recu : %d \n", *shm);
        valeur++;
        //ecriture SHM
        *shm = valeur;
        printf("Val shm : %d \n", *shm);
        //gestion synchro
        buf.sem_op = 1; 
        semop(sem1, &buf, 1);
        
    }

    //fin timer
    gettimeofday(&tf, NULL);
    
    //calcul debit
    tdiff.tv_sec = tf.tv_sec - td.tv_sec;
    tdiff.tv_usec = tf.tv_usec - td.tv_usec;
    tdiff_usec = tdiff.tv_sec * 1000 * 1000 + (tdiff.tv_usec);
    debit = (N * sizeof(int) * 8) / tdiff_usec;
    printf("\nTemps d'execution : %f s\n", tdiff_usec/1000000);
    printf("Nb de bits transmis : %f Kbits\n",(float) (N * sizeof(int) * 8) / 1000);
    printf("Debit : %f Kbits/s\n", debit * 1000);

    //destruction SHM
    shmdt(&shm);
    shmctl(shmid, IPC_RMID, 0);

    //destruction semaphore
    shmctl(sem1, IPC_RMID, 0);
    shmctl(sem2, IPC_RMID, 0);

    return debit * 1000;

}

int main(int argc, char * argv[]) {
    //il faut le process1 s'execute en premier car c'est lui qui initialise les semaphores
    int process = 1;
    int key = 100;
    int keySHM = 200;
    int initValue = 0;
    int N = 0;
    //il faudra appeler la fonction main avec les arguments dans cet ordre

    //on verifie qu'on ait le bon nombre d'argument
    if (argc != 6) {
        printf("Nombre d'arguments incorrect !\n");
        exit(1);
    }
    process = atoi(argv[1]);
    key = atoi(argv[2]);
    keySHM = atoi(argv[3]);
    initValue = atoi(argv[4]);
    N = atoi(argv[5]);
    
    if (process == 1) {
        process1(key, keySHM, initValue, N);
    }
    if (process == 2) {
        process2(key, keySHM, initValue, N);
    }

    return 0;
}