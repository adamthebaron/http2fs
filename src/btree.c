#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "http2fs.h"
#include "hpack/hpack.h"

HNode*
newnode(void) {
	HNode *node = malloc(sizeof(HNode));
	node->huffman = malloc(sizeof(HuffmanNode));
	node->left = nil;
	node->right = nil;
	return node;
}

void
freenode(HNode *node) {
	free(node->huffman);
	free(node->left);
	free(node->right);
	free(node);
	return;
}

void
inittree(void) {
	root = gentree();
	return;
}

HNode*
gentree(void) {
	HNode *treeroot, *cur, *next;
	uint len;
	u64int data;
	u32int mask;

	treeroot = newnode();
	for (uint i = 0; i < 257; i++) {
		cur = treeroot;
		len = HuffmanTable[i].len;
		while (len > 0) {
			len--;
			mask = (1 << len);
			if ((HuffmanTable[i].data & mask) == mask) {
				next = cur->right;
				if (next == nil) {
					next = newnode();
					cur->right = next;
				}
				cur = next;
			}
			else {
				next = cur->left;
				if (next == nil) {
					next = newnode();
					cur->left = next;
				}
				cur = next;
			}
		}
		print("storing data elem %d of size %d\n", i, HuffmanTable[i].len);
		cur->huffman->data = HuffmanTable[i].data;
		cur->huffman->len = HuffmanTable[i].len;
	}
	return treeroot;
}
