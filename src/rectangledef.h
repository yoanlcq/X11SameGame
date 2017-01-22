#ifndef RECTANGLE_H
#define RECTANGLE_H

typedef struct Rectangle_s {
  unsigned x,y,l,h;
} Rectangle ;

void remplirRectangleStruct(Rectangle *r);
int sourisDansRectangle(Rectangle *r);

#endif /* Guards */
