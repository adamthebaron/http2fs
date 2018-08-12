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
	*num = *num + (u64int) *rep;
	//print("decint got: ");
	//for(int i = 0; i < *len; i++)
//		print("%x ", *(rep + i));
//	print("\n");
	//print("num is currently: %d\n", *num);
	if(*rep < max) {
	//	print("if ret\n");
	//	print("rep: %x len: %d\n" , *rep, *len);
		*num = (u64int) *rep;
	//	print("num is currently: %d\n", *num);
		return;
	} else
		while(*len > 1){
	//		print("rep: %x len: %d m: %d\n" , *rep, *len, m);
			*rep++;
			(*len)--;
			*num = *num + (u64int) (*rep & 0x7f) * pow(2, m);
	//		print("num is currently: %d\n", *num);
			m += 7;
		} //while (*len > 0);
	return;
}

/* decode from huffman representation in rep
 * and store in data
 */
void
huffmandec(u8int* data, u8int *rep, u16int len)
{
	u8int mask, currep, pos;
	HNode* current;

	current = rootnode;
	mask = 0;
	currep = 0;
	pos = 0;
	for(int i = 0; i < len; i++)
	{
		currep = rep[i];
		print("checking %b\n", currep);
		mask = 128;
		while(mask > 0)
		{
			if((currep & mask) == mask)
				current = current->right;
			else
				current = current->left;
			if(current->symbol != -1)
			{
				print("adding %c (hex %x)\n", current->symbol, current->symbol);
				data[pos] = current->symbol;
				pos++;
				current = rootnode;
			}
			mask = mask >> 1;
		}
	}
	return;
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