#include "univ.h"
#include "rectangledef.h"

void remplirRectangleStruct(Rectangle *r) {
  RemplirRectangle(r->x,r->y,r->l,r->h);
}

int sourisDansRectangle(Rectangle *r) {
  return (_X > r->x && _Y > r->y && _X < (r->x+r->l) && _Y < (r->y+r->h));
}
