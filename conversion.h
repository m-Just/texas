#ifndef CONVERSION_H
#define CONVERSION_H
#include "Card.h"

/* convert server language into local language(int) */
int ctoi(char* color, char* point) ;    // char* requires the header of the string

int colorof(int n) ;

int pointof(int n) ;

void change_to_Card(Card pubc[], Card handc[], int *hold, int *com); 

#endif
