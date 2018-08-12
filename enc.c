#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "http2fs.h"

static uvlong huffmantable[] = {
	0x1ff8,
	0x7fffd8,
	0xfffffd8,
	0xfffffe2,
	0xfffffe3,
	0xfffffe4,
	0xfffffe5,
	0xfffffe6,
	0xfffffe7,
	0xfffffe8,
	0xffffea,
	0x3ffffffc,
	0xfffffe9,
	0xfffffea,
	0x3ffffffd,
	0xfffffeb,
	0xfffffec,
	0xfffffed,
	0xfffffee,
	0xfffffef,
	0xffffff0,
	0xffffff1,
	0xffffff2,
	0x3ffffffe,
	0xffffff3,
	0xffffff4,
	0xffffff5,
	0xffffff6,
	0xffffff7,
	0xffffff8,
	0xffffff9,
	0xffffffa,
	0xffffffb,
	0x14,
	0x3f8,
	0x3f9,
	0xffa,
	0x1ff9,
	0x15,
	0xf8,
	0x7fa,
	0x3fa,
	0x3fb,
	0xf9,
	0x7fb,
	0xfa,
	0x16,
	0x17,
	0x18,
	0x0,
	0x1,
	0x2,
	0x19,
	0x1a,
	0x1b,
	0x1c,
	0x1d,
	0x1e,
	0x1f,
	0x5c,
	0xfb,
	0x7ffc,
	0x20,
	0xffb,
	0x3fc,
	0x1ffa,
	0x21,
	0x5d,
	0x5e,
	0x5f,
	0x60,
	0x61,
	0x62,
	0x63,
	0x64,
	0x65,
	0x66,
	0x67,
	0x68,
	0x69,
	0x6a,
	0x6b,
	0x6c,
	0x6d,
	0x6e,
	0x6f,
	0x70,
	0x71,
	0x72,
	0xfc,
	0x73,
	0xfd,
	0x1ffb,
	0x7fff0,
	0x1ffc,
	0x3ffc,
	0x22,
	0x7ffd,
	0x3,
	0x23,
	0x4,
	0x24,
	0x5,
	0x25,
	0x26,
	0x27,
	0x6,
	0x74,
	0x75,
	0x28,
	0x29,
	0x2a,
	0x7,
	0x2b,
	0x76,
	0x2c,
	0x8,
	0x9,
	0x2d,
	0x77,
	0x78,
	0x79,
	0x7a,
	0x7b,
	0x7ffe,
	0x7fc,
	0x3ffd,
	0x1ffd,
	0xffffffc,
	0xfffe6,
	0x3fffd2,
	0xfffe7,
	0xfffe8,
	0x3fffd3,
	0x3fffd4,
	0x3fffd5,
	0x3fffd9,
	0x3fffd6,
	0x3fffda,
	0x3fffdb,
	0x3fffdc,
	0x3fffdd,
	0x3fffde,
	0x3fffeb,
	0x3fffdf,
	0x3fffec,
	0x3fffed,
	0x3fffd7,
	0x7fffe0,
	0xffffee,
	0x7fffe1,
	0x7fffe2,
	0x7fffe3,
	0x7fffe4,
	0x1fffdc,
	0x3fffd8,
	0x7fffe5,
	0x3fffd9,
	0x7fffe6,
	0x7fffe7,
	0xffffef,
	0x3fffda,
	0x1fffdd,
	0xfffe9,
	0x3fffdb,
	0x3fffdc,
	0x7fffe8,
	0x7fffe9,
	0x1fffde,
	0x7fffea,
	0x3fffdd,
	0x3fffde,
	0xfffff0,
	0x1fffdf,
	0x3fffdf,
	0x7fffeb,
	0x7fffec,
	0x1fffe0,
	0x1fffe1,
	0x3fffe0,
	0x1fffe2,
	0x7fffed,
	0x3fffe1,
	0x7fffee,
	0x7fffef,
	0xfffea,
	0x3fffe2,
	0x3fffe3,
	0x3fffe4,
	0x7ffff0,
	0x3fffe5,
	0x3fffe6,
	0x7ffff1,
	0x3ffffe0,
	0x3ffffe1,
	0xfffeb,
	0x7fff1,
	0x3fffe7,
	0x7ffff2,
	0x3fffe8,
	0x1ffffec,
	0x3ffffe2,
	0x3ffffe3,
	0x3ffffe4,
	0x7ffffde,
	0x7ffffdf,
	0x3ffffe5,
	0xfffff1,
	0x1ffffed,
	0x7fff2,
	0x1fffe3,
	0x3ffffe6,
	0x7ffffe0,
	0x7ffffe1,
	0x3ffffe7,
	0x7ffffe2,
	0xfffff2,
	0x1fffe4,
	0x1fffe5,
	0x3ffffe8,
	0x3ffffe9,
	0xffffffd,
	0x7ffffe3,
	0x7ffffe4,
	0x7ffffe5,
	0xfffec,
	0xfffff3,
	0xfffed,
	0x1fffe6,
	0x3fffe9,
	0x1fffe7,
	0x1fffe8,
	0x7ffff3,
	0x3fffea,
	0x3fffeb,
	0x1ffffee,
	0x1ffffef,
	0xfffff4,
	0xfffff5,
	0x3ffffea,
	0x7ffff4,
	0x3ffffeb,
	0x7ffffe6,
	0x3ffffec,
	0x3ffffed,
	0x7ffffe7,
	0x7ffffe8,
	0x7ffffe9,
	0x7ffffea,
	0x7ffffeb,
	0xffffffe,
	0x7ffffec,
	0x7ffffed,
	0x7ffffee,
	0x7ffffef,
	0x7fffff0,
	0x3ffffee,
	0x3fffffff
};

static uint huffmanlen[] = {
	13, /* 1 */
	23, /* 2 */
	28, /* 3 */
	28, /* 4 */
	28, /* 5 */
	28, /* 6 */
	28, /* 7 */
	28, /* 8 */
	28, /* 9 */
	24, /* 10 */
	30, /* 11 */
	28, /* 12 */
	28, /* 13 */
	30, /* 14 */
	28, /* 15 */
	28, /* 16 */
	28, /* 17 */
	28, /* 18 */
	28, /* 19 */
	28, /* 20 */
	28, /* 1 */
	28, /* 1 */
	30, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	28, /* 1 */
	 6, /* 1 */
	10, /* 1 */
	10, /* 1 */
	12, /* 1 */
	13, /* 1 */
	 6, /* 1 */
	 8, /* 1 */
	11, /* 1 */
	10, /* 1 */
	10, /* 1 */
	 8,
	11, 
	 8, 
	 6, 
	 6, 
	 6,
	 5, 
	 5, 
	 5, 
	 6, 
	 6, 
	 6, 
	 6, 
	 6, 
	 6, 
	 6, 
	 7, 
	 8, 
	15, 
	 6, 
	12, 
	10,
	13, 
	 6, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7,
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	 8, 
	 7, 
	 8, 
	13, 
	19, 
	13, 
	14, 
	 6,
	15, 
	 5, 
	 6, 
	 5, 
	 6, 
	 5, 
	 6, 
	 6, 
	 6, 
	 5, 
	 7, 
	 7, 
	 6, 
	 6, 
	 6, 
	 5,
	 6, 
	 7, 
	 6, 
	 5, 
	 5, 
	 6, 
	 7, 
	 7, 
	 7, 
	 7, 
	 7, 
	15, 
	11, 
	14, 
	13, 
	28,
	20, 
	22, 
	20, 
	20, 
	22, 
	22, 
	22, 
	23, 
	22, 
	23, 
	23, 
	23, 
	23, 
	23, 
	24, 
	23,
	24, 
	24, 
	22, 
	23, 
	24, 
	23, 
	23, 
	23, 
	23, 
	21, 
	22, 
	23, 
	22, 
	23, 
	23, 
	24,
	22, 
	21, 
	20, 
	22, 
	22, 
	23, 
	23, 
	21, 
	23, 
	22, 
	22, 
	24, 
	21, 
	22, 
	23, 
	23,
	21, 
	21, 
	22, 
	21, 
	23, 
	22, 
	23, 
	23, 
	20, 
	22, 
	22, 
	22, 
	23, 
	22, 
	22, 
	23,
	26, 
	26, 
	20, 
	19, 
	22, 
	23, 
	22, 
	25, 
	26, 
	26, 
	26, 
	27, 
	27, 
	26, 
	24, 
	25,
	19, 
	21, 
	26, 
	27, 
	27, 
	26, 
	27, 
	24, 
	21, 
	21, 
	26, 
	26, 
	28, 
	27, 
	27, 
	27,
	20, 
	24, 
	20, 
	21, 
	22, 
	21, 
	21, 
	23, 
	22, 
	22, 
	25, 
	25, 
	24, 
	24, 
	26, 
	23,
	26, 
	27, 
	26, 
	26, 
	27, 
	27, 
	27, 
	27, 
	27, 
	28, 
	27, 
	27, 
	27, 
	27, 
	27, 
	26,
	30
};

/* COMPLETE */
void
strtohex(uchar *hex, uchar *str) {
	uint i, len;

	len = strlen((char*) str);
	if(str[len - 1] == '\n')
		str[--len] = '\0';
	for(i = 0; i < len; i++)
		sprintf((char*) hex + i * 2, "%02X", *(str + i));
	return;
}

/* INCOMPLETE */
void
hextostr(char *hex, char *str) {
	int i, len;

	len = floor(strlen(hex) / 2);
	for(i = 0; i < len; i++)
		sscanf(hex + i * 2, "%x", *(str + i)); 
	return;
}

/* huffman encode string buf into string bout 
 * INCOMPLETE */
void
huffencode(uchar *buf, uint len, uchar *bout, uint lout) {
	uchar shift; //, bitn;
	uvlong mask, val, bitq;
	HNode hnode;
	uint _p, _c, bitn;

	bitq = 0;
	bitn = 0;
	while (len > 0) {
		print("using char %c, index num %d\n", *buf, *buf);
		hnode.code = huffmantable[*buf + 1];
		print("getting huffmanlen[%d]\n", *buf);
		hnode.len = huffmanlen[*buf + 1];
		len--;
		print("huffman code: %X length: %d\n\n", hnode.code, hnode.len);
		*buf++;
		// wat
		bitq = (bitq << hnode.len) | hnode.code;
		bitn += hnode.len;
		print("bitn now equals %d\n", bitn);
		while (bitn >= 8) {
			if (bout) {
				shift = bitn - 8;
				mask = (uvlong) 0xFF << shift;
				val = (bitq & mask);
				*bout = (val >> shift);
				bout++;
				bitq ^= val;
			}
			bitn -= 8;
		}
	}
	if (bitn > 0) {
		if (bout) {
			shift = 8 - bitn;
			mask = (1 << shift) - 1;
			*bout = ((bitq << shift) | mask);
			bout++;
		}
	}
}

/* huffman decode string buf into string bout
 * INCOMPLETE
void
huffdeocode(uchar *buf, uint len, uchar *bout, uint lout) {
	ulong tmp;
	uchar byte, bc, mask;

	while (len > 0) {
		byte = *buf;
		buf++;
		bc = 0x80;
		mask = 0x7F;
		while( bc > 0) {
			if ((byte & bc) == bc)
				tmp = 
		}
	}
}*/

void
main(int argc, char **argv) {
	uchar *outbuf, *buf, *finbuf;
	unsigned long long int huffmanmedude;

	buf = malloc(1024 * sizeof(char));
	outbuf = malloc(1024 * sizeof(char));
	finbuf = malloc(1024 * sizeof(char));
	strcpy((char*) buf, "private");
	print("converting %s\n", buf);
	strtohex(outbuf, buf);
	print("converted %s\n", outbuf);
	//print("converting back.\n");
	print("converting %s via huffman coding\n", buf);
	/* verify with curl/wireshark */
	huffencode(buf, strlen((char*) buf), outbuf, 0);
	print("converted %X\n", outbuf);
	exits(0);
}
