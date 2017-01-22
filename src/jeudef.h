#ifndef JEUDEF_H
#define JEUDEF_H

#include "rectangledef.h"
#include "grilledef.h"
#include "zonedef.h"
#include "chronodef.h"

/* Un Jeu, c'est une Grille et des Zones de notification. */
typedef struct Jeu_s {
  Rectangle *rec; /* Pour la fenetre */
  Grille* g;
  Zone* zone_score_total;
  Zone* zone_valeur;
  Zone* zone_temps;
  Chrono* chrono;
  unsigned limite_temps;
} Jeu ;

Jeu* jeuDepuisConfiguration(const char *nom_fichier, unsigned *hauteurs_police);

Jeu* nouveauJeu(unsigned p_x, unsigned p_y, unsigned p_l, unsigned p_h, Grille *p_g, Zone *p_zone_score_total, Zone *p_zone_valeur, Zone *p_zone_temps, unsigned p_limite_temps);
void effacerJeu(Jeu *j);

unsigned jouer(Jeu *j);

int ecrireTemps(Jeu *j, unsigned li, unsigned co, unsigned *dsec);
void demarrerTemps(Jeu *j);

#endif /* Guards */
