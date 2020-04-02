#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

sigset_t mask1, mask2;
int pfid, status;

void handler1(int sig){ //handler pour le pere
    printf("Handler du pere\n");
    printf("Fin du programme !\n");
    exit(0);
}

//http://www-igm.univ-mlv.fr/~dr/NCS/node178.html
void handler2(int sig) { //handler pour le gerer la mort du fils par le pere
    printf(" Entree dans le handler\n");
    system("ps");                 /* affichage avec etat zombi du fils */
    if ((pfid = wait(&status)) == -1) /* suppression du fils zombi */
    {
         perror("wait handler ");
         return ;
    }
    printf(" wait handler pfid: %d  status %d \n", pfid, status);
    return;
}

void handler3(int sig){ //handler pour le fils
    printf("Handler du fils\nFin du processus fils (normalement)\n");
    exit(0); //je fais l'hypothese que le exit(0) envoie automatiquement un sigchld au pere
}


int main() {
    int pid, fid;
    pid = getpid();
    switch(fid = fork()) {
        case -1: printf("erreur fork()\n");
        case 0: //fils
            printf("fils\n");
            //mise en place du masque pour le fils
            sigfillset(&mask2);
            sigdelset(&mask2, SIGUSR1);
            sigprocmask(SIG_SETMASK, &mask2, NULL);
            signal(SIGUSR1, handler3);
            printf("Masque fils OK\n");
            //attente du signal SIGUSR1
            printf("\nEntrée dans le while du fils\n");
            while(1) {}
        default: 
            printf("pere\n");
            break;
    }
    //mise en place du masque pour le pere
    sigfillset(&mask1);
    sigdelset(&mask1, SIGUSR1);
    sigdelset(&mask1, SIGCHLD);
    sigprocmask(SIG_SETMASK, &mask1, NULL);
    signal(SIGUSR1, handler1);
    signal(SIGCHLD, handler2);
    printf("Masque pere OK\n");
    //endormissement pendant 10s 
    sleep(10);
    //envoi du signal au fils 
    printf("FID : %d\n", fid);
    if (kill(fid, SIGUSR1) == 0) { 
        printf("SIGUSR1 envoyé au fils !\n");
    } else {
        printf("Echec kill fils\n");
    }
    //attente du signal SIGUSR1
    printf("\nPID : %d\n", pid);
    printf("Entrée dans le while du pere\n");
    while(1) {}

    return (0);
}

