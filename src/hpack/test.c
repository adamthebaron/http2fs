#include <u.h>
#include <libc.h>

#include "../http2fs.h"
#include "h_hpack.h"

void
main(int argc, char** argv)
{
	char* str = "lemonade was a popular drink and it still is";
	char* decodedstr;
	u8int* encodedstr;
	u8int replen;

	h_inittree();
	print("inited the tree\n");
	encodedstr = calloc(20, sizeof(u8int));
	decodedstr = calloc(strlen(str), sizeof(char));
	print("encoding %s\n", str);
	h_huffmanenc((u8int*) str, encodedstr, strlen(str), &replen);
	print("got encoded size: %d\n", replen);
	for(int i = 0; i < replen; i++)
		print("%x ", encodedstr[i]);
	print("\n");
	print("decoding...");
	h_huffmandec((u8int*) decodedstr, encodedstr, replen);
	print("got %s\n", decodedstr);
	free(encodedstr);
	free(decodedstr);
	h_freenode(rootnode);
	exits(0);
}
