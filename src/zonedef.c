#include "univ.h"
#include "chronodef.h"
#include "zonedef.h"

Zone* nouvelleZone(unsigned p_x, unsigned p_y, unsigned p_l, unsigned p_h,
		   unsigned p_taille_police, couleur p_couleur_police, couleur p_fond)
{
  Zone* nouvelle = (Zone*) malloc(sizeof(Zone));
  if(nouvelle == NULL) {
    fprintf(stderr, "Attention : nouvelleZone() : malloc() a échoué.\n");
    return NULL; /* On lui épargne les assignations */
  }
  nouvelle->rec = (Rectangle*) malloc(sizeof(Rectangle));
  if(nouvelle->rec == NULL) {
    fprintf(stderr, "Attention : nouvelleZone() : malloc() a échoué.\n");
    free(nouvelle);
    return NULL; /* On lui épargne les assignations */
  }
  nouvelle->rec->x = p_x;
  nouvelle->rec->y = p_y;
  nouvelle->rec->l = p_l;
  nouvelle->rec->h = p_h;
  nouvelle->taille_police  = p_taille_police;
  nouvelle->couleur_police = p_couleur_police;
  nouvelle->fond = p_fond;
  return nouvelle;
}

void effacerZone(Zone *z) {
  free(z->rec);
  free(z);
}

/* Garantit l'affichage d'un texte centré dans la boite. */
void afficherZone(Zone *z, char *texte) {
  ChoisirCouleurDessin(z->fond);
  remplirRectangleStruct(z->rec);
  ChoisirCouleurDessin(z->couleur_police);
  EcrireTexte(z->rec->x + z->rec->l/2 - TailleChaineEcran(texte, z->taille_police)/2, 
	      z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2,
	      texte, z->taille_police);
}

/* Permet d'utiliser une Zone comme un champ d'entrée de texte, sur une ligne. */
/* Fonction bloquante jusqu'à appui sur Entrée ou Echap. */
void entreeTexteZone(Zone *z, char *str, unsigned size) {
  unsigned t=0, curseur=0, fin=0, i; /* t=0 pour empecher un Warning */
  Chrono chrono_curseur;
  int etat_curseur;
  char *str_sauvegarde = (char*) malloc(size*sizeof(char));

  /* Si la chaine est non vide, on se positionne correctement. */
  /* Au passage, on garde une copie de la chaine pour le cas où on presse Echap. */
  for(i=0 ; str[i] != '\0' && i<size-1 ; i++, curseur++, fin++)
    str_sauvegarde[i] = str[i];
      
  /* On ne sait pas ce qu'elle contient après. Faire ça empêche la vue de caractères bizarres. */
  for( ; i<size ; i++)
    str[i] = str_sauvegarde[i] = '\0';

  /* Vider la file de touches */
  while(ToucheEnAttente())
    Touche();

  /* ACTUALISATION AFFICHAGE */
  ChoisirCouleurDessin(z->fond);
  remplirRectangleStruct(z->rec);
  ChoisirCouleurDessin(z->couleur_police);
  EcrireTexte(z->rec->x, 
	      z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2,
	      str, z->taille_police);

  etat_curseur = 0;
  demarrerChrono(&chrono_curseur, 0);

  do {
    if(!ToucheEnAttente()){
      if(etat_curseur == 1) {
	if(lireChrono(&chrono_curseur) == 0) {
	  etat_curseur = 0;
	  demarrerChrono(&chrono_curseur, TEMPS_CLIGNOTEMENT_CURSEUR_TEXTE);
	  /* Gommer le curseur */
	  ChoisirCouleurDessin(z->fond);
	  DessinerSegment(z->rec->x + curseur*TailleChaineEcran("O",z->taille_police),
			  z->rec->y + z->rec->h/2 - TailleSupPolice(z->taille_police)/2,
			  z->rec->x + curseur*TailleChaineEcran("O",z->taille_police), 
			  z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2);
	}
      } else {
	if(lireChrono(&chrono_curseur) == 0) {
	  etat_curseur = 1;
	  demarrerChrono(&chrono_curseur, TEMPS_CLIGNOTEMENT_CURSEUR_TEXTE);
	  /* Tracer le curseur */
	  ChoisirCouleurDessin(z->couleur_police);
	  DessinerSegment(z->rec->x + curseur*TailleChaineEcran("O",z->taille_police),
			  z->rec->y + z->rec->h/2 - TailleSupPolice(z->taille_police)/2,
			  z->rec->x + curseur*TailleChaineEcran("O",z->taille_police), 
			  z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2);
	}
      }
    }

    /* PARTIE ECOUTE TOUCHE */

    if(ToucheEnAttente()) {
      t = Touche();
      ChoisirCouleurDessin(z->fond);
      remplirRectangleStruct(z->rec);

      if((t >= 32 && t<=126) && fin < size-1) {
	if(curseur < fin)
	  for(i=fin ; i>curseur ; i--)
	    str[i] = str[i-1];
	str[curseur] = t;
	curseur++;
	fin++;
      }
      else if(t >= XK_KP_0 && t <= XK_KP_9 && fin < size-1) {
	if(curseur < fin)
	  for(i=fin ; i>curseur ; i--)
	    str[i] = str[i-1];
	str[curseur] = t - XK_KP_0 + '0';
	curseur++;
	fin++;
      }
      else if(t == XK_Delete && curseur < fin) {
	for(i=curseur ; i<fin ; i++)
	  str[i] = str[i+1];
	fin--;
      }
      else if(t == XK_BackSpace && curseur > 0) {
	curseur--;
	for(i=curseur ; i<fin ; i++)
	  str[i] = str[i+1];
	fin--;
      }
      else if(t == XK_Left && curseur > 0)
	curseur--;
      else if(t == XK_Right && curseur < fin)
	curseur++;

      /* ACTUALISATION AFFICHAGE */
      ChoisirCouleurDessin(z->fond);
      remplirRectangleStruct(z->rec);
      ChoisirCouleurDessin(z->couleur_police);
      EcrireTexte(z->rec->x, 
		  z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2,
		  str, z->taille_police);

      etat_curseur = 0;
      demarrerChrono(&chrono_curseur, 0);

    } /* FIN ECOUTE TOUCHE */
  } while(t != XK_Escape && t != XK_Return && t != XK_KP_Enter && !SourisCliquee());

  str[fin] = '\0';

  if(t == XK_Escape)
    for(i=0 ; i<size ; i++)
      str[i] = str_sauvegarde[i];

  free(str_sauvegarde);

  /* On gomme le curseur */
  ChoisirCouleurDessin(z->fond);
  DessinerSegment(z->rec->x + curseur*TailleChaineEcran("O",z->taille_police),
		  z->rec->y + z->rec->h/2 - TailleSupPolice(z->taille_police)/2,
		  z->rec->x + curseur*TailleChaineEcran("O",z->taille_police),
		  z->rec->y + z->rec->h/2 + TailleSupPolice(z->taille_police)/2);
}

