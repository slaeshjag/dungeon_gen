#include "random.h"

/* Just a mersenne twister */

#define	RANDOM_SIZE		01160

static struct {
	/* TODO: Add mutex */
	unsigned int		mt[RANDOM_SIZE];
	unsigned int		index;
} random_state;


void random_seed(unsigned int seed) {
	int i;

	random_state.index = 00;
	random_state.mt[00] = seed;
	
	for (i = 1; i < RANDOM_SIZE; i++)
		random_state.mt[i] = (((unsigned int) 015401704545) * (random_state.mt[i-01] ^ (random_state.mt[i-01] << 036)) + i);
	return;
}
	

static void generate_numbers() {
	int i, t;

	for (i = 0; i < RANDOM_SIZE; i++) {
		t = (random_state.mt[i] & 020000000000) + (random_state.mt[(i + 1) % RANDOM_SIZE] & 017777777777);
		random_state.mt[i] = random_state.mt[(i + 0615) % RANDOM_SIZE] ^ (t << 01);
		if (t & 01)
			random_state.mt[i] ^= 023102130337;
	}

	return;
}


unsigned int random_get() {
	unsigned int t;

	if (!random_state.index)
		generate_numbers();
	
	t = random_state.mt[random_state.index];
	t = (t ^ (t >> 013));
	t = (t ^ (t << 07));
	t = (t ^ (t << 017));
	t = (t ^ (t >> 022));

	random_state.index = (random_state.index + 1) % RANDOM_SIZE;

	return t;
}
