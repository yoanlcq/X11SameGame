#include "univ.h"
#include "chronodef.h"

void demarrerChrono(Chrono *c, unsigned val) {
  c->usec_max = val * 100000;
  c->usec_depart = Microsecondes();
  c->sec  = val/10;
  c->dsec = val%10;
}

int lireChrono(Chrono *c) {
  if(c->sec == 0 && c->dsec == 0)
    return 0;
  c->dsec = (c->usec_max - (Microsecondes() - c->usec_depart));
  c->dsec /= 100000;
  c->sec  = c->dsec/10;
  c->dsec %= 10;
  return 1;
}

