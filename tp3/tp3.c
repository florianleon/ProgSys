#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define D 1000

//init
int vect[D];
int mat[D][D];
int res[D];
int N;
int nbthread = 0;
int nbiter = 0;

//init matrice vect et res (j'ai repris mon code d'archi mat
//Poiur l'instant je vais initialiser mat et vect avec des 1 et res avec des 0
void init() {
    for (int i = 0; i < D; i++) {
        vect[i] = 1;
        res[i] = 0;
        for (int j; j < D; j++) {
            mat[i][j] = 1;
        }
    }
}

/** 
 * CAS 1
 * V(n+1) = A*V(n) 
 * (multiplication en manipulant les lignes des matrices)
*/

void * calculLigne(void *arg) {
    int noth = *(int *)arg; //numéro du thread
    int deb = noth * D / nbthread; //indice de debut du calcul (n)
    int fin = (noth + 1) * D / nbthread; //indice de fin du calcul (n+1)
    //on a une erreur noth = nbthread - 1 non ? 


    for(int i = deb; i < fin; i++) {
        for(int j = 0; j < D; j++) {
            res[i] += vect[j]*mat[i][j];
            nbiter++;
        }
    }
    pthread_exit(NULL); 
}

/**
 * CAS 2
 * Vt(n+1)=Vt(n)*A
 * (multiplication en manipulant les colonnes des matrices
 * je peux reutiliser le code de calculLigne en modifiant juste certains elements
*/

void * calculColonne(void *arg) {
    int noth = *(int *)arg; //numéro du thread
    int deb = noth * D / nbthread; //indice de debut du calcul (n)
    int fin = (noth + 1) * D / nbthread; //indice de fin du calcul (n+1)
    //on a une erreur noth = nbthread - 1 non ? 


    for(int i = deb; i < fin; i++) {
        for(int j = 0; j < D; j++) {
            res[i] += vect[j]*mat[j][i];
            nbiter++;
        }
    }
    pthread_exit(NULL); 
}


int main (int argc, char * argv[]) {

    int *noThread; //garder le numéro du thread
    pthread_t *tid; //tableau des TID
    struct timeval tf, td; 
    float temps;
    /**
     * Arguments à rentrer dans cet ordre :
     * 1 ou 2 pour calcul en ligne ou en colonne
     * nombre de thread entre 1 et 4
     * nombre d'itération entre 100 et 1000
    */

    if (argc != 4) {
        printf("Nombre d'arguments incorrect !\n");
        exit(1);
    }

    int methode = atoi(argv[1]);
    nbthread = atoi(argv[2]);
    N = atoi(argv[3]);

    gettimeofday(&td, NULL);
    init();
    printf("init OK\n");

    //init tid et noThread
    tid = malloc(sizeof(pthread_t) * nbthread);
    noThread = malloc(sizeof(int) * nbthread);

    for(int i = 0; i < nbthread; i++) {
        noThread[i] = i;
    }

    for(int i = 0; i < N; i++) {
        for(int j = 0; i < nbthread; i++){
            if (methode == 1) {
                pthread_create(&tid[i], NULL, calculLigne, (void*)&noThread[i]);
            } else if (methode == 2) {
                pthread_create(&tid[i], NULL, calculColonne, (void*)&noThread[i]);
            } else {
                exit(1);
            }
        } 
    }

    //j'utilise des pthread_join pour attendre la fin de tous les
    //threads sans les détruire
    for (int i = 0; i < nbthread; i++) {
        pthread_join(tid[i], NULL);
    }
    gettimeofday(&tf, NULL);
    temps = (float)(tf.tv_sec * 1000000 + tf.tv_usec - (td.tv_sec * 1000000 + td.tv_usec))/(float)1000000;
    printf("Temps avec %d thread: %f\n", nbthread, temps);

    return 0;

}

