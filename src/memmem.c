#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "http2fs.h"

void
prefixarr(u8int *s, int ssize, int *arr) {
	int k;

	k = -1;
	arr[0] = k;
	// scan array
	for (int i = 1; i < ssize; i++) {
		while (k > -1 && s[k + 1] != s[i])
			k = arr[k]; // prefix s[k] != suffix s[i]
		if (s[i] == s[k + 1])
			k++; // longest prefix s[k] is also a suffix of s[i]
		arr[i] = k;
	}
	return;
}

// knuth morris prat implementation
// hsize: size of haystack in bytes
// nsize: size of needle in bytes
// return offset in arr where match was found
// 0: match not found
int
memmem(u8int *haystack, int hsize, u8int *needle, int nsize) {
	int k; // num of bytes matched
	int *prefix; // prefix array (variable size)

	prefix = malloc(nsize * sizeof(int));
	prefixarr(needle, nsize, prefix);
	k = -1;
	for (int i = 0; i < hsize; i++) {
		while (k > -1 && needle[k + 1] != haystack[i])
			k = prefix[k];
		if (needle[k + 1] == haystack[i])
			k++;
		if (k + 1 == nsize) { // match found
			return i - k + 1;
		}
	}
	return 0;
}
