#include "../http2fs.h"
#include "h_hpack.h"

/* decode hpack encoded integer
 * rep is the hpack representation of the integer
 * len is the num of bytes stored in rep
 * bpref is the prefix size
 * num is the result */
void
h_decint(u8int *rep, u64int len, u64int *num, u8int bpref) {
	u64int m, max;

	m = 0;
	max = pow(2, bpref) - 1;
	print("found max: %d\n", max);
	print("len is %d, num is %d\ngot rep: ", len, *num);
	for(u64int i = 0; i < len; i++)
		print("%x ", rep[i]);
	print("\n");
	print("and bit prefix %d\n", bpref);
	if(*rep < max)
	{
		*num = (u64int) *rep;
		print("rep < max num is: %d\n", num);
		return;
	}
	else
		while(len > 1)
		{
			*rep++;
			len--;
			*num = *num + (u64int) (*rep & 0x7f) * pow(2, m);
			m += 7;
		}
	return;
}

/* decode from huffman representation in rep
 * and store in data
 */
void
h_huffmandec(u8int* data, u8int *rep, u16int len)
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
		mask = 0x80;
		while(mask > 0)
		{
			if((currep & mask) == mask)
			{
				print("currep & mask == 1 going right\n");
				current = current->right;
			}
			else
			{
				print("currep & mask == 0 going left\n");
				current = current->left;
			}
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

