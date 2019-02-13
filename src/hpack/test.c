#include <u.h>
#include <libc.h>

#include "../http2fs.h"
#include "h_hpack.h"

void
main(int argc, char** argv)
{
	char* defaultstr = "lemonade was a popular drink and it still is";
	char* str;
	char* decodedstr;
	u8int* encodedstr;
	u8int replen;

	h_inittree();
	print("inited the tree\n");
	str = readcons("str ", defaultstr, 0);
	print("got %s of size %d\n", str, strlen(str));
	encodedstr = calloc(strlen(str), sizeof(u8int));
	decodedstr = calloc(strlen(str), sizeof(char));
	print("encoding %s\n", str);
	h_huffmanenc((u8int*) str, encodedstr, strlen(str), &replen);
	print("got encoded size: %d\n representation: ", replen);
	for(int i = 0; i < replen; i++)
		print("%x ", encodedstr[i]);
	print("\n");
	print("decoding...");
	h_huffmandec((u8int*) decodedstr, encodedstr, replen);
	print("got decoded string: %s\n", decodedstr);
	free(encodedstr);
	free(decodedstr);
	h_freenode(rootnode);
	exits(0);
}
