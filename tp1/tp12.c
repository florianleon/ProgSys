#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>

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
    printf(" wait handler pfid: %d  status %d \n\n", pfid, status);
    return;
}

void handler3(int sig){ //handler pour le fils
    printf("Handler du fils\nFin du processus fils (normalement)\n");
    exit(0); //je fais l'hypothese que le exit(0) envoie automatiquement un sigchld au pere
}


int main() {
    struct timeval td, tf;
    int pid, fid;
    pid = getpid();
    int tube[2];
    //tube[0] : lecture, tube[1] : ecriture
    int tube2[2];
    int msg, msg2 = 0;
    float d;

    //on crée les tubes 
    if (pipe(tube) == -1) {
        perror("creation tube\n");
        exit(42);
    } //ecriture dans le pere et lecture dans le fils
    if (pipe(tube2) == -1) {
        perror("creation tube 2\n");
        exit(42);
    } //lecture dans le pere et ecriture dans le fils
    int i = 0;
    int ret;
    printf("tube[0] : %d\n", tube[0]);
    printf("tube[1] : %d\n", tube[1]);
    printf("tube2[0] : %d\n", tube2[0]);
    printf("tube2[1] : %d\n", tube2[1]);


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
            gettimeofday(&td, NULL);

            while(i<(1000)) {
                ret = read(tube[0], &msg2, sizeof(int));
                if (ret == -1) {
                    perror("read() du fils");
                    exit(EXIT_FAILURE);
                } else {
                    printf("read %d ok\n", i);
                }
                printf("message pere : %d\n", msg2);
                msg = msg2 + 1;
                ret = write(tube2[1], &msg, sizeof(int));
                if (ret == -1) {
                    perror("write() du fils");
                    exit(EXIT_FAILURE);
                } else {
                    printf("write %d ok\n", i);
                }
                i++;
            }
            printf("fin transmission fils\n");
            gettimeofday(&tf, NULL);
            // printf("td : %d, tf : %d\n", tf.tv_usec, td.tv_usec);
            d = ((1000 * 2 * 4 * 8) * pow(10, -6))/((tf.tv_sec - td.tv_sec)*pow(1, 6) + (tf.tv_usec - td.tv_usec));
            printf("Debit fils: %f\n", d);
            //attente du signal SIGUSR1
            printf("\nEntrée dans le while du fils\n");
            while(1) {}
        default: 
            printf("pere\n");
            //mise en place du masque pour le pere
            sigfillset(&mask1);
            sigdelset(&mask1, SIGUSR1);
            sigdelset(&mask1, SIGCHLD);
            sigdelset(&mask1, SIGINT); //pour autoriser le ctrl c et pouvoir debugger plus facilement
            sigprocmask(SIG_SETMASK, &mask1, NULL);
            signal(SIGUSR1, handler1);
            signal(SIGCHLD, handler2);
            printf("Masque pere OK\n");

            gettimeofday(&td, NULL);
            while(i<(1000)) {
                ret = write(tube[1], &msg, sizeof(int));
                if (ret == -1) {
                    perror("write() du pere");
                    exit(EXIT_FAILURE);
                } else {
                    printf("write %d ok\n", i);
                }
                ret = read(tube2[0], &msg2, sizeof(int));
                if (ret == -1) {
                    perror("read() du pere");
                    exit(EXIT_FAILURE);
                } else {
                    printf("read %d ok\n", i);
                }
                printf("message fils : %d\n", msg2);
                msg = msg2 + 1;
                i++;
            }

            // printf("fin transmission pere\n");
            gettimeofday(&tf, NULL);
            // printf("td : %d, tf : %d\n", tf.tv_usec, td.tv_usec);
            d = ((1000 * 2 * 4 * 8) * pow(10, -6))/((tf.tv_sec - td.tv_sec)*pow(1, 6) + (tf.tv_usec - td.tv_usec));
            printf("Debit pere: %f\n", d);

            //endormissement pendant 10s 
            sleep(10);
            signal(SIGUSR1, handler1);
            // envoi du signal au fils 
            printf("\nFID : %d\n", fid);
            if (kill(fid, SIGUSR1) == 0) { 
                printf("SIGUSR1 envoyé au fils !\n");
            } else {
                printf("Echec kill fils\n");
            }
            //attente du signal SIGUSR1
            printf("\nPID : %d\n", pid);
            printf("Entrée dans le while du pere\n");
            while(1) {}
            
    }

    return (0);
}

//gettimeofday() : (end_sec-start_sec)*1e6+(end_usec-start_usec)
// (tf.tv_sec - td.tv_sec)*pow(1, 6) + ((tf.tv_sec - td.tv_sec)*pow(1, 6) + (tf.tv_usec - td.tv_usec))
