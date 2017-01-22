#ifndef CHRONODEF_H
#define CHRONODEF_H

typedef struct Chrono_s {
  unsigned usec_depart, usec_max, sec, dsec;
} Chrono ;

void demarrerChrono(Chrono *c, unsigned val);

/* retourne 0 si le chrono a atteint le 0. */
int lireChrono(Chrono *c);

#endif /* Guards */
