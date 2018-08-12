#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "hpack.h"

/* return encoded header
u64int
huffencode(char *buf, int len) {
	int codelen;
	unsigned long long int curcode, result;

	result = huffmantable[*buf];
	*buf++;
	while(*buf) {
		result = (result << huffmanlen[*buf]) | huffmantable[*buf]
		*buf++;
	}
	return result;
}
*/

/* return decoded header */
char*
huffdecode(char *buf, int len) {
	/*BTNode node;
	char *decbuf, cbuf, lbyte, symbyte;
	int index;

	decbuf = malloc(len * 8 * sizeof(char));
	cbuf = lbyte = symbyte = 0;
	index = 0;
	while(*buf++) {
		cbuf = cbuf << 8 | (uint) *buf;
		lbyte += 8;
		symbyte += 8;
		while (lbyte >= 8) {
			index = cbuf >> (lbyte - 8);
			node = node->children[index];
			if (node == nil)
				return nil;
			if (node->children == nil) {
				*decbuf++ = node->val;
				lbyte -= node->len;
			}
		}
	} */

	return nil;
}

int
encodeint(uint val, int prefixsize) {
	/*if (val < (pow(2, prefixsize) - 1))
		huffencode(&val, prefixsize)
	else {
		huffencode(pow(2, prefixsize) - 1), prefixsize);
		val = val - (pow(2, prefixsize) - 1);
		while (val >= 128) {
			huffencode((val % 128 + 128), 8)
			val = val / 128;
		}
		huffencode(val, 8);
	}
	return val;*/
	return -1;
}

int
decodeint(uint val, int prefixsize) {
	/*int m;

	huffmandecode(buf, prefixsize);
	if (val < pow(2, prefixsize) - 1)
		return val;
	m = 0;
	do {
		val = val + pow(2, m);
		m += 7;
	} while (val % 128 == 128)
	return val;*/
	return -1;
}