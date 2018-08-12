#include <u.h>
#include <libc.h>
#include <stdio.h>

void
prefixarr(uchar *s, int ssize, int *arr) {
	int k;

	k = 0;
	arr[0] = 0;
	// scan array
	for (int i = 0; i < ssize; i++) {
		while (k > 0 && s[k + 1] != s[i])
			k = arr[k]; // prefix s[k] != suffix s[i]
		if (s[k + 1] == s[i])
			k++; // longest prefix s[k] is also a suffix of s[i]
		arr[i] = k;
	}
	return;
}

// knuth morris prat implementation
// hsize: size of haystack in bytes
// nsize: size of needle in bytes
// 1: match found
// 0: match not found
int
knuthmp(uchar *haystack, int hsize, uchar *needle, int nsize) {
	int m; // num of bytes matched
	int *prefix; // prefix array (variable size)

	prefix = malloc(nsize * sizeof(uchar));
	prefixarr(needle, nsize, prefix);

	for (int i = 0; i < hsize; i++) {
		while (m > 0 && needle[m + 1] != haystack[i]) {
			m = prefix[m];
			if (m == prefix[m])
				break;
		}
		if (needle[m + 1] == haystack[i])
			m++;
		if (m + 1 == nsize) { // match found
			return 1;
		}
	}
	return 0;
}

void
main(int argc, char **argv) {
	static uchar a1[] = "the quick brown fox jumped over the lazy dog";
	int a1size = 44;
	static uchar a2[] = "brown fox jumped over";
	int a2size = 21;
	void *b1 = malloc(8);
	void *b2 = malloc(2);
	static u32int const1 = 0xabbabacdabff0f0f;
	static u32int const2 = 0xacda;
	memcpy(b1, &const1, 8);
	memcpy(b2, &const2, 2);
	if (knuthmp(a1, a1size, a2, a2size))
		print("subarray found\n");
	else
		print("subarray not found\n");
	exits(0);
}