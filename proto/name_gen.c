/* name_gen.c - Steven Arnow <s@rdw.se>,  2013 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define	VOWELS			6
#define	CONSONANTS		15

const char *vowels[] = {
	"a", "e", "i", "o", "u", "y", NULL
};

const char *consonants[] = {
	"sh", "ch", "tj", "ck", "th", "s", "r", "z", "l", "m", "n", "d", "b", "k", "t", NULL
};

int main(int argc, char **argv) {
	int num, w, z, i, j;
	
	srand(time(NULL));
	
	for (i = 0; i < 20; i++) {
		num = (rand() % 4) + 4;
		w = rand() & 1;
		for (j = 0; num > 0; num--) {
			if (w)
				fprintf(stdout, "%s", vowels[rand() % VOWELS]);
			else {
				if ((z = rand() % CONSONANTS) == 3 && !j) {
					num++;
					continue;
				}
				fprintf(stdout, "%s", consonants[z]);
				j++;
			}
			w = !w;
		}
		
		fprintf(stdout, "\n");
	}
	
	return 0;
}
