#ifndef MENUDEF_H
#define MENUDEF_H

#define COULEUR_FOND_MENU 0x777777
#define COULEUR_FOND_BOUTON 0xAAAAAA
#define COULEUR_SURBRILLANCE_BOUTON 0xDDDDDD
#define COULEUR_POLICE_BOUTON 0
#define COULEUR_POLICE_TEXTE 0

int menuPrincipal(unsigned *hauteurs_police, char *pseudo, char *map, char *config);
void credits(unsigned *hauteurs_police);

#endif /* Guards */
