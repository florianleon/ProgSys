# Programation Système
> TP de prog système pour l'INSA Toulouse
---
## TP1
> Aucun problème particulier normalement
> Le code à été testé
> Seul le calcul n'a pas été fait correctement il a été bien fait dans la suite des TP
---
## TP2
> La première partie est correcte normalement on observe bien un probleme de synchronisation
Ne pas tenir compte des fichiers **tp221.c** et **tp222.c**, je l'ai abandonné pour le réécrire proprement
Du coup, **tp23.c** est le fichier à considerer pour la deuxieme partie avec les SHM

La commande à taper dans le terminal pour le lancer est : 
    
    ./tp23 process key keySHM initValue N
```c
//il faut le process1 s'execute en premier car c'est lui qui initialise les semaphores
    int process = 1;
    int key = 100;
    int keySHM = 200;
    int initValue = 0;
    int N = 0;
```

---

## TP3
> J'ai utilié une partie du TP sur les matrice qu'on a fait en architecture matérielle
Le code à l'air de marcher correctement

La commande à taper dans le terminal pour le lancer est : 

    ./tp3 methode nbthread N
    
```c
    int methode = 1 ou 2 pour ligne ou colonne
    int nbthread = 1..4 pour le nombre le thread à créer
    int N = 100..1000 pour le nombre d'itération à réaliser
```