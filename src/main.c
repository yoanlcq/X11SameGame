#include "univ.h"
#include "rectangledef.h"
#include "zonedef.h"
#include "grilledef.h"
#include "jeudef.h"
#include "chronodef.h"
#include "confdef.h"
#include "menudef.h"

int main(int argc, char *argv[]) {
  Jeu *j;
  int tmp; /* Essentiellement pour le retour de fscanf() */
  unsigned score;
  FILE *fichier_tmp; /* Un pointeur de fichier qui peut servir à plusieurs. */
  /* Dans ces sources, JAMAIS deux fichiers ne sont ouverts simultanément. */

  char config[NOM_TAILLE_MAX];
  char map[NOM_TAILLE_MAX];
  char pseudo[PSEUDO_TAILLE_MAX];

  unsigned hauteurs_police[NB_TAILLES_POLICE];

  /* OBTENTION DES HAUTEURS DE POLICE */
  /* Le menu sera forgé en fonction de ces données. */
  InitialiserGraphique();
  CreerFenetre(0,0,1,1);
  for(tmp=0 ; tmp<NB_TAILLES_POLICE ; tmp++)
    hauteurs_police[tmp] = TailleSupPolice(tmp) + TailleInfPolice(tmp) + (tmp+1);
  FermerGraphique();

  /*----------------------------------*/


  /* ACCES AU FICHIER .billerc */
  /* S'il existe... On lit */
  fichier_tmp = fopen(".billerc", "r");
  if(fichier_tmp != NULL) {
    tmp = fscanf(fichier_tmp, "pseudo = %s\n", pseudo);
    if(tmp <= 0 || tmp == EOF)
      strncpy(pseudo, PSEUDO_DEFAUT, PSEUDO_TAILLE_MAX);

    tmp = fscanf(fichier_tmp, "config = %s\n", config);
    if(tmp <= 0 || tmp == EOF)
      strncpy(config, FICHIER_CONFIG_INITIAL, NOM_TAILLE_MAX);

    tmp = fscanf(fichier_tmp, "map = %s\n", map);
    if(tmp <= 0 || tmp == EOF)
      map[0] = '\0';
    fclose(fichier_tmp);
    /* On l'a fermé. */
  }
  /* S'il n'existe pas... On ne le crée pas tout de suite mais on assigne les données par défaut*/
  else {
    strncpy(pseudo, PSEUDO_DEFAUT, PSEUDO_TAILLE_MAX);
    strncpy(config, FICHIER_CONFIG_INITIAL, NOM_TAILLE_MAX);
    map[0] = '\0';
  }
  /* ------------------------- */

  /* BOUCLE MENU-JEU-MENU-JEU... */
  /* menuprincipal() retourne 0 sur appui du bouton Quitter. */
  /* Cette fonction InitialiseGraphique(), ouvre une fenêtre et la ferme à la fin. */
  while(menuPrincipal(hauteurs_police, pseudo, map, config)) { 
    
    /* Ecriture dans .billerc. */
    /* Cette partie de code se retrouve aussi après cette boucle. On la place ici aussi dans le cas où l'utilisateur ferme sauvagement la fenetre en plein jeu. */
    fichier_tmp = fopen(".billerc", "w");
    fprintf(fichier_tmp, "pseudo = %s\n", pseudo);
    fprintf(fichier_tmp, "config = %s\n", config);
    fprintf(fichier_tmp, "map = %s\n", map);
    fclose(fichier_tmp);

    /* Lecture de la configuration... */

    j = jeuDepuisConfiguration(config, hauteurs_police);
    if(j == NULL) {
		fprintf(stderr, "Impossible de continuer. Assurez-vous d'exécuter l'application dans son dossier.\n(Ce problème sera fixé dans une prochaine version).\n");
		exit(EXIT_FAILURE);
    }
    fichier_tmp = fopen(config, "r");
    if(fichier_tmp == NULL)
      strcpy(config, FICHIER_CONFIG_INITIAL);
    else fclose(fichier_tmp);

    /* Lecture de la map... */

    srand((unsigned) time(NULL));  
    fichier_tmp = fopen(map, "r");
    if(fichier_tmp == NULL) {
      /* Grille aléatoire */
      remplirGrilleAvecHasard(j->g);
      strcpy(map, "");
    }
    else {
      fclose(fichier_tmp);
      /* Grille à remplir avec la map. */
      remplirGrilleAvecFichier(j->g, map);
    }

    /* A l'attaque !*/

    InitialiserGraphique();
    /* Une structure Jeu contient la grille de billes et les zones de notification de score, etc. 
    ** On accède à son rectangle (qui englobe les autres) pour déterminer la taille de la fenetre.*/
    CreerFenetre(0, 0, j->rec->l, j->rec->h);
	ChoisirTitreFenetre("Marble Game Micro-project");
    /* Initialisation du fond et des sprites de la Grille. */
    initialiserDonneesGraphiquesGrille(j->g);
    
    /* Entrée dans la boucle de jeu avec jouer(); elle retourne le score obtenu dans tous les cas. */
    
    score = jouer(j); 
    effacerJeu(j); /* On libère le jeu. les destructeurs des sous-objets seront égelement appelés. */
    FermerGraphique();

    /* Il reste à graver l'exploit incroyable du joueur dans le fichier de scores. */
    ecrireScore(score, pseudo, map, config); 
  }
  
  fichier_tmp = fopen(".billerc", "w");
  fprintf(fichier_tmp, "pseudo = %s\n", pseudo);
  fprintf(fichier_tmp, "config = %s\n", config);
  fprintf(fichier_tmp, "map = %s\n", map);
  fclose(fichier_tmp);
  
  exit(EXIT_SUCCESS);
}
