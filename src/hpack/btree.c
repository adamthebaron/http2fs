#include <u.h>
#include <libc.h>
#include <stdio.h>

#include "../http2fs.h"
#include "hpack.h"

HNode*
newnode(void) {
	HNode *node;

	node = malloc(1 * sizeof(HNode));
	node->symbol = -1;
	node->len = 0;
	node->left = nil;
	node->right = nil;
	return node;
}

void
freenode(HNode *node)
{
	if(node->left != nil)
		freenode(node->left);
	if(node->right != nil)
		freenode(node->right);
	if(node->left == nil && node->right == nil)
		free(node);
	node = nil;
	return;
}

void
inittree(void)
{
	HuffmanNode node;
	HNode* current;
	HNode* next;
	u32int mask;

	rootnode = newnode();
	current = nil;
	next = nil;
	mask = 0;
	for(int i = 0; i < 258; i++)
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
					next = newnode();
					current->right = next;
				}
				current = next;
			}
			else
			{
				next = current->left;
				if(next == nil)
				{
					next = newnode();
					current->left = next;
				}
				current = next;
			}
		}
		current->symbol = i;
		//print("added %c at addr %x\n", i, current);
	}
	return;			
}

void
printtree(HNode* root)
{
	HNode* cur;

	cur = root;
	if(cur->left != nil)
		printtree(cur->left);
	if(cur->right != nil)
		printtree(cur->right);
	print("at addr %x with symbol %c\n", cur, cur->symbol);
	return;
}