#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

//j'utilise ce fichier pour creer les deux tubes

int main() {

    struct timeval td, tf;

    //creation des 2 tubes 
    int fd1, fd2;

    //tube pour ecrire 
    if (mkfifo("tube1", S_IRUSR | S_IWUSR | S_IRGRP) == 0) {
        printf("Creation tube1 ok\n");
    } else {
        perror("Tube 1 ");
    }
    //tube pour lire
    if (mkfifo("tube2", S_IRUSR | S_IWUSR | S_IRGRP) == 0) {
        printf("Creation tube2 ok\n");
    } else {
        perror("Tube 2 ");
    }
    
    //on ouvre le premier tube dans le sens ecriture 
    //la synchro est faite avec le open
    printf("attente open\n");
    if ((fd1 = open("tube1", O_WRONLY)) == -1) {
        printf("echec du open 1 : Sortie du programme !\n");
        exit(0);
    }
    // fd1 = open("tube1", O_WRONLY);
    //on ouvre le deuxieme tube dans le sens lecture
    if ((fd2 = open("tube2", O_RDONLY)) == -1) {
        printf("echec du open 2 : Sortie du programme !\n");
        exit(0);
    }
    printf("Fin open\n");
    int buf, buf2 = 0; //buf pour ecrire et buf2 pour lire
    int ret;
    int i = 0;
    gettimeofday(&td, NULL);
    //boucle pour ecrire puis lire 
    while(i<(1000)) {
        ret = write(fd1, &buf, sizeof(int));
        if (ret == -1) {
            perror("write() ");
            exit(EXIT_FAILURE);
        } else {
            printf("write %d ok\n", i);
        }
        ret = read(fd2, &buf2, sizeof(int));
        if (ret == -1) {
            perror("read() ");
            exit(EXIT_FAILURE);
        } else {
            printf("read %d ok\n", i);
        }
        printf("message fils : %d\n", buf2);
        buf = buf2 + 1;
        i++;
    }
    gettimeofday(&tf, NULL);
    float d = ((1000 * 2 * 4 * 8) * pow(10, -6))/((tf.tv_sec - td.tv_sec)*pow(1, 6) + (tf.tv_usec - td.tv_usec));
    printf("Debit : %f\n", d);


    //on ferme les fichiers
    close(fd1);
    printf("fermeture fichier 1\n");
    close(fd2);
    printf("fermeture fichier 2\n");
    
    //on supprime les fichiers
    if (unlink("tube1") == 0) {
        printf("Suppression tube1 ok\n");
    } else {
        perror("Tube 1 ");
    }

    if (unlink("tube2") == 0) {
        printf("Suppression tube2 ok\n");
    } else {
        perror("Tube 2 ");
    }

    return (0);
}

