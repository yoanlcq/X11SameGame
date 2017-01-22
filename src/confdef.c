#include "univ.h"
#include "confdef.h"
#include "rectangledef.h"
#include "grilledef.h"
#include "zonedef.h"
#include "jeudef.h"


int streq(const char *str1, const char *str2) {
  int i;
  for(i=0 ; str1[i]!='\0' || str2[i]!='\0'; i++)
    if(str1[i]!=str2[i] || (str1[i]=='\0' && str2[i]!='\0') || (str1[i]!='\0' && str2[i]=='\0'))
      return 0;
  return 1;
}

int strsrch(const char srch[TOKEN_TAILLE_MAX], char ary[][TOKEN_TAILLE_MAX], unsigned dim) { 
  int chari, stri, max=dim, c;

  int *stck;
  if((stck = (int*) malloc(dim*sizeof(int))) == NULL) {
    fprintf(stderr,"Attention : strsrch() : malloc() a échoué.\n");
    return -1;
  }
  
  for(c=0 ; c<dim ; c++)
    stck[c]=c;

  for(chari=0 ; chari<TOKEN_TAILLE_MAX ; chari++) {
    for(c=stri=0 ; stri<max ; stri++) { 
      /* 2ème tourne */
      if(ary[stck[stri]][chari] == srch[chari]) {
	stck[c]=stck[stri];
	c++;
      }     
    }
    if(c==1) {
      max=stck[0];
      free(stck);
      return max; 
    }
    else if(c==0) {
      free(stck);
      return -1;
    }
    max=c;
  }
  return -1;
}

couleur hexToCouleur(const char *chaine) {
  couleur c = 0;
  unsigned i, deb = 0, fin;

  /* Manger le 0x */
  if(chaine[0] == '0' && chaine[1] == 'x')
    deb = 2;

  for(i=0 ; chaine[i]!='\0' ; i++);
  i--;
  fin=i;
  for( ; i>=deb ; i--) {
    if(chaine[i] >= '0' && chaine[i] <= '9')
      c = c + ((chaine[i]-'0')<<((fin-i)*4));
    else if(chaine[i] >= 'A' && chaine[i] <= 'F')
      c = c + ((chaine[i]-'A'+10)<<((fin-i)*4));
    else if(chaine[i] >= 'a' && chaine[i] <= 'f')
      c = c + ((chaine[i]-'a'+10)<<((fin-i)*4));
  }
  return c;
}

void tokenSuivant(FILE *fichier, char token[TOKEN_TAILLE_MAX]) {
  char tmp;
  unsigned i;
  tmp = fgetc(fichier);
  i=0;
  do {
    if(tmp == '#') {
      while(tmp != '\n' && !feof(fichier))
	tmp = fgetc(fichier);
      if(i==0)
	tmp = fgetc(fichier);
      else
	break;
    }
    if(tmp == '=' || tmp == '"' || tmp == ',' || tmp == '{' || tmp == '}') {
      if(i != 0) {
	ungetc(tmp, fichier); /* Oops, il est en trop. On return le token courant. On lira plus tard ce caractère. */
	break;
      }
      token[0]=tmp; token[1]='\0';
      return;
    }
    if(tmp >= 33 && tmp <= 126) {
      token[i]=tmp;
      i++;
    }

    tmp = fgetc(fichier);

  } while(!feof(fichier) && i<TOKEN_TAILLE_MAX-1 && !(tmp == '\n' && i>0));

  token[i] = '\0';
}

void lireStr(FILE *fichier, char token[TOKEN_TAILLE_MAX]) {
  unsigned i;
  char tmp;
  
  if(token[0] != '"')
    while(token[0] != '"')
      tokenSuivant(fichier, token);
  
  i=0;
  do {
    tmp = fgetc(fichier);
    if(tmp >= 33 && tmp <= 126 && tmp != '"') {
      token[i]=tmp;
      i++;
    } 
  } while(i<TOKEN_TAILLE_MAX-1 && tmp != '"');
  token[i] = '\0';
}

Config *nouvelleConfiguration(const char *nom_fichier) {
  FILE *fichier;
  Config *cfg = (Config*) malloc(sizeof(Config));
  int tmp;
  unsigned i;
  char token[TOKEN_TAILLE_MAX];
  char dic[25][TOKEN_TAILLE_MAX]={
    "palette",
    "image_bille_1",
    "image_bille_2",
    "image_bille_3",
    "image_bille_4",
    "image_bille_5",
    "image_bille_6",
    "image_fond_grille",
    /**/
    "lignes_grille",
    "colonnes_grille",
    "largeur_case_grille",
    "hauteur_case_grille",
    "separateur_grille",
    "temps_limite",
    /**/
    "couleur_surbrillance_grille",
    "couleur_fond_grille",
    /**/
    "couleur_fond_zone_score",
    "couleur_police_zone_score", 
    "taille_police_zone_score", 
    "couleur_fond_zone_valeur",  
    "couleur_police_zone_valeur",
    "taille_police_zone_valeur", 
    "couleur_fond_zone_temps",   
    "couleur_police_zone_temps", 
    "taille_police_zone_temps"
  };

  fichier = fopen(nom_fichier, "r");
  if(fichier == NULL) {
    fprintf(stderr, "Attention : Fichier de configuration \"%s\" introuvable !\n", nom_fichier);
    fprintf(stderr, "Ouverture de la configuration par defaut.\n");
    fichier = fopen(FICHIER_CONFIG_INITIAL, "r");
    if(fichier == NULL) {
      fprintf(stderr, "Configuration par defaut (\"%s\") introuvable !\n", FICHIER_CONFIG_INITIAL);
      return NULL;
    }
  }
  
  while(!feof(fichier)) {
    tokenSuivant(fichier, token);
    tmp = strsrch(token, dic, 25);
    
    if(tmp != -1) {
      /* On va jusqu'au '=' */
      do {
	tokenSuivant(fichier, token);
      } while(token[0] != '=' && !feof(fichier));
    }
    
    /* le token de l'opérande assignée (épargne de la répétition dans le switch) */
    tokenSuivant(fichier, token);
    /* On traite l'asignement */
    switch(tmp) {
    case 0 : 
      while(token[0] != '{')
	tokenSuivant(fichier, token);
      
      i=0;
      while(token[0] != '}') {
	if(i >= 6) {
	   while(token[0] != '}')
	     tokenSuivant(fichier, token);
	}
	else {
	  tokenSuivant(fichier, token);
	  if(token[0] != '}' && token[0] != ',') {
	    cfg->palette[i] = hexToCouleur(token);
	    i++;
	    while(token[0] != '}' && token[0] != ',')
	      tokenSuivant(fichier, token);
	  }
	}
      }
      cfg->numcouleurs = i;
      break;
    case 1 :  lireStr(fichier, token); strncpy(cfg->image_bille[0], token, NOM_TAILLE_MAX); break;
    case 2 :  lireStr(fichier, token); strncpy(cfg->image_bille[1], token, NOM_TAILLE_MAX); break;
    case 3 :  lireStr(fichier, token); strncpy(cfg->image_bille[2], token, NOM_TAILLE_MAX); break;
    case 4 :  lireStr(fichier, token); strncpy(cfg->image_bille[3], token, NOM_TAILLE_MAX); break;
    case 5 :  lireStr(fichier, token); strncpy(cfg->image_bille[4], token, NOM_TAILLE_MAX); break;
    case 6 :  lireStr(fichier, token); strncpy(cfg->image_bille[5], token, NOM_TAILLE_MAX); break;
    case 7 :  lireStr(fichier, token); strncpy(cfg->image_fond_grille, token, NOM_TAILLE_MAX); break;
    case 8 :  cfg->lignes_grille = (unsigned) atoi(token); break;
    case 9 :  cfg->colonnes_grille = (unsigned) atoi(token); break;
    case 10 : cfg->largeur_case_grille = (unsigned) atoi(token); break;
    case 11 : cfg->hauteur_case_grille = (unsigned) atoi(token); break;
    case 12 : cfg->separateur_grille = (unsigned) atoi(token); break;
    case 13 : cfg->temps_limite = (unsigned) atoi(token); break;
    case 14 : cfg->couleur_surbrillance_grille = hexToCouleur(token); break;
    case 15 : cfg->couleur_fond_grille = hexToCouleur(token); break;
    case 16 : cfg->couleur_fond_zone_score = hexToCouleur(token); break;
    case 17 : cfg->couleur_police_zone_score = hexToCouleur(token); break; 
    case 18 : cfg->taille_police_zone_score = (unsigned) atoi(token); break; 
    case 19 : cfg->couleur_fond_zone_valeur = hexToCouleur(token); break;  
    case 20 : cfg->couleur_police_zone_valeur = hexToCouleur(token); break;
    case 21 : cfg->taille_police_zone_valeur = (unsigned) atoi(token); break; 
    case 22 : cfg->couleur_fond_zone_temps = hexToCouleur(token); break;   
    case 23 : cfg->couleur_police_zone_temps = hexToCouleur(token); break;
    case 24 : cfg->taille_police_zone_temps = (unsigned) atoi(token); break;
    }
  }
  fclose(fichier);
  return cfg;
}

void ecrireScore(unsigned score, char *pseudo, char *map, char *config) {
  FILE *fichier;
  char *nom_fichier_score;

  long position;
  int retour_fscanf;
  unsigned score_tmp;

  nom_fichier_score = (char*) malloc((strlen(config)+strlen(".scores")+1)*sizeof(char));
  nom_fichier_score[0] = '\0';
  strcat(nom_fichier_score, config);
  strcat(nom_fichier_score, ".scores");
  
  fichier = fopen(nom_fichier_score, "r+");
  if(fichier == NULL){
    fichier = fopen(nom_fichier_score, "w");
    fprintf(fichier, "%.10u <-- %s (%s)\n", score, pseudo, map);

    fclose(fichier);
    free(nom_fichier_score);
    return;
  }

  /* On a ouvert le fichier. */
  do {
    position = ftell(fichier);
    retour_fscanf = fscanf(fichier, "%u <-- %*s (%*s)", &score_tmp);
    fgetc(fichier); /* Avaler le \n pour la prochaine sauvegarde de position */
    if(retour_fscanf <= 0 || retour_fscanf == EOF){
      fprintf(fichier, "%.10u <-- %s (%s)\n", score, pseudo, map);
      fclose(fichier);
      free(nom_fichier_score);
      return;
    } else {
      if(score >= score_tmp){
	ajouterScore(fichier, position, score, pseudo, map);
	fclose(fichier);
	free(nom_fichier_score);
	return;
      }
    }
  } while(!feof(fichier) && retour_fscanf > 0 && retour_fscanf != EOF);

  fclose(fichier);
  free(nom_fichier_score);
}

void ajouterScore(FILE *fichier, long position, unsigned score, char *pseudo, char *map) {
  char massif[FICHIER_SCORE_TAILLE_MAX];
  unsigned long i;
  int taille;
  fseek(fichier, position, SEEK_SET);
  /* Soulever */
  for(i=0 ; i<FICHIER_SCORE_TAILLE_MAX && !feof(fichier) ; i++)
    massif[i] = fgetc(fichier);
  massif[i] = '\0';
  
  fseek(fichier, position, SEEK_SET);

  taille = fprintf(fichier, "%.10u <-- %s (%s)\n", score, pseudo, map);
  for(i=0 ; massif[i] != '\0'; i++) {
    fseek(fichier, position+taille+i, SEEK_SET); /* Ne pas toucher ! */
    fputc(massif[i], fichier);
  }
  ungetc('\n', fichier);
}
