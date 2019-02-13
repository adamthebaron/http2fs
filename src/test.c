/*#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "http2fs.h"

void
main(int argc, char** argv) {
	char *a1 = "aaaaaaaaa\0 	aabbbababbbaaba";
	char a1size = 27;
	char a2[] = "abba";
	char a2size = 4;
	u16int *c1;
	u16int c2[4];

	c1 = malloc(27 * sizeof(u16int));
	for (int i = 0; i < a1size; i++)
		memcpy(&c1[i], &a1[i], 1);
	for (int i = 0; i < a2size; i++)
		memcpy(&c2[i], &a2[i], 1);
	int stuff;
	if ((stuff = submem(c1, a1size, c2, a2size)))
		print("found match at index: %d\n", stuff);
	else
		print("match not found\n");
	exits(0);
}*/
