#ifndef GRILLEDEF2_H
#define GRILLEDEF2_H

#include "rectangledef.h"

typedef struct Grille_s {
  couleur **bille;
  Rectangle *rec;             /* x,y,largeur,hauteur. Facilite certaines tâches. */
  unsigned numli, numco;      /* Nombre de lignes et Colonnes */
  unsigned case_l, case_h;    /* Largeur et hauteur d'une case */
  unsigned separateur;        /* nombre de pixels séparant chaque case (0 par défaut). */
  couleur fond, surbrillance; /* Couleur de fond, et couleur de subrillance de cases. */
  couleur palette[PAL_TAILLE_MAX];
  char bille_img[PAL_TAILLE_MAX][NOM_TAILLE_MAX];
  int bille_sprite[PAL_TAILLE_MAX];
  unsigned numcouleurs;
  char fond_img[NOM_TAILLE_MAX];/* '\0' si on n'en veut pas. */
  int fond_img_sprite;
  int **selection;            /* Tableau dynamique de coordonnées ayant deux rôles :
			      ** -> Prévention de récursivité infinie
			      ** -> Eviter de tout recalculer tant qu'on reste sur un groupe de même billes
			      ** Convention : selection[foo][0] : ligne
			      **              selection[foo][1] : Colonne
			      */
                              /* Pas un unsigned, parce qu'on veut initialiser à -1. Sinon, la toute première case (0,0) est toujours considérée comme prise. */
  unsigned nbselectionnes;    /* Surtout un index pour le tableau précédent. */
} Grille ;



Grille* nouvelleGrille(unsigned p_x, unsigned p_y,
		       unsigned p_numli, unsigned p_numco,
		       unsigned p_case_l, unsigned p_case_h,
		       unsigned p_separateur,
		       couleur p_fond, couleur p_surbrillance,
		       const couleur p_palette[PAL_TAILLE_MAX], 
		       char p_bille_img[PAL_TAILLE_MAX][NOM_TAILLE_MAX],
		       unsigned p_numcouleurs,
		       const char p_fond_img[NOM_TAILLE_MAX]
		       );

void effacerGrille(Grille *g);
void annulerGrille(Grille *g);
void remplirGrilleAvecFichier(Grille *g, const char *nom_fichier);
void remplirGrilleAvecHasard(Grille *g);
void initialiserDonneesGraphiquesGrille(Grille *g);
void afficherGrille(Grille *g);
void afficherBille(Grille *g, unsigned li, unsigned co);
void voisinsBille(Grille *g, unsigned li, unsigned co);
void dechargerSelection(Grille *g, unsigned li, unsigned co);
void tomberBilles(Grille *g, unsigned li, unsigned co);
int GrilleTerminee(Grille *g);

couleur luminosite(couleur c, short valeur);

/* v Fonctions privées v */

void retirer(Grille *g, unsigned li, unsigned co, couleur teinte);
void tomber(Grille *g);
void decaler(Grille *g);
void voisins(Grille *g, unsigned li, unsigned co, couleur teinte);
int estSelectionnee(Grille *g, unsigned li, unsigned co);
void eclairerBille(Grille *g, unsigned li, unsigned co);
void deseclairerBille(Grille *g, unsigned li, unsigned co);

#endif /* Guards */
