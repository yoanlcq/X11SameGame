#include "univ.h"
#include "zonedef.h"
#include "menudef.h"

/* L'endroit où c'est mieux d'élargir son éditeur de texte... */
int menuPrincipal(unsigned *hauteurs_police, char *pseudo, char *map, char *config) {
  int valeur_de_retour;
  unsigned i; /* Pour parcourir le pseudo et traquer les caractères non autorisés */

  unsigned char_l, marge_l, fen_l, fen_h;

  Zone *txt_pseudo, *btn_pseudo, *info_pseudo;
  Zone *txt_config, *btn_config, *info_config;
  Zone *txt_map,    *btn_map,    *info_map;
  Zone *btn_quitter, *btn_jouer, *btn_about; 
  
  FILE *fichier_tmp;
  
reafficher:
  InitialiserGraphique();

  /* Ces assignations font du menu de qu'il est tel qu'on le voit. Ce n'est qu'une histoire de proportions à garder et la partie intéressante est plus loin. */
  
  char_l = TailleChaineEcran("0", 1);
  marge_l = 5*char_l;
  fen_l = char_l*NOM_TAILLE_MAX + 2*marge_l;
  fen_h = 15*hauteurs_police[1];

  txt_pseudo  = nouvelleZone(marge_l, hauteurs_police[1]/2, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);
  btn_pseudo  = nouvelleZone(marge_l + char_l*PSEUDO_TAILLE_MAX/2, hauteurs_police[1]*3/2, char_l*PSEUDO_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);
  info_pseudo = nouvelleZone(marge_l, hauteurs_police[1]*5/2, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);

  txt_config  = nouvelleZone(marge_l, hauteurs_police[1]*4, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);
  btn_config  = nouvelleZone(marge_l, hauteurs_police[1]*5, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);
  info_config = nouvelleZone(marge_l, hauteurs_police[1]*6, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);

  txt_map  = nouvelleZone(marge_l, hauteurs_police[1]*15/2, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);
  btn_map  = nouvelleZone(marge_l, hauteurs_police[1]*17/2, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);
  info_map = nouvelleZone(marge_l, hauteurs_police[1]*19/2, char_l*NOM_TAILLE_MAX, hauteurs_police[1], 1, COULEUR_POLICE_TEXTE, COULEUR_FOND_MENU);

  btn_quitter = nouvelleZone(marge_l, hauteurs_police[1]*23/2, char_l*NOM_TAILLE_MAX/2 -3, hauteurs_police[1]*3/2, 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);
  btn_jouer   = nouvelleZone(fen_l/2, hauteurs_police[1]*23/2, char_l*NOM_TAILLE_MAX/2 +3, hauteurs_police[1]*3/2, 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);
  btn_about   = nouvelleZone(marge_l, hauteurs_police[1]*27/2, char_l*NOM_TAILLE_MAX +3, hauteurs_police[1]*3/2, 1, COULEUR_POLICE_BOUTON, COULEUR_FOND_BOUTON);


  /* Enfin la fenêtre */
  CreerFenetre(0,0,fen_l,fen_h);
  ChoisirTitreFenetre("Marble Game Micro-project");
  ChoisirCouleurDessin(COULEUR_FOND_MENU);
  RemplirRectangle(0,0,fen_l,fen_h);

  /* Faisons un affichage initial avant d'entrer dans la boucle. */
  /* Noter qu'on essaye d'ouvrir les fichiers de chemin 'config' et 'map' juste pour voir s'ils existent. */

  afficherZone(txt_pseudo, "Name :");
  afficherZone(btn_pseudo, pseudo);
  afficherZone(info_pseudo, "");

  afficherZone(txt_config, "Config :");
  afficherZone(btn_config, config);
  fichier_tmp = fopen(config, "r");
  if(fichier_tmp == NULL)
    afficherZone(info_config, "No config ? Default one chosen.");
  else {
    fclose(fichier_tmp);
    afficherZone(info_config, "");
  }

  afficherZone(txt_map, "Map :");
  afficherZone(btn_map, map);
  fichier_tmp = fopen(map, "r");
  if(fichier_tmp == NULL)
    afficherZone(info_map, "No map ? Random grid.");
  else {
    fclose(fichier_tmp);
    afficherZone(info_map, "");
  }
  
  afficherZone(btn_quitter, "Exit");
  afficherZone(btn_jouer, "Play !");
  afficherZone(btn_about, "About...");
  

  /* La boucle du menu. La valeur de retour de cette fonction en est la condition de sortie. */
  /* Sur appui du bouton 'Quitter', valeur_de_retour prend 0, et 1 sur appui du bouton 'Jouer'. */

  valeur_de_retour = -1;
  while(valeur_de_retour == -1) {
    SourisPosition();

    /* SURVOL DE BOUTONS (Eclairer les boutons) */

    if(sourisDansRectangle(btn_pseudo->rec) && btn_pseudo->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_pseudo->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_pseudo, pseudo);
    }
    else if(sourisDansRectangle(btn_config->rec) && btn_config->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_config->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_config, config);
    }
    else if(sourisDansRectangle(btn_map->rec) && btn_map->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_map->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_map, map);
    }
    else if(sourisDansRectangle(btn_quitter->rec) && btn_quitter->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_quitter->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_quitter, "Exit");
    }
    else if(sourisDansRectangle(btn_jouer->rec) && btn_jouer->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_jouer->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_jouer, "Play !");
    }
	else if(sourisDansRectangle(btn_about->rec) && btn_about->fond != COULEUR_SURBRILLANCE_BOUTON) {
      btn_about->fond = COULEUR_SURBRILLANCE_BOUTON;
      afficherZone(btn_about, "About...");
    }

    /* SORTIE DE SURVOL DE BOUTON (les déséclairer) */

    if(!sourisDansRectangle(btn_pseudo->rec) && btn_pseudo->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_pseudo->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_pseudo, pseudo);
    }
    if(!sourisDansRectangle(btn_config->rec) && btn_config->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_config->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_config, config);
    }
    if(!sourisDansRectangle(btn_map->rec) && btn_map->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_map->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_map, map);
    }
    if(!sourisDansRectangle(btn_quitter->rec) && btn_quitter->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_quitter->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_quitter, "Exit");
    }
    if(!sourisDansRectangle(btn_jouer->rec) && btn_jouer->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_jouer->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_jouer, "Play !");
    }
	if(!sourisDansRectangle(btn_about->rec) && btn_about->fond == COULEUR_SURBRILLANCE_BOUTON) {
      btn_about->fond = COULEUR_FOND_BOUTON;
      afficherZone(btn_about, "About...");
    }

    /* CLICS SUR LES BOUTONS */

    if(SourisCliquee()) {
		/* Exemple : la zone de texte du pseudo. */
		if(sourisDansRectangle(btn_pseudo->rec)) {
			afficherZone(info_pseudo, "Press [ENTER] once done.");
			/* On entre le texte. */
			entreeTexteZone(btn_pseudo, pseudo, PSEUDO_TAILLE_MAX);
			afficherZone(info_pseudo, "");
			/* Si pseudo vide... on donne le défaut*/
			if(pseudo[0] == '\0')
				strncpy(pseudo, PSEUDO_DEFAUT, PSEUDO_TAILLE_MAX);
			/* Si pseudo à espaces... */
			for(i=0 ; i<PSEUDO_TAILLE_MAX ; i++)
				if(pseudo[i] == ' ') {
					afficherZone(info_pseudo, "Spaces aren't allowed !");
					/* même punition, pseudo par défaut. */
					strncpy(pseudo, PSEUDO_DEFAUT, PSEUDO_TAILLE_MAX);
					break;
				}
			afficherZone(btn_pseudo, pseudo);
		}
		else if(sourisDansRectangle(btn_config->rec)) {
			afficherZone(info_config, "Press [ENTER] once done.");
			entreeTexteZone(btn_config, config, NOM_TAILLE_MAX);
			afficherZone(btn_config, config);
			fichier_tmp = fopen(config, "r");
			if(fichier_tmp == NULL)
				afficherZone(info_config, "No config ? Default one chosen.");
			else {
				fclose(fichier_tmp);
				afficherZone(info_config, "");
			}
		}
		else if(sourisDansRectangle(btn_map->rec)) {
			afficherZone(info_map, "Press [ENTER] once done.");
			entreeTexteZone(btn_map, map, NOM_TAILLE_MAX);
			afficherZone(btn_map, map);
			fichier_tmp = fopen(map, "r");
			if(fichier_tmp == NULL)
				afficherZone(info_map, "No map ? Random grid.");
			else {
				fclose(fichier_tmp);
				afficherZone(info_map, "");
			}
		}
		
		else if(sourisDansRectangle(btn_jouer->rec))
			valeur_de_retour = 1;
		else if(sourisDansRectangle(btn_quitter->rec))
			valeur_de_retour = 0;
		else if(sourisDansRectangle(btn_about->rec)) {
			FermerGraphique();
			credits(hauteurs_police);
			goto reafficher;
		}
		
    } /* FIN SOURIS CLIQUEE */
  }
  
  effacerZone(txt_pseudo);
  effacerZone(btn_pseudo);
  effacerZone(info_pseudo);
  effacerZone(txt_map);
  effacerZone(btn_map);
  effacerZone(info_map);
  effacerZone(txt_config);
  effacerZone(btn_config);
  effacerZone(info_config);
  effacerZone(btn_jouer);
  effacerZone(btn_quitter);
  effacerZone(btn_about);

  FermerGraphique();

  return valeur_de_retour;
}

void credits(unsigned *hauteurs_police) {
	Zone* lignes[5];
	char *str[]={"This game has been written in C by Yoan Lecoq and Adrien Bervoet",
				  "as a first-year micro-project in an IT course.",
				  "This is the original one, as given to the teachers, with some minor improvements and fixes.",
                  /* Ouais alors non */""
				  /*"A better, cross-platform version using SFML is currently being worked on."*/,
				  "Thanks to teacher Denis Monnerat for his simple X11 layer."};
	unsigned i, l, h, margin = 10;
	for(l=0, i=0 ; i<5 ; i++) {
		if(strlen(str[i]) > l)
			l = strlen(str[i]);
	}
	h = i*hauteurs_police[1] + (i+2)*margin;
	l = l*TailleChaineEcran("0", 1) + 2*margin;
	InitialiserGraphique();
	CreerFenetre(0,0,l,h);
	ChoisirTitreFenetre("Marble Game Micro-project");
	for(i=0 ; i<5 ; i++) {
		lignes[i] = nouvelleZone(0, i*h/5, l, h/5 + margin, 1, 0, 0xDDDDDD);
		afficherZone(lignes[i], str[i]);
	}
	while(!(ToucheEnAttente() || SourisCliquee()));
	FermerGraphique();
}
