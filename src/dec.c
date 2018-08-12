#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "hpack.h"

/* decode hpack encoded integer
 * rep is the hpack representation of the integer
 * len is the num of bytes stored in rep
 * bpref is the prefix size
 * num is the result */
void
decint(u8int *rep, u64int *len, u64int *num, u8int bpref) {
	u64int m, max;

	m = 0;
	max = pow(2, bpref) - 1;
	*num += (u64int) *rep;

	if(*rep < max) {
		print("if ret\n");
		*num = (u64int) *rep;
		return;
	} else
		do {
			*rep++;
			print("rep: %x\n", *rep);
			*len = *len - 1;
			*num += (u64int) (*rep & 0x7f) << m;
			m += 7;
		} while (*len > 0);
	return;
}

/* decode data from huffman representation in rep
 * and store in data
 */
int
huffmandec(u8int *data, u8int *rep, u16int len) {
	u8int mask;
	HNode *cur;

	if (len == 0)
		return len;
	cur = root;
	for(int i = 0; *(rep + i); i++) {
		mask = 0x80;
		while (mask > 0x0) {
			if (*rep & mask == mask)
				cur = cur->right;
			else
				cur = cur->left;
			if (cur->huffman->data != -1) {
				*data++ = cur->huffman->data;
				cur = root;
			}
			mask >>= 1;
		}
	}
	return 1;
}

/** indexed header field representation 6.1 **/

/* decode indexed header (0x80) 6.1
 *   0   1   2   3   4   5   6   7
 * +---+---+---+---+---+---+---+---+
 * | 1 |        Index (7+)         |
 * +---+---------------------------+ 
u8int *
dechdrind(u8int* buf) {

}*/

/** literal header field representations 6.2 **/

/* decode literal header with indexing (0x40) 6.2.1
 *   0   1   2   3   4   5   6   7
 * +---+---+---+---+---+---+---+---+
 * | 0 | 1 |      Index (6+)       |
 * +---+---+-----------------------+
 * | H |     Value Length (7+)     |
 * +---+---------------------------+
 * | Value String (Length octets)  |
 * +-------------------------------+
 * indexed name
 *   0   1   2   3   4   5   6   7
 * +---+---+---+---+---+---+---+---+
 * | 0 | 1 |           0           |
 * +---+---+-----------------------+
 * | H |     Name Length (7+)      |
 * +---+---------------------------+
 * |  Name String (Length octets)  |
 * +---+---------------------------+
 * | H |     Value Length (7+)     |
 * +---+---------------------------+
 * | Value String (Length octets)  |
 * +-------------------------------+
 * new name
u8int*
dechdrlitind(u8int* buf) {

}*/

/* decode literal header without indexing (0x0) 6.2.2 
u8int*
dechdrlitnind(u8int* buf) {

}*/

/* decode literal header never index (0x10) 6.2.3 
u8int*
dechdrlitneind(u8int* buf) {

}*/