#include "../http2fs.h"
#include "u_util.h"

/*
 * u_shiftarr
 * direction: 0x01 == left, 0x00 == right
 */
void
u_shiftarr(u8int* arr, u64int arrlen, u8int shiftlen, u8int direction)
{
	u8int byte1, byte2;

	byte1 = byte2 = 0;
	byte1 = arr[0];
	byte2 = arr[1] & shiftlen;
	for(u64int i = 0; i < arrlen, i++)
	{

	}
	return;
}

/*
 * u_prefixarr
 */
void
u_prefixarr(u8int *s, int ssize, int *arr) {
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

/*
 * u_submem
 */
int
u_submem(u8int *haystack, int hsize, u8int *needle, int nsize) {
	int k; // num of bytes matched
	int *prefix; // prefix array (variable size)

	prefix = malloc(nsize * sizeof(int));
	u_prefixarr(needle, nsize, prefix);
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
