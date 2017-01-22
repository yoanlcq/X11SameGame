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
      fprintf(stderr, "Erreur : nouvelleGrille() : malloc() de mémoire de sélection a échoué.\n");
      annulerGrille(g);
      return NULL;
    }
    g->selection[i][0] = g->selection[i][1] = -1; 
  }
  g->nbselectionnes = 0;

  /* Initialisation du bille. */

  g->bille = (couleur**) calloc(p_numli, sizeof(couleur*));
  for(i=0 ; i<p_numli ; i++)
    g->bille[i] = (couleur*) calloc(p_numco, sizeof(couleur));


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

  for(i=0 ; i < g->numli ; i++)
    free(g->bille[i]);
  free(g->bille);
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
  
  for(i=0 ; i < g->numli ; i++)
    if(g->bille[i] != NULL)
      free(g->bille[i]);
  if(g->bille != NULL)
    free(g->bille);
  if(g->selection != NULL) {
    for(i=0 ; i<(g->numli * g->numco) ; i++) {
      if(g->selection[i] != NULL)
	free(g->selection[i]);
      free(g->selection);
    }
  }
  free(g);
}

/* Remplit les billes de la Grille à partir d'un fichier. En cas de fin de fichier ou de lignes trop courtes, les billes manquantes seront noires-sans-image. */
void remplirGrilleAvecFichier(Grille *g, const char *nom_fichier) {
  unsigned li,co;
  char ch = 0; /* Empêcher un warning ennuyeux */
  FILE *fichier = fopen(nom_fichier, "r");

  if(fichier == NULL) {
    fprintf(stderr, "Attention : Echec d'ouverture du fichier \"%s\".\n", nom_fichier);
    return;
  }

  for(li=0 ; li < g->numli ; li++) {
    for(co=0 ; co < g->numco ; co++) {
      ch = fgetc(fichier);
      /* Désolé Luc, j'ai rejoint le côté case-break de la force */ 
      switch(ch) {
      case 'R': g->bille[li][co] = 0xFF0000; break;
      case 'J':
	  case 'Y': g->bille[li][co] = 0xFFFF00; break;
	  case 'V':
      case 'G': g->bille[li][co] = 0x00FF00; break;
      case 'C': g->bille[li][co] = 0x00FFFF; break;
      case 'B':
	  case 'U': g->bille[li][co] = 0x0000FF; break;
      case 'M': g->bille[li][co] = 0xFF00FF; break;
      case '1': g->bille[li][co] = g->palette[0]; break;
      case '2': g->bille[li][co] = g->palette[1]; break;
      case '3': g->bille[li][co] = g->palette[2]; break;
      case '4': g->bille[li][co] = g->palette[3]; break;
      case '5': g->bille[li][co] = g->palette[4]; break;
      case '6': g->bille[li][co] = g->palette[5]; break;
      case '\n': co = g->numco; break;
      default : g->bille[li][co] = 0x000000; break;
      }
      if(feof(fichier)) {
	fprintf(stderr, "Attention : Le fichier \"%s\" suppose des proportions inferieures.\n", nom_fichier);
	fclose(fichier);
	return;
      }
    }
    if(ch != '\n')
      while(fgetc(fichier) != '\n');
  }
  fclose(fichier);
}

/* Précondition : srand((unsigned) time(NULL)) a été invoqué. */
void remplirGrilleAvecHasard(Grille *g) {
  unsigned co, li;
  for(li=0 ; li < g->numli ; li++)
    for(co=0 ; co < g->numco ; co++)
      g->bille[li][co] = g->palette[rand()%g->numcouleurs];
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
  for(li=0 ; li < g->numli ; li++)
    for(co=0 ; co < g->numco ; co++)
      afficherBille(g, li, co);
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

void afficherBille(Grille *g, unsigned li, unsigned co) {
  unsigned i;

  for(i=0 ; i<g->numcouleurs ; i++)
    if(g->bille[li][co] == g->palette[i] && g->bille_sprite[i] != -1) {
      AfficherSprite(g->bille_sprite[i],
		     g->rec->x + (co * (g->case_l + g->separateur)),
		     g->rec->y + (li * (g->case_h + g->separateur)));
      return;
    }

  if(g->bille[li][co] == 0) {
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
  ChoisirCouleurDessin(g->bille[li][co]);
  RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	     g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	     g->case_l, g->case_h, 0, 360);
  /* Si ca vaut vraiment la peine... */
  if(g->case_l > 10 && g->case_h > 10) {
    ChoisirCouleurDessin(luminosite(g->bille[li][co], -100)); 
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*31/32, g->case_h*31/32, 0, 360);
    ChoisirCouleurDessin(luminosite(g->bille[li][co], -40)); 
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*18/20, g->case_h*18/20, 0, 360);
    ChoisirCouleurDessin(g->bille[li][co]);
    RemplirArc(g->rec->x + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*16/20, g->case_h*16/20, 0, 360);
    ChoisirCouleurDessin(luminosite(g->bille[li][co], 80));
    RemplirArc(g->rec->x + g->case_l/10 + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + g->case_h/10 + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*4/9, g->case_h*4/9, 0, 360);
    ChoisirCouleurDessin(luminosite(g->bille[li][co], 200));
    RemplirArc(g->rec->x + g->case_l/5 + (co * g->case_l) + ((co+1) * g->separateur),
	       g->rec->y + g->case_h/5 + (li * g->case_h) + ((li+1) * g->separateur),
	       g->case_l*1/7, g->case_h*1/7, 0, 360);
  }
}

/* Retourne 0 si non, ou nb de billes restantes + 1 si oui. */
int GrilleTerminee(Grille *g) {
  unsigned li, co, billes=0;
  for(li=0 ; li < g->numli ; li++) {
    for(co=0 ; co < g->numco ; co++) {
      if(g->bille[li][co] != 0)
	billes++;
      voisins(g, li, co, g->bille[li][co]); 
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
  voisins(g, li, co, g->bille[li][co]);
}

void dechargerSelection(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  /* Nettoyage graphique */
  for(li=0 ; li<g->numli ; li++)
    for(co=0 ; co<g->numco ; co++)
      if(estSelectionnee(g, li, co))
	deseclairerBille(g, li, co);
  /* Nettoyage mémoire */
  for(i=0 ; i < g->nbselectionnes ; i++)
    g->selection[i][0] = g->selection[i][1] = -1; 
  g->nbselectionnes = 0;
}


void tomberBilles(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  /* Nettoyage mémoire */
  for(i=0 ; i < g->nbselectionnes ; i++)
    g->selection[i][0] = g->selection[i][1] = -1; 
  g->nbselectionnes = 0;
  retirer(g, li, co, g->bille[li][co]);
  tomber(g);
  decaler(g);
}






/* ------------------------------------------------------------------------------------------- */





/* Le coin des fonctions privées, destinées à n'être appelées que par les fonctions au-dessus. */





/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */


void retirer(Grille *g, unsigned li, unsigned co, couleur teinte) {
  /* Si la bille n'est pas déjà sélectionnée... */
  if(!estSelectionnee(g, li, co)) {
    g->bille[li][co]=0;
    g->selection[g->nbselectionnes][0]=li;
    g->selection[g->nbselectionnes][1]=co;
    (g->nbselectionnes)++;
    
    /* HAUT */
    if(li > 0 && g->bille[li-1][co] == teinte) 
      retirer(g, li-1, co, teinte); 
    /* BAS */
    if(li < (g->numli-1) && g->bille[li+1][co] == teinte) 
      retirer(g, li+1, co, teinte);     
    /* GAUCHE */
    if(co > 0 && g->bille[li][co-1] == teinte)  
      retirer(g, li, co-1, teinte); 
    /* DROITE */
    if(co < (g->numco+1) && g->bille[li][co+1] == teinte)
      retirer(g, li, co+1, teinte); 
    
  }
}
void tomber(Grille *g) {
  unsigned li, co, trou, notok;
  /* Pour chaque Colonne... */
  for(co=0 ; co < g->numco ; co++) {
    
    /* Tant qu'un trou a une bille au-dessus... */
    do {
      for(notok=0, li=1 ; li < g->numli ; li++) {
	if(g->bille[li][co] == 0 && g->bille[li-1][co] != 0) {
	  trou=li;
	  notok=1;
	  break;
	}
      }
      if(notok==1) {
	/* On fait un décalage vers le bas. */
	for(li=trou ; li > 0 ; li--)
	  g->bille[li][co] = g->bille[li-1][co];
	g->bille[li][co] = 0;
      }
    } while(notok == 1);
  }
}

int colonneVide(Grille *g, unsigned co) {
  unsigned li;
  for(li=0 ; li < g->numli ; li++)
    if(g->bille[li][co] != 0)
      return 0;
  return 1;
}

void decaler(Grille *g) { 
  unsigned li, co, tmp;
  /* En partant de la droite, on oublie les premieres Colonnes vides. */
  for(co=g->numco-1 ; co > 0 && colonneVide(g, co) ; co--);
  /* On tombe sur une Colonne non-vide. Ensuite, la prochaine Colonne vide sera la base d'un décalage. Et la prochaine aussi. etc, sans dépasser le nombre de Colonnes. */
  for( ; co > 0 ; ) {
    co--; /* Pas le choix, ne pas toucher. */
    if(colonneVide(g, co)) {
      for(tmp=co ; tmp<g->numco-1 ; tmp++)
	for(li=0 ; li<g->numli ; li++)
	  g->bille[li][tmp] = g->bille[li][tmp+1];
      for(li=0 ; li<g->numli ; li++)
	g->bille[li][tmp] = 0; 
    }
  }
}

void voisins(Grille *g, unsigned li, unsigned co, couleur teinte) {
  /* Si la bille n'est pas déjà sélectionnée... */
  if(!(estSelectionnee(g, li, co)) && teinte != 0) {
    g->selection[g->nbselectionnes][0]=li;
    g->selection[g->nbselectionnes][1]=co;
    (g->nbselectionnes)++;
    
    /* HAUT */
    if(li > 0 && g->bille[li-1][co] == teinte) {
      eclairerBille(g, li, co);
      voisins(g,li-1, co, teinte);       
    }  
    /* BAS */
    if(li < (g->numli-1) && g->bille[li+1][co] == teinte) {
      eclairerBille(g, li, co);
      voisins(g, li+1, co, teinte);        
    }
    /* GAUCHE */
    if(co > 0 && g->bille[li][co-1] == teinte) {          
      eclairerBille(g, li, co);
      voisins(g, li, co-1, teinte); 
    }
    /* DROITE */
    if(co < (g->numco-1) && g->bille[li][co+1] == teinte) {
      eclairerBille(g, li, co);
      voisins(g, li, co+1, teinte); 
    }
    
  }
}

/* renvoie 1 si la bille a déjà été sélectionnée. */
/* partie du mécanisme d'anti-récursivité infinie. */
int estSelectionnee(Grille *g, unsigned li, unsigned co) {
  unsigned i;
  for(i=0 ; i<g->nbselectionnes ; i++) 
    if(li == g->selection[i][0] && co == g->selection[i][1])
      return 1;
  return 0;
}

/* Suppose que l'image de bille a des zones transparentes. */
void eclairerBille(Grille *g, unsigned li, unsigned co) {
  ChoisirCouleurDessin(g->surbrillance);
  RemplirRectangle(g->rec->x + (co * (g->case_l + g->separateur)),
		   g->rec->y + (li * (g->case_h + g->separateur)),
		   g->case_l + 2*g->separateur,
		   g->case_h + 2*g->separateur);
  afficherBille(g, li, co);
}

void deseclairerBille(Grille *g, unsigned li, unsigned co) {
  CopierZone(ECRAN_FOND_GRILLE, 0, 
	     g->rec->x + (co * (g->case_l + g->separateur)),
	     g->rec->y + (li * (g->case_h + g->separateur)),
	     g->case_l + 2*g->separateur,
	     g->case_h + 2*g->separateur,
	     g->rec->x + (co * (g->case_l + g->separateur)),
	     g->rec->y + (li * (g->case_h + g->separateur)));
  afficherBille(g, li, co);
}


