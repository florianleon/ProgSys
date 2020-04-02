#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// union semun {
// int val; /* valeur pour SETVAL */
// struct semid_ds *buf; /* tampon pour IPC_STAT, IPC_SET */
// unsigned short int *array; /* tableau pour GETALL, SETALL */
// struct seminfo * buff; /* tampon pour IPC_INFO */
// };

int main() {
    char chaine[1000];
    //creation semaphore
    int key = 100;
    int sem;
    sem = semget((key_t)key, 1, 0666 | IPC_CREAT);
    //initialisation
    union semun arg;
    arg.val = 1; //initialiser à 1
    semctl(sem, 0, SETVAL, arg);
    //creation memoire partagé pour le semaphore
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_flg = 0;
    buf.sem_op = 1;

    //boucle
    for (int i = 0; i < 5; i++) {
        //attente semaphore : P(S)
        printf("Attente semaphore\n");
        semop(sem, &buf, -1);
        printf("J'ai le semaphore\n");
        // wait(&sem);
        //saisie chaine 
        printf("Saisir chaine : \n");
        scanf("%s", chaine);
        //liberation semaphore : V(S)
        printf("Libération semaphore\n");
        semop(sem, &buf, 1);
        printf("Semaphore liberé\n");
        // signal(&sem);
        printf("Chaine saisie : %s\n", chaine);
    }

    //detruire le semaphore
    semctl(sem, 0, IPC_RMID, 0);
    return 0;
}