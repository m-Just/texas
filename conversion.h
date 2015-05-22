#include <stdio.h>

/* convert server language into local language(int) */
int ctoi(char* color, char* point) {      // char* requires the header of the string
	int c, p;
	if (point[0] > 64) {
		switch ((int)point[0]) {
			case 65: p = 1;  break;
			case 74: p = 11; break;
			case 81: p = 12; break;
			case 75: p = 13; break;
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
