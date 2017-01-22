#include "univ.h"
#include "chronodef.h"
#include "confdef.h"
#include "jeudef.h"


Jeu* nouveauJeu(unsigned p_x, unsigned p_y, unsigned p_l, unsigned p_h, Grille *p_g, Zone *p_zone_score_total, Zone *p_zone_valeur, Zone *p_zone_temps, unsigned p_limite_temps) {
  Jeu* nouveau = (Jeu*) malloc(sizeof(Jeu));
  if(nouveau == NULL) {
    fprintf(stderr, "Attention : nouveauJeu() : malloc() a échoué.\n");
    return NULL; /* On lui épargne les assignations */
  }
  nouveau->rec = (Rectangle*) malloc(sizeof(Rectangle));
  if(nouveau->rec == NULL) {
    fprintf(stderr, "Attention : nouveauJeu() : malloc() de Rectangle a échoué.\n");
    free(nouveau);
    return NULL; /* On lui épargne les assignations */
  }
  nouveau->chrono = NULL;
  if(p_limite_temps > 0) {
    nouveau->chrono = (Chrono*) malloc(sizeof(Chrono));
    if(nouveau->chrono == NULL) {
      fprintf(stderr, "Attention : nouveauJeu() : malloc() de Chrono a échoué.\n");
      free(nouveau->rec);
      free(nouveau);
      return NULL; /* On lui épargne les assignations */
    }
  }  
  nouveau->rec->x = p_x;
  nouveau->rec->y = p_y;
  nouveau->rec->l = p_l;
  nouveau->rec->h = p_h;
  nouveau->limite_temps     = p_limite_temps;
  nouveau->g                = p_g;
  nouveau->zone_score_total = p_zone_score_total;
  nouveau->zone_valeur      = p_zone_valeur;
  nouveau->zone_temps       = p_zone_temps;
 
  return nouveau;
}

void effacerJeu(Jeu *j) {
  effacerGrille(j->g);
  effacerZone(j->zone_score_total);
  effacerZone(j->zone_valeur);
  effacerZone(j->zone_temps);
  free(j->chrono);
  free(j);
}

Jeu *jeuDepuisConfiguration(const char *nom_fichier, unsigned *hauteurs_police) {
  Grille *g;
  Jeu *j;
  Config *cfg;
  unsigned hauteur_zones = hauteurs_police[2] + 20;
  
  cfg = nouvelleConfiguration(nom_fichier);
  if(cfg == NULL)
	  return NULL;

  g = nouvelleGrille(0, 
		     hauteur_zones, 
		     cfg->lignes_grille,
		     cfg->colonnes_grille,
		     cfg->largeur_case_grille,
		     cfg->hauteur_case_grille, 
		     cfg->separateur_grille, 
		     cfg->couleur_fond_grille,
		     cfg->couleur_surbrillance_grille, 
		     cfg->palette, 
		     cfg->image_bille, 
		     cfg->numcouleurs,
		     cfg->image_fond_grille);
  
  j = nouveauJeu(0, 0, g->rec->l, g->rec->h+2*hauteur_zones, 
		 g, 
		 nouvelleZone(0, 0, g->rec->l/2, hauteur_zones, 
			      cfg->taille_police_zone_score, cfg->couleur_police_zone_score, cfg->couleur_fond_zone_score),
		 nouvelleZone(g->rec->l/2, 0, g->rec->l/2, hauteur_zones,
			      cfg->taille_police_zone_valeur, cfg->couleur_police_zone_valeur, cfg->couleur_fond_zone_valeur), 
		 nouvelleZone(0, hauteur_zones+g->rec->h, g->rec->l, hauteur_zones,
			      cfg->taille_police_zone_temps, cfg->couleur_police_zone_temps, cfg->couleur_fond_zone_temps),
		 cfg->temps_limite);

  free(cfg); /* c'est une config mallocée. */

  return j;
}


void demarrerTemps(Jeu *j) {
  char chaine[AFFICHAGE_CHAINE_TAILLE_MAX];
  if(j->chrono != NULL) {
    demarrerChrono(j->chrono, j->limite_temps);
    snprintf(chaine, AFFICHAGE_CHAINE_TAILLE_MAX, "%u:%u", j->chrono->sec, j->chrono->dsec);
  } else sprintf(chaine, "--");
  afficherZone(j->zone_temps, chaine);
}

int ecrireTemps(Jeu *j, unsigned li, unsigned co, unsigned *dsec) {
  char chaine[AFFICHAGE_CHAINE_TAILLE_MAX];
  if(j->chrono != NULL) {
    if(lireChrono(j->chrono) == 0) {
      dechargerSelection(j->g, li, co);
      afficherZone(j->zone_valeur, "TIME OUT");
      afficherZone(j->zone_temps, "Click to finish");
      while(!SourisCliquee());
      return 0;
    }
    if(j->chrono->dsec != *dsec) {
      *dsec = j->chrono->dsec;
      snprintf(chaine, AFFICHAGE_CHAINE_TAILLE_MAX, "%u:%u", j->chrono->sec, j->chrono->dsec);
      afficherZone(j->zone_temps, chaine);
    }
  } 
  return 1;
}

unsigned jouer(Jeu *j) {
  char chaine[AFFICHAGE_CHAINE_TAILLE_MAX];
  unsigned score=0, valeur=0, tmp;
  unsigned oldli=0, oldco=0, olddsec=0; /* pour n'actualiser le temps que si nécéssaire */
  Chrono foo;

  afficherGrille(j->g);

  if(GrilleTerminee(j->g) > 0) {
    afficherZone(j->zone_valeur, "(Unplayable)");
    afficherZone(j->zone_temps, "Click to finish");
    while(!SourisCliquee());
    return score;
  }

  afficherZone(j->zone_score_total, "SCORE : 0");

  if(j->limite_temps > 0) {
    afficherZone(j->zone_valeur, "Ready ?");
    afficherZone(j->zone_temps, "Click to start");
    while(!SourisCliquee());
    demarrerChrono(&foo, 40);
    while(foo.sec > 3)
      lireChrono(&foo);
    afficherZone(j->zone_temps, "3...");
    afficherZone(j->zone_valeur, "Ready ?.");
    while(foo.sec > 2)
      lireChrono(&foo);
    afficherZone(j->zone_temps, "2...");
    afficherZone(j->zone_valeur, "Ready ?..");
    while(foo.sec > 1)
      lireChrono(&foo);
    afficherZone(j->zone_temps, "1...");
    afficherZone(j->zone_valeur, "Ready ?...");
    while(foo.sec > 0)
      lireChrono(&foo);
  }

  afficherZone(j->zone_valeur, "0");
  demarrerTemps(j);

  while(1) { /* On sortira par un autre moyen. */
    /* C'est important. Si on en faisait la condition de ce while, la vérification se ferait presque à chaque mouvement de la souris. Nous devons vérifier que la grille est terminee seulement après le clic utilisateur. */


    /* On attend d'entrer dans la Grille */
    while(!sourisDansRectangle(j->g->rec)) {
      SourisPosition();
      if(ecrireTemps(j, oldli, oldco, &olddsec) == 0) return score;
    }

    /* Tracer et mémoriser la sélection */
    oldli = ((_Y-j->g->rec->y)/(j->g->case_h+j->g->separateur));
    oldco = ((_X-j->g->rec->x)/(j->g->case_l+j->g->separateur));
    voisinsBille(j->g, oldli, oldco);

    /* Nécessaire */
    if(j->g->nbselectionnes <= 2) {
      valeur=0;
      afficherZone(j->zone_valeur, "0");
    }

    /* On attend de sortir de la sélection. (ou de cliquer !) */
    while(estSelectionnee(j->g, ((_Y-j->g->rec->y)/(j->g->case_h+j->g->separateur)), 
			  ((_X-j->g->rec->x)/(j->g->case_l+j->g->separateur)))
	  || j->g->nbselectionnes==0) {
      SourisPosition();
      if(ecrireTemps(j, oldli, oldco, &olddsec) == 0) return score;
      /* Si on survole des cases vides et qu'on est dans la Grille... on regarde si y'a du nouveau */
      if(j->g->nbselectionnes==0 && sourisDansRectangle(j->g->rec)) {
	oldli = ((_Y-j->g->rec->y)/(j->g->case_h+j->g->separateur));
	oldco = ((_X-j->g->rec->x)/(j->g->case_l+j->g->separateur));
	voisinsBille(j->g, oldli, oldco);
	continue;
      }
      /* Si la valeur n'est pas l'actuelle... */
      if(j->g->nbselectionnes > 2 && valeur != (j->g->nbselectionnes - 2)*(j->g->nbselectionnes - 2)) {
	valeur = (j->g->nbselectionnes - 2)*(j->g->nbselectionnes - 2);
	snprintf(chaine, AFFICHAGE_CHAINE_TAILLE_MAX, "+%u", valeur);
	afficherZone(j->zone_valeur, chaine);
      }
      if(SourisCliquee() && j->g->nbselectionnes > 1) {
	tomberBilles(j->g, oldli, oldco);
	afficherGrille(j->g);
	score += valeur;
	snprintf(chaine, AFFICHAGE_CHAINE_TAILLE_MAX, "SCORE : %u", score);
	afficherZone(j->zone_score_total, chaine); 
	dechargerSelection(j->g, oldli, oldco); 
	tmp = GrilleTerminee(j->g);
	if(tmp > 0) {
	  if(tmp == 1)
	    afficherZone(j->zone_valeur, "PERFECT !!");
	  else if(tmp<4)
	    afficherZone(j->zone_valeur, "SO CLOSE...");
	  else if(tmp<8)
	    afficherZone(j->zone_valeur, "YOU TRIED");
	  else
	    afficherZone(j->zone_valeur, "DONE");
	  afficherZone(j->zone_temps, "Click to finish");
	  while(!SourisCliquee());
	  return score;
	}
	demarrerTemps(j);
	break;
      }
    }
    dechargerSelection(j->g, oldli, oldco); /* Libère aussi graphiquement la sélection */
  }
}
