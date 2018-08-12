#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "hpack.h"

/* hex should contain the raw values that make up the
 * string str. size is size of str, so expect size of hex 
 * to be atleast 2 * size 
 * todo: figure out why i need this func */
void
strtohex(char *hex, char *str, long size) {
	for (int i = 0; i < size; i++)
		sprintf(hex + (i * 2), "%x", str[i]);
	return;
}

/* place encoded integer in rep
 * num is the integer to encode
 * bpref is the bit prefix (0 < n <= 8)
 * len is len in bytes */
void
encint(u8int* rep, u64int *len, u64int num, u8int bpref) {
	u64int max;

	if (bpref < 1 || bpref > 8)
		return;
	max = pow(2, bpref) - 1;
	print("encint: num: %d, max: %d\n", num, max);
	if (num < max) {
		*rep = (u8int) num;
		*len = 1;
		print("if ret\n");
		return;
	}
	else {
		*(rep + *len) = (u8int) max;
		*len = *len + 1;
		num = num - max;
		while (num >= 128) {
			print("len at %d\n", *len);
			*(rep + *len) = (u8int) (num % 128) + 128;
			*len = *len + 1;
			num = num / 128;
		}
		*(rep + *len) = (u8int) num;
		*len = *len + 1;
		print("else ret\n");
		return;
	}
}

/* encode data with huffman compression
 * and store in rep
 */
int
huffmanenc(u8int *data, u8int *rep, u16int len) {
	Byte b;
	HuffmanNode h, eos;
	u32int shift, tmp, pad;

	inittree();
	b.rem = 8;
	if (len == 0)
		return 0;
	for (int i = 0; *(data + i); i++) {
		h = HuffmanTable[*(data + i)];
		while (h.len > 0) {
			if (b.rem > h.len) {
				shift = b.rem - h.len;
				tmp = h.data << shift;
				b.val += tmp;
				b.rem = shift;
				h.len = 0;
			}
			else {
				shift = h.len - b.rem;
				tmp = h.data >> shift;
				b.val += tmp;
				b.rem = 0;
				h.data -= (tmp << shift);
				h.len = shift;
			}
			if (b.rem == 0) {
				*rep++ = b.val;
				b.val = 0;
				b.rem = 8;
			}
		}
	}
	if (b.rem > 0 && b.rem < 8) {
		eos = HuffmanTable[256];
		shift = eos.len - b.rem;
		pad = eos.data >> shift;
		b.val += pad;
		b.rem = 0;
		*rep++ = b.val;
	}
	return 1;
}
