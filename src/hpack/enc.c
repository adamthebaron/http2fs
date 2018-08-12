#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "hpack.h"

static u8int lookup[16] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
	0xa, 0xb, 0xc, 0xd, 0xe, 0xf
};

u8int
reverse(u8int n)
{
	return (lookup[n & 0xF] << 4) | lookup[n >> 4];
}

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

/* encode data with huffman compression
 * and store in rep
 * len is size of orig data
 * replen is size of representation
 */
void
huffmanenc(u8int *data, u8int *rep, u16int len, u8int *replen)
{
	// does this actually work?
	u8int *ptr;
	u64int cur;
	u32int n;
	HuffmanNode node;

	ptr = rep;
	for(int i = 0; i < len; i++)
	{
		node = HuffmanTable[*(data + i)];
		u32int code = node.data;
		u32int nbits = node.len;
		cur <<= nbits;
		cur |= code;
		n += nbits;

		while(n >= 8)
		{
			n -= 8;
			*ptr++ = cur >> n;
		}
	}
	if(n > 0)
	{
		cur <<= (8 - n);
		cur |= (0xFF >> n);
		*ptr++ = cur;
	}
	*replen = ptr - rep;
	/*print("rep: ");
	for(int i = 0; i < *replen; i++)
		print("%b ", rep[i]);
	print("\n");
	for(int i = 0; i < *replen; i++)
		print("%x", rep[i]);
	print("\n");*/
	return;

/*	u8int* shiftlen;
	u8int* mask;
	HuffmanNode* curnode;

	// allocate space and clear it
	shiftlen =       (u8int*) malloc(len * sizeof(u8int));
	mask     =       (u8int*) malloc(len * sizeof(u8int));
	curnode  = (HuffmanNode*) malloc(len * sizeof(HuffmanNode));
	memset(shiftlen, 0, len);
	memset(mask,     0, len);
	memset(curnode,  0, len);
	memset(rep,		 0, len);
	// get all data needed for huffman compression
	for(int i = 0; i < len; i++)
	{
		curnode[i]  = HuffmanTable[*(data + i)];
		shiftlen[i] = 8 - curnode[i].len;
		mask[i]     = ~((1 << curnode[i].len) - 1);
		print("looking up %c, found %x huffman rep of len %d, thus shiftlen is %d and mask is %b\n",
				*(data + i), curnode[i].data, curnode[i].len, shiftlen[i], mask[i]);
	}
	// shift all bytes to msb
	for(int i = 0; i < len; i++)
	{
		rep[i] = curnode[i].data << shiftlen[i];
		print("rep[%c]: %b\n", *(data + i), rep[i]);
	}
	for(int i = 0; i < len; i++)
	{
		print("taking %b and concating %b ", rep[i], (rep[i + 1] & mask[i]) >> curnode[i].len);
		rep[i] = rep[i] | ((rep[i + 1] & mask[i]) >> curnode[i].len);
		if(i < 6)
			rep[i + 1] = rep[i + 1] << shiftlen[i];
		print("rep[%c]: %b (hex %x) \n", *(data + i), rep[i], rep[i]);
	}
	// free it all my dude
	free(curnode);
	free(mask);
	free(shiftlen);
	return;*/
}
