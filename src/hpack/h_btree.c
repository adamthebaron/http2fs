#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "h_hpack.h"

HNode*
h_newnode(void) {
	HNode *node;

	node = calloc(1, sizeof(HNode));
	node->symbol = -1;
	node->len = 0;
	node->left = nil;
	node->right = nil;
	return node;
}

void
h_freenode(HNode *node)
{
	if(node->left != nil)
		h_freenode(node->left);
	if(node->right != nil)
		h_freenode(node->right);
	if(node->left == nil && node->right == nil)
		free(node);
	node = nil;
	return;
}

void
h_inittree(void)
{
	HuffmanSymbol node;
	HNode* current;
	HNode* next;
	u32int mask;

	rootnode = h_newnode();
	current = nil;
	next = nil;
	mask = 0x000000000;
	for(int i = 0; i < 257; i++)
	{
		node = HuffmanTable[i];
		current = rootnode;
		while(node.len > 0)
		{
			node.len--;
			mask = 1 << node.len;
			if((node.data & mask) == mask)
			{
				next = current->right;
				if(next == nil)
				{
					next = h_newnode();
					current->right = next;
				}
				current = next;
			}
			else
			{
				next = current->left;
				if(next == nil)
				{
					next = h_newnode();
					current->left = next;
				}
				current = next;
			}
		}
		current->symbol = i;
	}
	return;			
}

void
h_printtree(HNode* root)
{
	HNode* cur;

	cur = root;
	if(cur->left != nil)
		h_printtree(cur->left);
	if(cur->right != nil)
		h_printtree(cur->right);
	print("at addr %x with symbol %c\n", cur, cur->symbol);
	return;
}
