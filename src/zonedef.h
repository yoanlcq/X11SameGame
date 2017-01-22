#ifndef ZONEDEF_H
#define ZONEDEF_H

#include "rectangledef.h"

#define TEMPS_CLIGNOTEMENT_CURSEUR_TEXTE 3 /* En décisecondes */

/* Une Zone de notification, c'est un rectangle avec du texte dedans. */
typedef struct Zone_s {
  Rectangle *rec;
  unsigned taille_police;
  couleur couleur_police, fond;
} Zone ;

Zone* nouvelleZone(unsigned p_x, unsigned p_y, unsigned p_l, unsigned p_h,
		   unsigned p_taille_police, couleur p_couleur_police, couleur p_fond);
void effacerZone(Zone *z);
void afficherZone(Zone *z, char *texte);

void entreeTexteZone(Zone *z, char *str, unsigned size);
/* Permet d'utiliser une Zone comme un champ d'entrée de texte, sur une ligne. */
/* Fonction bloquante jusqu'à appui sur Entrée ou Echap. */

#endif /* Guards */
