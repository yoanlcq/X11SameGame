#include "univ.h"
#include "rectangledef.h"
#include "grilledef.h"

/* Constructeur de Grille. Prend beaucoup de paramètres, mais après, tout ce dont on a besoin est accessible depuis la Grille construite. On peut même, du coup, gérer plusieurs Grilles ! */
/* Attention, le constructeur initialise uniformément les billes de la Grille avec une couleur noire. Il faut appeler d'autres fonctions pour générer les billes selon un fichier ou aléatoirement. */
/* On ne se permet PAS de remplir aléatoirement la Grille par défaut. C'est l'utilisateur qui choisit. */
Grille* nouvelleGrille(unsigned p_x, unsigned p_y,
		       unsigned p_numli, unsigned p_numco,
		       unsigned p_case_l, unsigned p_case_h,
		       unsigned p_separateur,
		       couleur p_fond, couleur p_surbrillance,
		       const couleur p_palette[PAL_TAILLE_MAX], 
		       char p_bille_img[PAL_TAILLE_MAX][NOM_TAILLE_MAX],
		       unsigned p_numcouleurs,
		       const char p_fond_img[NOM_TAILLE_MAX]
		       )
{
  
  unsigned i;

  /* Initialisation du pointeur de Grille */

  Grille* g = (Grille*) malloc(sizeof(Grille));
  if(g == NULL) {
    fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de Grille a échoué.\n");
    return NULL; /* un exit(EXIT_FAILURE) n'est pas acceptable. */
  }

  /* Initialisation du rectangle. */
  g->rec = (Rectangle*) malloc(sizeof(Rectangle));
  if(g->rec == NULL) {
    fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de Rectangle a échoué.\n");
    annulerGrille(g);
    return NULL;
  }

  /* Initialisation du tableau dynamique de sélection des billes. */
  g->selection = (int**) malloc(p_numli*p_numco*sizeof(int*));
  if(g->selection == NULL) {
    fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de mémoire de sélection a échoué.\n");
    annulerGrille(g);
    return NULL;
  }
  for(i=0 ; i<(p_numli*p_numco) ; i++) {
    g->selection[i] = (int*) malloc(2*sizeof(int));
    if(g->selection[i] == NULL) {
      fprintf(stderr, "Erreur : nouvellegGrille() : malloc() de mémoire de sélection a échoué.\n");
      annulerGrille(g);
      return NULL;
    }
    g->selection[i][0] = g->selection[i][1] = -1; 
  }
  g->nbselectionnes = 0;

  /* Initialisation et liaison cyclique des Colonnes. Un schéma sur papier ne prend que quelques secondes. */

  g->prem = (Colonne*) malloc(sizeof(Colonne));
  if(g->prem == NULL) {
    fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de Colonnes a échoué.\n");
    annulerGrille(g);
    return NULL;
  }
  g->courante = g->prem;
  for(i=1 ; i < p_numco ; i++) {
    g->courante->adroite = (Colonne*) malloc(sizeof(Colonne));
    if(g->courante->adroite == NULL) {
      fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de Colonnes a échoué.\n");
      annulerGrille(g);
      return NULL;
    }
    g->courante->adroite->agauche = g->courante;
    g->courante = g->courante->adroite;
  }
  g->courante->adroite = g->prem;
  g->prem->agauche = g->courante;
  g->courante = g->prem;

  /* Remplissage par des billes noires-sans-image (défaut) */
  for(i=0 ; i < p_numco ; i++) {
    /* Avec calloc(), les bits sont mis à zéro, donc les billes seront bien de couleur noire. */
    g->courante->bille = (couleur*) calloc(p_numli, sizeof(couleur));
    if(g->courante->bille == NULL) {
      fprintf(stderr, "Erreur : nouvelleGrille() : calloc() de billes a échoué.\n");
      annulerGrille(g);
      return NULL;
    }
    g->courante = g->courante->adroite;
    /* reviendra toute seule à la Colonne de gauche de la Grille à la fin. */
  }


  /* Le coin des assignations */
  g->numli = p_numli;
  g->numco = p_numco;
  g->case_l = p_case_l;
  g->case_h = p_case_h;
  g->separateur = p_separateur;
  g->rec->x = p_x;
  g->rec->y = p_y;
  g->rec->l = (g->numco * g->case_l) + ((g->numco+1) * g->separateur);
  g->rec->h = (g->numli * g->case_h) + ((g->numli+1) * g->separateur);
  g->fond = p_fond;
  g->surbrillance = p_surbrillance;
  g->nbselectionnes = 0;

  g->numcouleurs = p_numcouleurs;
  for(i=0 ; i < p_numcouleurs ; i++) {
    g->palette[i] = p_palette[i];
    g->bille_sprite[i] = -1;
    strncpy(g->bille_img[i], p_bille_img[i], NOM_TAILLE_MAX);
  }
  strncpy(g->fond_img, p_fond_img, NOM_TAILLE_MAX);
  return g;
}

/* Le destructeur d'une Grille correctement construite. */
void effacerGrille(Grille *g) {
  unsigned i;
  free(g->rec);
  /* Encore une fois, faire un petit schéma aide bien. */
  for(i=1 ; i < g->numco ; i++) {
    g->prem = g->prem->adroite;
    free(g->prem->agauche->bille);
    free(g->prem->agauche);
  }
  free(g->prem->bille);
  free(g->prem);
  for(i=0 ; i<(g->numli * g->numco) ; i++)
    free(g->selection[i]);
  free(g->selection);
  for(i=0 ; i<PAL_TAILLE_MAX ; i++)
    if(g->bille_sprite[i] != -1)
      LibererSprite(g->bille_sprite[i]);
  if(g->fond_img_sprite != -1)
    LibererSprite(g->fond_img_sprite);
  free(g);
}

/* Un destructeur spécialement consacré au cas de malloc() ayant échoué lors de la construction de la Grille. */
/* Tous ces 'if' servent à éviter les segfaults si la Grille n'est pas entièrement construite (ce qui n'arrivera pratiquement jamais, mais on doit être robustes) : On risque de libérer des pointeurs non initialisés. */
void annulerGrille(Grille *g) {
  unsigned i;
  if(g->rec != NULL)
    free(g->rec);
  /* Encore une fois, faire un petit schéma aide bien. */
  for(i=1 ; i < g->numco ; i++) {
    if(g->prem->adroite != NULL) {
      g->prem = g->prem->adroite;
      if(g->prem->agauche !=NULL) { /* < TODO : ce 'if' est peut-être inutile. */
	if(g->prem->agauche->bille != NULL)
	  free(g->prem->agauche->bille);
	free(g->prem->agauche);
      }
    }
  }
  if(g->selection != NULL) {
    for(i=0 ; i<(g->numli * g->numco) ; i++) {
      if(g->selection[i] != NULL)
	free(g->selection[i]);
      free(g->selection);
    }
  }
  if(g->prem->bille != NULL)
    free(g->prem->bille);
  if(g->prem != NULL)
    free(g->prem);
  free(g);
}

/* Remplit les billes de la Grille à partir d'un fichier. En cas de fin de fichier ou de lignes trop courtes, les billes manquantes seront noires-sans-image. */
/* Cette fonction suppose que l'on utilise les lettres RJVCBM (Aka Rouge Jaune Vert Cyan Bleu Magenta) */
void remplirGrilleAvecFichier(Grille *g, const char *nom_fichier) {
  unsigned li,co;
  FILE *fichier = fopen(nom_fichier, "r");
  char **ch;

  if(fichier == NULL) {
    fprintf(stderr, "Attention : Echec d'ouverture du fichier \"%s\".\n", nom_fichier);
    return;
  }
  /* Emploi de calloc pour que l'utilisateur ne voie pas de billes si son fichier est incomplet. */
  ch = (char**) calloc(g->numli, sizeof(char*));
  for(li=0 ; li < g->numli ; li++)
    ch[li] = (char*) calloc(g->numco, sizeof(char));

  /* Accès disque lent, alors on lit le fichier d'une traite ! (et on stocke tout) */
  for(li=0 ; li < g->numli ; li++) {
    for(co=0 ; co < g->numco ; co++) {
      ch[li][co] = fgetc(fichier);
      if(ch[li][co] == '\n')
	break;
      if(feof(fichier)) {
	fprintf(stderr, "Attention : Le fichier \"%s\" suppose des proportions inferieures.\n", nom_fichier);
	break;
      }
    }
    if(feof(fichier))
      break;
    if(ch[li][co] != '\n')
      while(fgetc(fichier) != '\n'); /* Poursuivons le \n */
  }
  fclose(fichier);

  /* Remplissage optimisé pour nos Colonnes chaînées : On remplit chaque Colonne d'une traite avant de passer à la suivante. */
  for(g->courante=g->prem, co=0 ; co< g->numco ; co++, g->courante=g->courante->adroite) {
    for(li=0 ; li < g->numli ; li++) {
      /* Désolé Luc, j'ai rejoint le côté case-break de la force */ 
      switch(ch[li][co]) {
      case 'R': g->courante->bille[li] = 0xFF0000; break;
      case 'J': g->courante->bille[li] = 0xFFFF00; break;
      case 'V': g->courante->bille[li] = 0x00FF00; break;
      case 'C': g->courante->bille[li] = 0x00FFFF; break;
      case 'B': g->courante->bille[li] = 0x0000FF; break;
      case 'M': g->courante->bille[li] = 0xFF00FF; break;
      case '1': g->courante->bille[li] = g->palette[0]; break;
      case '2': g->courante->bille[li] = g->palette[1]; break;
      case '3': g->courante->bille[li] = g->palette[2]; break;
      case '4': g->courante->bille[li] = g->palette[3]; break;
      case '5': g->courante->bille[li] = g->palette[4]; break;
      case '6': g->courante->bille[li] = g->palette[5]; break;
      default : g->courante->bille[li] = 0x000000; break;
      }
    }
  }
}

/* Précondition : srand((unsigned) time(NULL)) a été invoqué. */
void remplirGrilleAvecHasard(Grille *g) {
  unsigned co, li;
  for(g->courante=g->prem, co=0 ; co< g->numco ; co++, g->courante=g->courante->adroite)
    for(li=0 ; li < g->numli ; li++)
      g->courante->bille[li] = g->palette[rand()%g->numcouleurs];
}

void initialiserDonneesGraphiquesGrille(Grille *g) {
  unsigned i;
  /* Ecrire le fond de la grille*/
  ChoisirEcran(ECRAN_FOND_GRILLE);
  ChoisirCouleurDessin(g->fond);
  remplirRectangleStruct(g->rec); 
  /* Afficher par-dessus ça l'image de fond, si elle existe. */
  if(g->fond_img[0] != '\0') {
    g->fond_img_sprite = ChargerSprite(g->fond_img);
    if(g->fond_img_sprite == -1) {
      fprintf(stderr, "Attention : Image \"%s\" introuvable.\n", g->fond_img);
    }
    else
      AfficherSprite(g->fond_img_sprite, g->rec->x, g->rec->y);
  }

  for(i=0 ; i<g->numcouleurs ; i++)
    if(g->bille_img[i][0] != '\0') {
      g->bille_sprite[i] = ChargerSprite(g->bille_img[i]);
      if(g->bille_sprite[i] == -1)
	fprintf(stderr, "Attention : Image \"%s\" introuvable.\n", g->bille_img[i]);
    }
  ChoisirEcran(0);
}

void afficherGrille(Grille *g) {
  unsigned co, li;
  CopierZone(ECRAN_FOND_GRILLE, 0, 
	     g->rec->x, g->rec->y,g->rec->l,g->rec->h,
	     g->rec->x, g->rec->y);
  for(g->courante=g->prem, co=0 ; co< g->numco ; co++, g->courante=g->courante->adroite)
    for(li=0 ; li < g->numli ; li++)
      afficherBille(g, g->courante, li, co);
}

/* Fonction artisanale. */
couleur luminosite(couleur c, short valeur) {
  unsigned char r,v,b;
  r = c>>16; 
  v = (c & ((1<<16)-1))>>8;
  b = c & ((1<<8)-1);

  r = valeur<0 ? (r>valeur*-1 ? r+valeur : 0) : (r+valeur<255 ? r+valeur : 255);
  v = valeur<0 ? (v>valeur*-1 ? v+valeur : 0) : (v+valeur<255 ? v+valeur : 255);
  b = valeur<0 ? (b>valeur*-1 ? b+valeur : 0) : (b+valeur<255 ? b+valeur : 255);
  return ((r<<16) + (v<<8) + b);
}

void afficherBille(Grille *g, Colonne *c, unsigned li, unsigned co) {
  unsigned i;

  for(i=0 ; i<g->numcouleurs ; i++)
    if(c->bille[li] == g->palette[i] && g->bille_sprite[i] != -1) {
      AfficherSprite(g->bille_sprite[i],
		     g->rec->x + (co * (g->case_l + g->separateur)),
		     g->rec->y + (li * (g->case_h + g->separateur)));
      return;
    }

  if(c->bille[li] == 0) {
    /* On écrase avec la bonne portion de fond */
    CopierZone(ECRAN_FOND_GRILLE, 0, 
	       g->rec->x + (co * (g->case_l + g->separateur)),
	       g->rec->y + (li * (g->case_h + g->separateur)),
	       g->case_l + 2*g->separateur,
	       g->case_h + 2*g->separateur,
	       g->rec->x + (co * (g->case_l + g->separateur)),
	       g->rec->y + (li * (g->case_h + g->separateur)));
    return;
  }
  /* Des billes artisanales ! */
  ChoisirCouleurDessin(c->bille[li]);
  RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	     g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	     g->case_l, g->case_h, 0, 360);
  /* Si ca vaut vraiment la peine... */
  if(g->case_l > 10 && g->case_h > 10) {
    ChoisirCouleurDessin(luminosite(c->bille[li], -100)); 
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*31/32, g->case_h*31/32, 0, 360);
    ChoisirCouleurDessin(luminosite(c->bille[li], -40)); 
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*18/20, g->case_h*18/20, 0, 360);
    ChoisirCouleurDessin(c->bille[li]);
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*16/20, g->case_h*16/20, 0, 360);
    ChoisirCouleurDessin(luminosite(c->bille[li], 80));
    RemplirArc(g->rec->x + g->case_l/10 + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + g->case_h/10 + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*4/9, g->case_h*4/9, 0, 360);
    ChoisirCouleurDessin(luminosite(c->bille[li], 200));
    RemplirArc(g->rec->x + g->case_l/5 + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + g->case_h/5 + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*1/7, g->case_h*1/7, 0, 360);
  }
}

/* Retourne 0 si non, ou nb de billes restantes + 1 si oui. */
int GrilleTerminee(Grille *g) {
  unsigned li, co, billes=0;
  for(g->courante=g->prem, co=0 ; co< g->numco ; co++, g->courante=g->courante->adroite) {
    for(li=0 ; li < g->numli ; li++) {
      if(g->courante->bille[li] != 0)
	billes++;
      voisins(g, g->courante, li, co, g->courante->bille[li]); 
      if(g->nbselectionnes>1) {
	dechargerSelection(g, li, co);
	return 0;
      }
      dechargerSelection(g, li, co);
    }
  }
  return billes+1;
}

void voisinsBille(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  /* Déterminer la Colonne courante d'après co une fois pour tout le reste. */
  for(g->courante=g->prem, i=0 ; i<co ; i++)
    g->courante = g->courante->adroite;
  
  voisins(g, g->courante, li, co, g->courante->bille[li]);
}

void dechargerSelection(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  /* Oui, li ne sert qu'à être conforme au prototype universel de cette fonction (cad celui du modele sans listes chainees). */
  /* Nettoyage graphique */
  for(g->courante=g->prem, i=0 ; i<co ; i++)
    g->courante = g->courante->adroite;
  decharger(g, co);
  /* Nettoyage mémoire */
  for(i=0 ; i < g->nbselectionnes ; i++)
    g->selection[i][0] = g->selection[i][1] = -1; 
  g->nbselectionnes = 0;
}



void tomberBilles(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  /* Déterminer la Colonne courante d'après co une fois pour tout le reste. */
  for(g->courante=g->prem, i=0 ; i<co ; i++)
    g->courante = g->courante->adroite;
  /* Nettoyage mémoire */
  for(i=0 ; i < g->nbselectionnes ; i++)
    g->selection[i][0] = g->selection[i][1] = -1; 
  g->nbselectionnes = 0;

  retirer(g, g->courante, li, co, g->courante->bille[li]);
  tomber(g);
  decaler(g);
}






/* ------------------------------------------------------------------------------------------- */





/* Le coin des fonctions privées, destinées à n'être appelées que par les fonctions au-dessus. */





/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */


void retirer(Grille *g, Colonne *c, unsigned li, unsigned co, couleur teinte) {
  /* Si la bille n'est pas déjà sélectionnée... */
  if(!estSelectionnee(g, li, co)) {
    c->bille[li]=0;
    g->selection[g->nbselectionnes][0]=li;
    g->selection[g->nbselectionnes][1]=co;
    (g->nbselectionnes)++;
    
    /* HAUT */
    if(li > 0 && c->bille[li-1] == teinte) {
      retirer(g, c, li-1, co, teinte);       
    }  
    /* BAS */
    if(li < (g->numli-1) && c->bille[li+1] == teinte) {
      retirer(g, c, li+1, co, teinte);        
    }
    /* GAUCHE */
    if(c != g->prem && c->agauche->bille[li] == teinte) {    
      retirer(g, c->agauche, li, co-1, teinte); 
    }
    /* DROITE */
    if(c->adroite != g->prem && c->adroite->bille[li] == teinte) {
      retirer(g, c->adroite, li, co+1, teinte); 
    }
    
  }
}
void tomber(Grille *g) {
  unsigned li, co, trou, notok;
  /* Pour chaque Colonne... */
  for(g->courante=g->prem, co=0 ; co < g->numco ; co++, g->courante=g->courante->adroite) {
    
    /* Tant qu'un trou a une bille au-dessus... */
    do {
      for(notok=0, li=1 ; li < g->numli ; li++) {
	if(g->courante->bille[li] == 0 && g->courante->bille[li-1] != 0) {
	  trou=li;
	  notok=1;
	  break;
	}
      }
      if(notok==1) {
	/* On fait un décalage vers le bas. */
	for(li=trou ; li > 0 ; li--)
	  g->courante->bille[li] = g->courante->bille[li-1];
	g->courante->bille[li] = 0;
      }
    } while(notok == 1);
  }
}

int colonneVide(Grille *g, Colonne *c) {
  unsigned li;
  for(li=0 ; li < g->numli ; li++)
    if(c->bille[li] != 0)
      return 0;
  return 1;
}

void decaler(Grille *g) { 
  unsigned li, co;
  Colonne *tmp;
  couleur *vide;
  /* En partant de la droite, on oublie les premieres Colonnes vides. */
  for(g->courante=g->prem->agauche, co=0 ; co < g->numco ; co++, g->courante=g->courante->agauche)
    if(!colonneVide(g, g->courante)) {
      g->courante = g->courante->agauche;
      break;
    }
  /* On tombe sur une Colonne non-vide. Ensuite, la prochaine Colonne vide sera la base d'un décalage. Et la prochaine aussi. etc, sans dépasser le nombre de Colonnes. */
  for( ; co < g->numco ; co++, g->courante=g->courante->agauche) {
    if(colonneVide(g, g->courante)) {
      tmp = g->courante;
      vide = g->courante->bille;
      while(!colonneVide(g, tmp->adroite) && tmp != g->prem->agauche) {
	tmp->bille = tmp->adroite->bille;
	tmp = tmp->adroite;
      }
      tmp->bille = vide;
      for(li=0 ; li < g->numli ; li++)
	tmp->bille[li] = 0;
    }
  }
}

void decharger(Grille *g, unsigned co) {
  unsigned li, trouve;
  for(trouve=0, li=0 ; li < g->numli ; li++)
    if(estSelectionnee(g, li, co)) {
      trouve = 1;
      deseclairerBille(g, g->courante, li, co);
    }
  if(trouve == 1) {
    dechargerGauche(g, g->courante, co,  g->courante->adroite);
    dechargerDroite(g, g->courante, co,  g->courante->agauche);
  }
}

void dechargerGauche(Grille *g, Colonne *c, unsigned co, Colonne *org) {
  unsigned li;
  char trouve;
  if(c != org) {
    for(trouve=0, li=0 ; li < g->numli ; li++)
      if(estSelectionnee(g, li, co-1)) {
	trouve = 1;
	deseclairerBille(g, c->agauche, li, co-1);
      }
    if(trouve == 1)
      dechargerGauche(g, c->agauche, co-1, org);
  }
}
void dechargerDroite(Grille *g, Colonne *c, unsigned co, Colonne *org) {
  unsigned li;
  char trouve;
  if(c != org) {
    for(trouve=0, li=0 ; li < g->numli ; li++)
      if(estSelectionnee(g, li, co+1)) {
	trouve = 1;
	deseclairerBille(g, c->adroite, li, co+1);
      }
    if(trouve == 1)
      dechargerDroite(g, c->adroite, co+1, org);
  }
}

void voisins(Grille *g, Colonne *c, unsigned li, unsigned co, couleur teinte) {
  /* Si la bille n'est pas déjà sélectionnée... */
  if(!(estSelectionnee(g, li, co)) && teinte != 0) {
    g->selection[g->nbselectionnes][0]=li;
    g->selection[g->nbselectionnes][1]=co;
    (g->nbselectionnes)++;
    
    /* HAUT */
    if(li > 0 && c->bille[li-1] == teinte) {
      eclairerBille(g, c, li, co);
      voisins(g, c, li-1, co, teinte);       
    }  
    /* BAS */
    if(li < (g->numli-1) && c->bille[li+1] == teinte) {
      eclairerBille(g, c, li, co);
      voisins(g, c, li+1, co, teinte);        
    }
    /* GAUCHE */
    if(c != g->prem && c->agauche->bille[li] == teinte) {          
      eclairerBille(g, c, li, co);
      voisins(g, c->agauche, li, co-1, teinte); 
    }
    /* DROITE */
    if(c->adroite != g->prem && c->adroite->bille[li] == teinte) {
      eclairerBille(g, c, li, co);
      voisins(g, c->adroite, li, co+1, teinte); 
    }
    
  }
}

/* renvoie 1 si la bille a déjà été sélectionnée. */
/* partie du mécanisme d'anti-récursivité infinie. */
int estSelectionnee(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  for(i=0 ; i<g->nbselectionnes ; i++) {
    if(li == g->selection[i][0] && co == g->selection[i][1])
      return 1;
  }
  return 0;
}

/* Suppose que l'image de bille a des zones transparentes. */
void eclairerBille(Grille *g, Colonne *c, unsigned li, unsigned co) {
  ChoisirCouleurDessin(g->surbrillance);
  RemplirRectangle(g->rec->x + (co * (g->case_l + g->separateur)),
		   g->rec->y + (li * (g->case_h + g->separateur)),
		   g->case_l + 2*g->separateur,
		   g->case_h + 2*g->separateur);
  afficherBille(g, c, li, co);
}

void deseclairerBille(Grille *g, Colonne *c, unsigned li, unsigned co) {
  CopierZone(ECRAN_FOND_GRILLE, 0, 
	     g->rec->x + (co * (g->case_l + g->separateur)),
	     g->rec->y + (li * (g->case_h + g->separateur)),
	     g->case_l + 2*g->separateur,
	     g->case_h + 2*g->separateur,
	     g->rec->x + (co * (g->case_l + g->separateur)),
	     g->rec->y + (li * (g->case_h + g->separateur)));
  afficherBille(g, c, li, co);
}


