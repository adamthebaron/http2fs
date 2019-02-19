#include "../http2fs.h"
#include "u_util.h"

/*
 * u_printarr
 */
void
u_printarr(u8int* arr, u64int arrlen, char* name)
{
	print("printing %s (len %d): ", name, arrlen);
	for(u64int i = 0; i < arrlen; i++)
		print("%x ", arr[i]);
	print("\n");
	return;
}


/*
 * u_shiftarr
 * direction: 0x01 == left, 0x00 == right
 */
void
u_shiftarr(u8int* arr, u64int arrlen, u8int shiftlen, u8int direction)
{
	u8int tmpbits;

	u_printarr(arr, arrlen, "u_shiftarr");
	for(u64int i = 0; i < arrlen; i++)
	{
		arr[i] <<= shiftlen;
		if(i + 1 < arrlen)
		{
			tmpbits = arr[i + 1] & ((1 << shiftlen) - 1);
			print("got first %d bits (0x%x)\n", shiftlen, (1 << shiftlen) - 1);
			arr[i] |= tmpbits;
		}
	}
	u_printarr(arr, arrlen, "u_shiftarr");
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
