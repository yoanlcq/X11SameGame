#ifndef CONFDEF_H
#define CONFDEF_H

typedef struct Config_s {
  couleur palette[PAL_TAILLE_MAX];
  char image_bille[PAL_TAILLE_MAX][NOM_TAILLE_MAX];
  unsigned numcouleurs;
  char image_fond_grille[NOM_TAILLE_MAX];
  /**/
  unsigned lignes_grille;
  unsigned colonnes_grille;
  unsigned largeur_case_grille;
  unsigned hauteur_case_grille;
  unsigned separateur_grille;
  unsigned temps_limite;
  /**/
  couleur couleur_surbrillance_grille;
  couleur couleur_fond_grille;
  /**/
  couleur couleur_fond_zone_score;
  couleur couleur_police_zone_score; 
  unsigned taille_police_zone_score; 
  couleur couleur_fond_zone_valeur;  
  couleur couleur_police_zone_valeur;
  unsigned taille_police_zone_valeur; 
  couleur couleur_fond_zone_temps;   
  couleur couleur_police_zone_temps; 
  unsigned taille_police_zone_temps; 
} Config ;


void ecrireScore(unsigned score, char *pseudo, char *map, char *config);
void ajouterScore(FILE *fichier, long position, unsigned score, char *pseudo, char *map);

Config *nouvelleConfiguration(const char *nom_fichier);

int streq(const char *str1, const char *str2);
int strsrch(const char srch[TOKEN_TAILLE_MAX], char ary[][TOKEN_TAILLE_MAX], unsigned dim);
couleur hexToCouleur(const char *chaine);
void lireStr(FILE *fichier, char token[TOKEN_TAILLE_MAX]);
void tokenSuivant(FILE *fichier, char token[TOKEN_TAILLE_MAX]);


#endif /* Guards */
