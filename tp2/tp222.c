//process 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

struct msgbuf {
    int m_int;
};

int main() {
    int msg; // stocker temporairement la chaine a lire
    //creation semaphore lecture
    int key = 100;
    int sem;
    sem = semget((key_t)key, 1, 0666 | IPC_CREAT);
    /*//initialisation
    union semun arg;
    arg.val = 1; 
    semctl(sem, 0, SETVAL, arg);*/
    //creation memoire partagé pour le semaphore
    struct sembuf buf;
    //buf.sem_num = 0;
    //buf.sem_flg = 0;
    //buf.sem_op = 1;
    
    
    //creation semaphore ecriture
    int key2 = 101;
    int sem2;
    sem2 = semget((key_t)key2, 1, 0666 | IPC_CREAT);
    //initialisation
    union semun arg2;
    arg2.val = 1; 
    semctl(sem2, 0, SETVAL, arg2);
    //creation memoire partagé pour le semaphore
    struct sembuf buf2;
    buf2.sem_num = 0;
    buf2.sem_flg = 0;
    buf2.sem_op = 1;
    
    //creation espace partagé
    int key3 = 200;
    int shmid = shmget(key3, sizeof(struct msgbuf), 0666 | IPC_CREAT);
    struct msgbuf * shm;
    shm = (struct msgbuf*)shmat(shmid, 0, 0); // O,O -> acces en lecture/ecriture
    printf("addresse de la mémoire partagée : %p\n", &shm);
    shm->m_int = 0;
    //boucle
    for (int i = 1; i <= 5; i++) {        
        //gestion synchronisation avec les semaphores 
        //on incremente le semaphore
        semop(sem2, &buf2, -1);
        //lecture
        msg = shm->m_int;
        printf("lecture : %d\n", msg);
        //semop(sem, &buf, 1);
        //ecriture
        //semop(sem2, &buf2, -1);
        shm->m_int += 1;
        printf("ecriture : %d\n", shm->m_int);
        semop(sem, &buf, 1);
        //semop(sem2, &buf2, 1);
        
    }

    //detruire le semaphore et la memoire partagée
    semctl(sem, 0, IPC_RMID, 0);
    //semctl(sem2, 0, IPC_RMID, 0);
    shmdt(&shm);
    return 0;
}
