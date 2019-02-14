#include "../http2fs.h"
#include "h_hpack.h"

static u8int lookup[16] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
	0xa, 0xb, 0xc, 0xd, 0xe, 0xf
};

u8int
h_reverse(u8int n)
{
	return (lookup[n & 0xF] << 4) | lookup[n >> 4];
}

/* hex should contain the raw values that make up the
 * string str. size is size of str, so expect size of hex 
 * to be atleast 2 * size 
 * todo: figure out why i need this func */
void
h_strtohex(char *hex, char *str, long size) {
	for (int i = 0; i < size; i++)
		sprintf(hex + (i * 2), "%x", str[i]);
	return;
}

/* place encoded integer in rep
 * num is the integer to encode
 * bpref is the bit prefix (0 < n <= 8)
 * len is len in bytes */
void
h_encint(u8int* rep, u64int *len, u64int num, u8int bpref) {
	u64int max;

	if (bpref < 1 || bpref > 8)
		return;
	max = pow(2, bpref) - 1;
	//print("encint: num: %d, max: %d\n", num, max);
	if (num < max) {
		*rep = (u8int) num;
		*len = 1;
		//print("if ret\n");
		return;
	}
	else {
		*(rep + *len) = (u8int) max;
		*len = *len + 1;
		num = num - max;
		while (num >= 128) {
			//print("len at %d\n", *len);
			*(rep + *len) = (u8int) (num % 128) + 128;
			*len = *len + 1;
			num = num / 128;
		}
		*(rep + *len) = (u8int) num;
		*len = *len + 1;
		//print("else ret\n");
		return;
	}
}

/*
 * h_huffmanenc
 */
void
h_huffmanenc(u8int *data, u8int *rep, u16int len, u8int *replen)
{
	// yes
	u8int *pos;
	u32int cur;
	u32int n;
	HuffmanSymbol node;

	print("in h_huffmanenc with %s\n", (char*) data);
	pos = rep;
	n = 0;
	cur = 0;
	for(int i = 0; i < len; i++)
	{
		node = HuffmanTable[*(data + i)];
		print("encoding char %c: 0x%x %0b\n", *(data + i), node.data, node.data);
		cur <<= node.len;
		cur |= node.data;
		n += node.len;

		while(n >= 8)
		{
			n -= 8;
			*pos++ = cur >> n;
		}
	}
	if(n > 0)
	{
		cur <<= (8 - n);
		cur |= (0xFF >> n);
		*pos++ = cur;
	}
	*replen = pos - rep;
	print("rep: ");
	for(int i = 0; i < *replen; i++)
		print("%0b ", rep[i]);
	print("\n");
	for(int i = 0; i < *replen; i++)
		print("%x ", rep[i]);
	print("\n");
	return;
}
