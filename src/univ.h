#ifndef UNIV_H
#define UNIV_H

#define _POSIX_C_SOURCE 200112L /* Pour snprintf en ANSI C. */
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include <string.h>
#include <time.h>

#define NB_TAILLES_POLICE 3 /* 0,1,2 sont autorisées par la bibiothèque. */
#define PAL_TAILLE_MAX 6 /* Nombre de couleurs max. d'une palette. On se limite à 6 puisque 3 couleurs primaires et 3 couleurs secondaires semblent visuellement équilibré. Et surtout, au delà de 6 couleurs, une grille n'est franchement pas drôle à jouer.*/
#define NOM_TAILLE_MAX 48 /* Taille max. des chemins vers les fichiers. */
#define TOKEN_TAILLE_MAX 48 /* Taille max. de token ramassé lors de la lecture de config. */
#define PSEUDO_TAILLE_MAX 24 /* Auto-explicatif */
#define PSEUDO_DEFAUT "Foobar"
#define FICHIER_CONFIG_INITIAL "data/cfg/cafe.cfg"
#define FICHIER_SCORE_TAILLE_MAX (1<<18) /* Nombre de caractères "soulevables" du fichier de scores.
					 ** On ne peut pas append au milieu d'un fichier, donc on enregistre tout ce qui suit après que l'on souhaite écrire, on écrit, puis on réécrit ce qu'on a soulevé juste après.
					 Ici, nous tenons 2^18 octets de capacité. */
#define AFFICHAGE_CHAINE_TAILLE_MAX 32 /* Taille max. de la chaine pour l'affichage dans les zones in-game. */
#define ECRAN_FOND_GRILLE 1 /* Si plus tard on veut exploiter d'autres écrans, ca économise des nombres magiques */

#endif /* Guards */
