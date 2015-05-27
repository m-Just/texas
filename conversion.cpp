#include "conversion.h"
#include "Card.h"
#include <stdlib.h>

int ctoi(char* color, char* point) {      // char* requires the header of the string
	int c, p;
	if (point[0] > 64) {
		switch ((int)point[0]) {
			case 'A': p = 1;  break;
			case 'J': p = 11; break;
			case 'Q': p = 12; break;
			case 'K': p = 13; break;
		}
	} else 	p = atoi(point);

	switch(color[0]) {
		case 'H': c = 0; break;
		case 'D': c = 1; break;
		case 'S': c = 2; break;
		case 'C': c = 3; break;
	}

	return (p-1)*4 + c;
}


int colorof(int n) {
	return n % 4;
}

int pointof(int n) {
	return n / 4 + 1;
}

//convert read to win_rate
void change_to_Card(Card pubc[], Card handc[], int *hold, int *com)
{
	int i;
	for(i = 1; i <= hold[0]; i++){
		handc[i].color = colorof(hold[i]);
		handc[i].val = pointof(hold[i]);
	}
	for(i = 1; i <= com[0]; i++){
		pubc[i].color = colorof(com[i]);
		pubc[i].val = pointof(com[i]);
	}
}
